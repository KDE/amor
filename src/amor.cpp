/*
 * Copyright 1999 by Martin R. Jones <mjones@kde.org>
 * Copyright 2010 by Stefan Böhmann <kde@hilefoks.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#include "amor.h"
#include "amorpixmapmanager.h"
#include "amorbubble.h"
#include "amorwidget.h"
#include "amordialog.h"
#include "version.h"
#include "queueitem.h"
#include "amorthememanager.h"
#include "amoradaptor.h"
#include "amor_debug.h"

#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include <QDBusConnection>
#include <QTimer>
#include <QCursor>
#include <QStandardPaths>
#include <QApplication>
#include <QMenu>

#include <KLocalizedString>
#include <KMessageBox>
#include <KStartupInfo>
#include <KWindowInfo>
#include <KHelpMenu>
#include <KRandom>
#include <KAboutData>

#if defined Q_OS_LINUX
#include <xcb/xcb.h>
#include <QX11Info>
#endif

// #define DEBUG_AMOR

#define SLEEP_TIMEOUT   180     // Animation sleeps after SLEEP_TIMEOUT seconds
                                // of mouse inactivity.
#define TIPS_FILE       "tips-en"  // Display tips in TIP_FILE-LANG, e.g "tips-en" (this is then translated using i18n() at runtime)
#define TIP_FREQUENCY   20      // Frequency tips are displayed small == more often.

#define BUBBLE_TIME_STEP 250

// Standard animation groups
#define ANIM_BASE       "Base"
#define ANIM_NORMAL     "Sequences"
#define ANIM_FOCUS      "Focus"
#define ANIM_BLUR       "Blur"
#define ANIM_DESTROY    "Destroy"
#define ANIM_SLEEP      "Sleep"
#define ANIM_WAKE       "Wake"



Amor::Amor()
  : mAmor( 0 ),
    mBubble( 0 ),
    mForceHideAmorWidget( false )
{
    new AmorAdaptor( this );
    QDBusConnection::sessionBus().registerObject( QLatin1String( "/Amor" ), this );

    if( !readConfig() ) {
        exit(0);
    }

    mTargetWin   = 0;
    mNextTarget  = 0;
    mMenu        = 0;
    mCurrAnim    = mBaseAnim;
    mPosition    = -1;
    mState       = Normal;

    mWin = KWindowSystem::self();
    connect(mWin, &KWindowSystem::activeWindowChanged, this, &Amor::slotWindowActivate);
    connect(mWin, &KWindowSystem::windowRemoved, this, &Amor::slotWindowRemove);
    connect(mWin, &KWindowSystem::stackingOrderChanged, this, &Amor::slotStackingChanged);
    connect(mWin, QOverload<WId,NET::Properties,NET::Properties2>::of(&KWindowSystem::windowChanged),
            this, &Amor::slotWindowChange);
    connect(mWin, &KWindowSystem::currentDesktopChanged, this, &Amor::slotDesktopChange);

    mAmor = new AmorWidget;
    connect( mAmor, SIGNAL(mouseClicked(QPoint)), SLOT(slotMouseClicked(QPoint)) );
    connect( mAmor, SIGNAL(dragged(QPoint,bool)), SLOT(slotWidgetDragged(QPoint,bool)) );
    mAmor->resize(mTheme.maximumSize());

    mTimer = new QTimer( this );
    connect( mTimer, SIGNAL(timeout()), SLOT(slotTimeout()) );

    mStackTimer = new QTimer( this );
    connect( mStackTimer, SIGNAL(timeout()), SLOT(restack()) );

    mBubbleTimer = new QTimer( this );
    connect( mBubbleTimer, SIGNAL(timeout()), SLOT(slotBubbleTimeout()) );

    std::time( &mActiveTime );
    mCursPos = QCursor::pos();
    mCursorTimer = new QTimer( this );
    connect( mCursorTimer, SIGNAL(timeout()), SLOT(slotCursorTimeout()) );
    mCursorTimer->start( 500 );

    mNextTarget = mWin->activeWindow();
    selectAnimation( Focus );
    mTimer->setSingleShot( true );
    mTimer->start( 0 );

    if( !QDBusConnection::sessionBus().connect( QString(), QString(), QLatin1String( "org.kde.amor" ),
            QLatin1String( "KDE_stop_screensaver" ), this, SLOT(screenSaverStopped()) ) )
    {
        qCDebug(AMOR_LOG) << "Could not attach DBus signal: KDE_stop_screensaver()";
    }

    if( !QDBusConnection::sessionBus().connect( QString(), QString(), QLatin1String( "org.kde.amor" ),
            QLatin1String( "KDE_start_screensaver" ), this, SLOT(screenSaverStarted()) ) )
    {
        qCDebug(AMOR_LOG) << "Could not attach DBus signal: KDE_start_screensaver()";
    }

    KStartupInfo::appStarted();
}


Amor::~Amor()
{
    qDeleteAll( mTipsQueue );
    mTipsQueue.clear();
    delete mAmor;
    delete mBubble;
}


void Amor::screenSaverStopped()
{
    mAmor->show();
    mForceHideAmorWidget = false;

    mTimer->setSingleShot( true );
    mTimer->start( 0 );
}


void Amor::screenSaverStarted()
{
    mAmor->hide();
    mTimer->stop();
    mForceHideAmorWidget = true;

    // GP: hide the bubble (if there's any) leaving any current message in the queue
    hideBubble();
}


void Amor::showTip(const QString &tip)
{
    if( mTipsQueue.count() < 5 && !mForceHideAmorWidget ) { // start dropping tips if the queue is too long
        mTipsQueue.enqueue( new QueueItem( QueueItem::Tip, tip ) );
    }

    if( mState == Sleeping ) {
        selectAnimation( Waking );        // Set waking immediatedly
        mTimer->setSingleShot( true );
        mTimer->start( 0 );
    }
}


void Amor::showMessage( const QString &message , int msec )
{
    // FIXME: What should be done about messages and tips while the screensaver is on?
    if( mForceHideAmorWidget ) {
        return; // do not show messages sent while in the screensaver
    }

    mTipsQueue.enqueue( new QueueItem( QueueItem::Talk, message, msec ) );

    if( mState == Sleeping ) {
        selectAnimation( Waking );      // Set waking immediatedly
        mTimer->setSingleShot( true );
        mTimer->start( 0 );
    }
}


void Amor::reset()
{
    hideBubble();
    mAmor->setPixmap( 0L ); // get rid of your old copy of the pixmap

    AmorPixmapManager::manager()->reset();
    mTips.reset();

    readConfig();

    mCurrAnim = mBaseAnim;
    mPosition = mCurrAnim->hotspot().x();
    mState = Normal;

    mAmor->resize( mTheme.maximumSize() );
    mCurrAnim->reset();

    mTimer->setSingleShot( true );
    mTimer->start( 0 );
}


bool Amor::readConfig()
{
    // Read user preferences
    mConfig.read();

    if( mConfig.mTips ) {
        mTips.setFile(QLatin1String( TIPS_FILE ) );
    }

    // Select a random theme if user requested it
    if( mConfig.mRandomTheme ) {
        QStringList files;
        // Store relative paths into files to avoid storing absolute pathnames.
        const QStringList dirs = QStandardPaths::standardLocations(QStandardPaths::AppDataLocation);;
        for (const QString& dir : dirs) {
            const QStringList fileNames = QDir(dir).entryList(QStringList() << QStringLiteral("*rc"));
            for (const QString& file : fileNames) {
                files.append(dir + QLatin1Char('/') + file);
            }
        }
        if (files.isEmpty()) {
            return false;
        }
        const int randomTheme = KRandom::random() % files.count();
        mConfig.mTheme = files.at(randomTheme);
    }

    // read selected theme
    if( !mTheme.setTheme( mConfig.mTheme ) ) {
        KMessageBox::error( 0, i18nc( "@info:status", "Error reading theme: " ) + mConfig.mTheme );
        return false;
    }

    if( !mTheme.isStatic() ) {
        const char *groups[] = { ANIM_BASE, ANIM_NORMAL, ANIM_FOCUS, ANIM_BLUR, ANIM_DESTROY, ANIM_SLEEP, ANIM_WAKE, 0 };

        // Read all the standard animation groups
        for(int i = 0; groups[i]; ++i) {
            if( !mTheme.readGroup(QLatin1String( groups[i] ) ) ) {
                KMessageBox::error( 0, i18nc( "@info:status", "Error reading group: " ) + QLatin1String( groups[i] ) );
                return false;
            }
        }
    }
    else {
        if( !mTheme.readGroup(QLatin1String( ANIM_BASE ) ) ) {
            KMessageBox::error( 0, i18nc( "@info:status", "Error reading group: " ) + QLatin1String( ANIM_BASE ) );
            return false;
        }
    }

    // Get the base animation
    mBaseAnim = mTheme.random(QLatin1String( ANIM_BASE ) );

    return true;
}


void Amor::showBubble()
{
    if( !mTipsQueue.isEmpty() ) {
        if( !mBubble ) {
            mBubble = new AmorBubble;
        }

        mBubble->setOrigin( mAmor->x()+mAmor->width()/2, mAmor->y()+mAmor->height()/2 );
        mBubble->setMessage( mTipsQueue.head()->text() );

        // mBubbleTimer->start(mTipsQueue.head()->time(), true);
        mBubbleTimer->setSingleShot(true);
        mBubbleTimer->start(BUBBLE_TIME_STEP);
    }
}


void Amor::hideBubble(bool forceDequeue)
{
    if( mBubble ) {
        // GP: stop mBubbleTimer to avoid deleting the first element, just in case we are changing windows
        // or something before the tip was shown long enough
        mBubbleTimer->stop();

        // GP: the first message on the queue should be taken off for a
        // number of reasons: a) forceDequeue == true, only when called
        // from slotBubbleTimeout; b) the bubble is not visible ; c)
        // the bubble is visible, but there's Tip being displayed. The
        // latter is to keep backwards compatibility and because
        // carrying around a tip bubble when switching windows quickly is really
        // annoyying
        if( forceDequeue || !mBubble->isVisible() || ( mTipsQueue.head()->type() == QueueItem::Tip ) ) {
            /* there's always an item in the queue here */
            mTipsQueue.dequeue();
        }

        delete mBubble;
        mBubble = 0;
    }
}


void Amor::selectAnimation(State state)
{
    bool changedLocation = true;
    AmorAnimation *oldAnim = mCurrAnim;

    switch( state ) {
    case Blur:
        hideBubble();
        mCurrAnim = mTheme.random(QLatin1String( ANIM_BLUR ) );
        mState = Focus;
        break;

    case Focus:
        hideBubble();
        mCurrAnim = mTheme.random(QLatin1String( ANIM_FOCUS ) );
        if( oldAnim != mCurrAnim ) {
            mCurrAnim->reset();
        }

        mTargetWin = mNextTarget;

        if( mTargetWin != XCB_NONE ) {
            KWindowInfo windowInfo( mTargetWin, NET::WMFrameExtents );
            mTargetRect = windowInfo.frameGeometry();

            // if the animation falls outside of the working area,
            // then relocate it so that is inside the desktop again
            QRect desktopArea = mWin->workArea(KWindowSystem::currentDesktop());
            KWindowSystem::setOnDesktop(mAmor->winId(), KWindowSystem::currentDesktop());

            if( mTargetRect.y() - mCurrAnim->hotspot().y() + mConfig.mOffset < desktopArea.y() ) {
                if( mInDesktopBottom ) {
                    changedLocation = false;
                }

                // relocate the animation at the bottom of the screen
                mTargetRect = QRect( desktopArea.x(), desktopArea.y() + desktopArea.height(), desktopArea.width(), 0 );

                // we'll relocate the animation in the desktop
                // frame, so do not add the offset to its vertical position
                mInDesktopBottom = true;
            }
            else {
                mInDesktopBottom = false;
            }

            if( mTheme.isStatic() ) {
                if( mConfig.mStaticPos < 0 ) {
                    mPosition = mTargetRect.width() + mConfig.mStaticPos;
                }
                else {
                    mPosition = mConfig.mStaticPos;
                }

                if( mPosition >= mTargetRect.width() ) {
                    mPosition = mTargetRect.width()-1;
                }
                else if( mPosition < 0 ) {
                    mPosition = 0;
                }
            }
            else {
                if( mCurrAnim->frame() ) {
                    if( mTargetRect.width() == mCurrAnim->frame()->width() ) {
                        mPosition = mCurrAnim->hotspot().x();
                    }
                    else if(changedLocation || mPosition < 0) {
                        mPosition = KRandom::random() % ( mTargetRect.width() - mCurrAnim->frame()->width() );
                        mPosition += mCurrAnim->hotspot().x();
                    }
                }
                else {
                    mPosition = mTargetRect.width()/2;
                }
            }
        }
        else {
            // We don't want to do anything until a window comes into focus.
            mTimer->stop();
        }
        mAmor->hide();
        restack();
        mState = Normal;
        break;

    case Destroy:
        hideBubble();
        mCurrAnim = mTheme.random(QLatin1String( ANIM_DESTROY ) );
        mState = Focus;
        break;

    case Sleeping:
        mCurrAnim = mTheme.random(QLatin1String( ANIM_SLEEP ) );
        break;

    case Waking:
        mCurrAnim = mTheme.random(QLatin1String( ANIM_WAKE ) );
        mState = Normal;
        break;

    default:
        // Select a random normal animation if the current animation
        // is not the base, otherwise select the base.  This makes us
        // alternate between the base animation and a random animination.
        if( !mBubble && mCurrAnim == mBaseAnim ) {
            mCurrAnim = mTheme.random(QLatin1String( ANIM_NORMAL ) );
        }
        else {
            mCurrAnim = mBaseAnim;
        }
        break;
    }

    if( mCurrAnim->totalMovement() + mPosition > mTargetRect.width() || mCurrAnim->totalMovement() + mPosition < 0 ) {
        // The selected animation would end outside of this window's width
        // We could randomly select a different one, but I prefer to just
        // use the default animation.
        mCurrAnim = mBaseAnim;
    }

    if( changedLocation ) {
        mCurrAnim->reset();
    }
    else {
        mCurrAnim = oldAnim;
    }
}


void Amor::restack()
{
    if( mTargetWin == XCB_NONE ) {
        return;
    }

    if( mConfig.mOnTop ) {
        // simply raise the widget to the top
        mAmor->raise();
        return;
    }

#ifdef Q_OS_LINUX
    xcb_window_t sibling = mTargetWin;
    xcb_window_t dw, parent = XCB_NONE, *wins;

    do {
        unsigned int nwins = 0;

        // We must use the target window's parent as our sibling.
        // Is there a faster way to get parent window than XQueryTree?
        const auto cookie = xcb_query_tree(QX11Info::connection(), sibling);
        const auto reply = xcb_query_tree_reply(QX11Info::connection(), cookie, nullptr);
        if (!reply) {
            return;
        }

        nwins = xcb_query_tree_children_length(reply);
        dw = reply->root;
        parent = reply->parent;

        free(reply);

        if( parent != XCB_NONE && parent != dw ) {
            sibling = parent;
        }
    } while( parent != XCB_NONE && parent != dw );

    // Set animation's stacking order to be above the window manager's
    // decoration of target window.
    const uint32_t values[] = { sibling, XCB_STACK_MODE_ABOVE };
    xcb_configure_window(QX11Info::connection(), mAmor->winId(),
                         XCB_CONFIG_WINDOW_SIBLING | XCB_CONFIG_WINDOW_STACK_MODE,
                         values);
#endif
}


void Amor::slotMouseClicked(const QPoint &pos)
{
    bool restartTimer = mTimer->isActive();

    // Stop the animation while the menu is open.
    if( restartTimer ) {
        mTimer->stop();
    }

    if( !mMenu ) {
        KHelpMenu* help = new KHelpMenu(0, KAboutData::applicationData(), false );
        QMenu* helpMenu = help->menu();
#ifdef __GNUC__
#warning the following is kinda dirty and should be done by KHelpMenu::menu() I think. (hermier)
#endif
        helpMenu->setIcon(QIcon::fromTheme(QStringLiteral("help-contents")));
        helpMenu->setTitle( i18nc( "@action:inmenu Amor", "&Help" ) );

        mMenu = new QMenu( 0 );
        mMenu->setTitle( QLatin1String( "Amor" ) ); // I really don't want this i18n'ed
        mMenu->addAction(QIcon::fromTheme(QStringLiteral("configure")), i18nc( "@action:inmenu Amor", "&Configure..." ), this, SLOT(slotConfigure()) );
        mMenu->addSeparator();
        mMenu->addMenu( helpMenu );
        mMenu->addAction(QIcon::fromTheme(QStringLiteral("application-exit")), i18nc( "@action:inmenu Amor", "&Quit" ), qApp, SLOT(quit()) );
    }

    mMenu->exec( pos );

    if( restartTimer ) {
        mTimer->setSingleShot( true );
        mTimer->start( 1000 );
    }
}


void Amor::slotCursorTimeout()
{
    QPoint currPos = QCursor::pos();
    QPoint diff = currPos - mCursPos;
    std::time_t now = std::time( 0 );

    if( mForceHideAmorWidget ) {
        return; // we're hidden, do nothing
    }

    if( abs( diff.x() ) > 1 || abs( diff.y() ) > 1 ) {
        if( mState == Sleeping ) {
            // Set waking immediatedly
            selectAnimation( Waking );
        }
        mActiveTime = now;
        mCursPos = currPos;
    }
    else if( mState != Sleeping && now - mActiveTime > SLEEP_TIMEOUT ) {
        // GP: can't go to sleep if there are tips in the queue
        if( mTipsQueue.isEmpty() ) {
            mState = Sleeping;	// The next animation will become sleeping
        }
    }
}


void Amor::slotTimeout()
{
    if( mForceHideAmorWidget ) {
        return;
    }

    if( !mTheme.isStatic() ) {
        mPosition += mCurrAnim->movement();
    }

    mAmor->setPixmap( mCurrAnim->frame() );
    mAmor->move( mTargetRect.x() + mPosition - mCurrAnim->hotspot().x(),
                 mTargetRect.y() - mCurrAnim->hotspot().y() + ( !mInDesktopBottom?mConfig.mOffset:0 ) );

    if( !mAmor->isVisible() ) {
        mAmor->show();
        restack();
    }

    if( mCurrAnim == mBaseAnim && mCurrAnim->validFrame() ) {
        // GP: Application tips/messages can be shown in any frame number; amor tips are
        // only displayed on the first frame of mBaseAnim (the old way of doing this).
        if( !mTipsQueue.isEmpty() && !mBubble &&  mConfig.mAppTips ) {
            showBubble();
        }
        else if( KRandom::random()%TIP_FREQUENCY == 1 && mConfig.mTips && !mBubble && !mCurrAnim->frameNum() ) {
            mTipsQueue.enqueue( new QueueItem( QueueItem::Tip, mTips.tip() ) );
            showBubble();
        }
    }

    if( mTheme.isStatic() ) {
        mTimer->setSingleShot( true );
        mTimer->start( ( mState == Normal ) || ( mState == Sleeping ) ? 1000 : 100 );
    }
    else {
        mTimer->setSingleShot( true );
        mTimer->start( mCurrAnim->delay() );
    }

    if( !mCurrAnim->next() ) {
        if( mBubble ) {
            mCurrAnim->reset();
        }
        else {
            selectAnimation( mState );
        }
    }
}


void Amor::slotConfigure()
{
    AmorDialog *mAmorDialog = new AmorDialog();
    connect( mAmorDialog, SIGNAL(changed()), SLOT(slotConfigChanged()) );
    connect( mAmorDialog, SIGNAL(offsetChanged(int)), SLOT(slotOffsetChanged(int)) );
    mAmorDialog->show();
}


void Amor::slotConfigChanged()
{
    reset();
}


void Amor::slotOffsetChanged(int off)
{
    mConfig.mOffset = off;

    if( mCurrAnim->frame() ) {
        mAmor->move( mPosition + mTargetRect.x() - mCurrAnim->hotspot().x(),
                     mTargetRect.y() - mCurrAnim->hotspot().y() + ( !mInDesktopBottom ? mConfig.mOffset : 0 ) );
    }
}


void Amor::slotWidgetDragged(const QPoint &delta, bool release)
{
    if( mCurrAnim->frame() ) {
        int newPosition = mPosition + delta.x();
        if( mCurrAnim->totalMovement() + newPosition > mTargetRect.width() ) {
            newPosition = mTargetRect.width() - mCurrAnim->totalMovement();
        }
        else if( mCurrAnim->totalMovement() + newPosition < 0 ) {
            newPosition = -mCurrAnim->totalMovement();
        }
        mPosition = newPosition;
        mAmor->move( mTargetRect.x() + mPosition - mCurrAnim->hotspot().x(), mAmor->y() );

        if( mTheme.isStatic() && release ) {
            // static animations save the new position as preferred.
            int savePos = mPosition;
            if( savePos > mTargetRect.width()/2 ) {
                savePos -= (mTargetRect.width()+1);
            }
            mConfig.mStaticPos = savePos;
            mConfig.write();
        }
    }
}


void Amor::slotWindowActivate(WId win)
{

    // We don't fit on top of this window, see if we can find another one

    KWindowInfo windowInfo( win, NET::WMFrameExtents );
    const QRect desktopArea = mWin->workArea(KWindowSystem::currentDesktop());
    if( windowInfo.frameGeometry().y() - mCurrAnim->hotspot().y() + mConfig.mOffset < desktopArea.y() ) {
        for (const WId windowId : KWindowSystem::windows()) {
            windowInfo = KWindowInfo( windowId, NET::WMFrameExtents | NET::WMGeometry );

            if( windowInfo.frameGeometry().y() - mCurrAnim->hotspot().y() + mConfig.mOffset < desktopArea.y() ) {
                continue;
            }

            win = windowId;
            break;
        }
    }

    mTimer->stop();
    mNextTarget = win;

    // This is an active event that affects the target window
    std::time( &mActiveTime );

    // A window gaining focus implies that the current window has lost
    // focus.  Initiate a blur event if there is a current active window.
    if( mTargetWin ) {
        // We are losing focus from the current window
        mTimer->setSingleShot( true );
        mTimer->start( 0 );
    }
    else if( mNextTarget ) {
        // We are setting focus to a new window
        if( mState != Focus ) {
            selectAnimation( Focus );
        }
        mTimer->setSingleShot( true );
        mTimer->start( 0 );
    }
    else {
        // No action - We can get this when we switch between two empty desktops
        mAmor->hide();
    }
}


void Amor::slotWindowRemove(WId win)
{
    if( win == mTargetWin ) {
        // This is an active event that affects the target window
        std::time( &mActiveTime );

        selectAnimation( Destroy );
        mTimer->stop();
        mTimer->setSingleShot( true );
        mTimer->start( 0 );
    }
}


void Amor::slotStackingChanged()
{
    // This is an active event that affects the target window
    std::time( &mActiveTime );

    // We seem to get this signal before the window has been restacked,
    // so we just schedule a restack.
    mStackTimer->setSingleShot( true );
    mStackTimer->start( 20 );
}


void Amor::slotWindowChange(WId win, NET::Properties properties, NET::Properties2 properties2)
{
    if( win != mTargetWin ) {
        return;
    }

    // This is an active event that affects the target window
    std::time( &mActiveTime );

    KWindowInfo windowInfo( mTargetWin, NET::WMFrameExtents );
    NET::MappingState mappingState = windowInfo.mappingState();

    if( mappingState == NET::Iconic || mappingState == NET::Withdrawn ) {
        // The target window has been iconified
        selectAnimation( Destroy );
        mTargetWin = XCB_NONE;
        mTimer->stop();
        mTimer->setSingleShot( true );
        mTimer->start( 0 );

        return;
    }

    if( properties & NET::WMGeometry ) {
        QRect newTargetRect = windowInfo.frameGeometry();

        // if the change in the window caused the animation to fall
        // out of the working area of the desktop, or if the animation
        // didn't fall in the working area before but it does now, then
        //  refocus on the current window so that the animation is
        // relocated.
        QRect desktopArea = mWin->workArea();

        bool fitsInWorkArea = !( newTargetRect.y() - mCurrAnim->hotspot().y() + mConfig.mOffset < desktopArea.y() );
        if( ( !fitsInWorkArea && !mInDesktopBottom ) || ( fitsInWorkArea && mInDesktopBottom ) ) {
            mNextTarget = mTargetWin;
            selectAnimation( Blur );
            mTimer->setSingleShot( true );
            mTimer->start( 0 );

            return;
        }

        if( !mInDesktopBottom ) {
            mTargetRect = newTargetRect;
        }

        // make sure the animation is still on the window.
        if( mCurrAnim->frame() ) {
            hideBubble();
            if( mTheme.isStatic() ) {
                if( mConfig.mStaticPos < 0 ) {
                    mPosition = mTargetRect.width() + mConfig.mStaticPos;
                }
                else {
                    mPosition = mConfig.mStaticPos;
                }

                if( mPosition >= mTargetRect.width() ) {
                    mPosition = mTargetRect.width()-1;
                }
                else if( mPosition < 0 ) {
                    mPosition = 0;
                }
            }
            else if( mPosition > mTargetRect.width() - ( mCurrAnim->frame()->width() - mCurrAnim->hotspot().x() ) ) {
                mPosition = mTargetRect.width() - ( mCurrAnim->frame()->width() - mCurrAnim->hotspot().x() );
            }
            mAmor->move( mTargetRect.x() + mPosition - mCurrAnim->hotspot().x(),
                         mTargetRect.y() - mCurrAnim->hotspot().y() + ( !mInDesktopBottom ? mConfig.mOffset : 0 ) );
        }
    }
}


void Amor::slotDesktopChange(int desktop)
{
    // GP: signal currentDesktopChanged seems to be emitted even if you
    // change to the very same desktop you are in.
    if( mWin->currentDesktop() == desktop ) {
        return;
    }

    mNextTarget = XCB_NONE;
    mTargetWin = XCB_NONE;
    selectAnimation( Normal );
    mTimer->stop();
    mAmor->hide();
}


void Amor::slotBubbleTimeout()
{
    // has the queue item been displayed for long enough?
    QueueItem *first = mTipsQueue.head();

    if( first->time() > BUBBLE_TIME_STEP && mBubble->isVisible() ) {
        first->setTime( first->time() - BUBBLE_TIME_STEP );
        mBubbleTimer->setSingleShot( true );
        mBubbleTimer->start( BUBBLE_TIME_STEP );
        return;
    }

    // do not do anything if the mouse pointer is in the bubble
    if( mBubble->mouseWithin() ) {
        first->setTime( 500 );                  // show this item for another 500ms
        mBubbleTimer->setSingleShot( true );
        mBubbleTimer->start( BUBBLE_TIME_STEP );
        return;
    }

    // are there any other tips pending?
    if( mTipsQueue.count() > 1 ) {
        mTipsQueue.dequeue();
        showBubble();           // shows the next item in the queue
    }
    else {
        hideBubble( true );     // hideBubble calls dequeue() for itself.
    }
}


// kate: word-wrap off; encoding utf-8; indent-width 4; tab-width 4; line-numbers on; mixed-indent off; remove-trailing-space-save on; replace-tabs-save on; replace-tabs on; space-indent on;
// vim:set spell et sw=4 ts=4 nowrap cino=l1,cs,U1:

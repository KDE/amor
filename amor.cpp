//---------------------------------------------------------------------------
//
// amor.cpp
//
// Copyright (c) 1999 Martin R. Jones <mjones@kde.org>
//

#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include <qmessagebox.h>
#include <qpopupmenu.h>

#include <klocale.h>
#include <ksimpleconfig.h>
#include <kmessagebox.h>
#include <kwin.h>
#include <kwinmodule.h>
#include <kapp.h>

#include "amor.h"
#include "amor.moc"
#include "amorpm.h"
#include "amorbubble.h"
#include "version.h"
#include <kdebug.h>
#include <X11/Xlib.h>

#define SLEEP_TIMEOUT   180     // Animation sleeps after SLEEP_TIMEOUT seconds
                                // of mouse inactivity.
#define BUBBLE_TIMEOUT  4000    // Minimum milliseconds to display a tip
#define TIPS_FILE       "tips"  // Display tips in TIP_FILE-LANG, e.g "tips-en"
#define TIP_FREQUENCY   20      // Frequency tips are displayed small == more
                                // often.

// Standard animation groups
#define ANIM_BASE       "Base"
#define ANIM_NORMAL     "Sequences"
#define ANIM_FOCUS      "Focus"
#define ANIM_BLUR       "Blur"
#define ANIM_DESTROY    "Destroy"
#define ANIM_SLEEP      "Sleep"
#define ANIM_WAKE       "Wake"

//---------------------------------------------------------------------------
//
// Constructor
//
Amor::Amor() : QObject()
{
    mAmor = 0;
    mBubble = 0;

    if (readConfig())
    {
        mTargetWin   = 0;
        mNextTarget  = 0;
        mAmorDialog  = 0;
        mMenu        = 0;
        mCurrAnim    = mBaseAnim;
        mPosition    = mCurrAnim->hotspot().x();
        mState       = Normal;
        mResizeId    = 0;
        mCursId      = 0;
        mBubbleId    = 0;

        mWin = new KWinModule;
        connect(mWin, SIGNAL(activeWindowChanged(WId)),
                this, SLOT(slotWindowActivate(WId)));
        connect(mWin, SIGNAL(windowRemoved(WId)),
                this, SLOT(slotWindowRemove(WId)));
        connect(mWin, SIGNAL(stackingOrderChanged()),
                this, SLOT(slotStackingChanged()));
        connect(mWin, SIGNAL(windowChanged(WId)),
                this, SLOT(slotWindowChange(WId)));

        mAmor = new AmorWidget();
        connect(mAmor, SIGNAL(mouseClicked(const QPoint &)),
                        SLOT(slotMouseClicked(const QPoint &)));
        mAmor->resize(mTheme.maximumSize());

        mTimer = new QTimer(this);
        connect(mTimer, SIGNAL(timeout()), SLOT(slotTimeout()));

        time(&mActiveTime);
        mCursPos = QCursor::pos();
        mCursId = startTimer(200);

        if (mWin->activeWindow())
        {
            mNextTarget = mWin->activeWindow();
            selectAnimation(Focus);
            mTimer->start(0, true);
        }
    }
    else
    {
        kapp->quit();
    }
}

//---------------------------------------------------------------------------
//
// Destructor
//
Amor::~Amor()
{
    delete mWin;
    delete mAmor;
    delete mBubble;
}

//---------------------------------------------------------------------------
//
// Clear existing theme and reload configuration
//
void Amor::reset()
{
    mTimer->stop();

    AmorPixmapManager::manager()->reset();
    mTips.reset();
    delete mAmor;

    readConfig();

    mTargetWin  = 0;
    mNextTarget = 0;
    mCurrAnim   = mBaseAnim;
    mPosition   = mCurrAnim->hotspot().x();
    mState      = Normal;

    mAmor = new AmorWidget();
    connect(mAmor, SIGNAL(mouseClicked(const QPoint &)),
                    SLOT(slotMouseClicked(const QPoint &)));
    mAmor->resize(mTheme.maximumSize());
}

//---------------------------------------------------------------------------
//
// Read the selected theme.
//
bool Amor::readConfig()
{
    // Read user preferences
    mConfig.read();

//    mConfig.mOnTop = true;       // XXX until normal mode is fixed

    if (mConfig.mTips)
    {
        mTips.setFile(TIPS_FILE);
    }

    // read selected theme
    if (!mTheme.setTheme(mConfig.mTheme))
    {
        KMessageBox::error(0, i18n("Error reading theme: ") + mConfig.mTheme);
        return false;
    }

    const char *groups[] = { ANIM_BASE, ANIM_NORMAL, ANIM_FOCUS, ANIM_BLUR,
                            ANIM_DESTROY, ANIM_SLEEP, ANIM_WAKE, 0 };

    // Read all the standard animation groups
    for (int i = 0; groups[i]; i++)
    {
        if (mTheme.readGroup(groups[i]) == false)
        {
            KMessageBox::error(0, i18n("Error reading group: ") + groups[i]);
            return false;
        }
    }

    // Get the base animation
    mBaseAnim = mTheme.random(ANIM_BASE);

    return true;
}

//---------------------------------------------------------------------------
//
// Show the bubble text
//
void Amor::showBubble(const QString& msg)
{
    if (msg)
    {
        if (!mBubble)
        {
            mBubble = new AmorBubble;
        }

        mBubble->setOrigin(mAmor->x()+mAmor->width()/2,
                           mAmor->y()+mAmor->height()/2);
        mBubble->setMessage(msg);
        mBubble->show();
        mBubbleId = startTimer(BUBBLE_TIMEOUT + msg.length() * 30);
    }
}

//---------------------------------------------------------------------------
//
// Hide the bubble text if visible
//
void Amor::hideBubble()
{
    if (mBubbleId)
    {
        killTimer(mBubbleId);
        mBubbleId = 0;
    }
    if (mBubble)
    {
        delete mBubble;
        mBubble = 0;
    }
}

//---------------------------------------------------------------------------
//
// Select a new animation appropriate for the current state.
//
void Amor::selectAnimation(State state)
{
    switch (state)
    {
        case Blur:
            hideBubble();
            mCurrAnim = mTheme.random(ANIM_BLUR);
            mState = Focus;
            break;

        case Focus:
            hideBubble();
            mCurrAnim = mTheme.random(ANIM_FOCUS);
            mCurrAnim->reset();
            mTargetWin = mNextTarget;
            if (mTargetWin != None)
            {
//                mTargetRect = windowGeometry(mTargetWin);
//                mTargetRect = KWM::geometry(mTargetWin, true);
                mTargetRect = KWin::info(mTargetWin).frameGeometry;
                if (mCurrAnim->frame())
                {
                    mPosition = (kapp->random() %
                        (mTargetRect.width() - mCurrAnim->frame()->width())) +
                         mCurrAnim->hotspot().x();
                }
                else
                {
                    mPosition = mTargetRect.width()/2;
                }
            }
            else
            {
                // We don't want to do anything until a window comes into
                // focus.
                mTimer->stop();
            }
            mAmor->hide();
            restack();
            mState = Normal;
            break;

        case Destroy:
            hideBubble();
            mCurrAnim = mTheme.random(ANIM_DESTROY);
            mState = Focus;
            break;

        case Sleeping:
            mCurrAnim = mTheme.random(ANIM_SLEEP);
            break;

        case Waking:
            mCurrAnim = mTheme.random(ANIM_WAKE);
            mState = Normal;
            break;

        default:
            // Select a random normal animation if the current animation
            // is not the base, otherwise select the base.  This makes us
            // alternate between the base animation and a random
            // animination.
            if (mCurrAnim == mBaseAnim && !mBubble)
            {
                mCurrAnim = mTheme.random(ANIM_NORMAL);
            }
            else
            {
                mCurrAnim = mBaseAnim;
            }
            break;
    }

    if (mCurrAnim->totalMovement() + mPosition > mTargetRect.width() ||
        mCurrAnim->totalMovement() + mPosition < 0)
    {
        // The selected animation would end outside of this window's width
        // We could randomly select a different one, but I prefer to just
        // use the default animation.
        mCurrAnim = mBaseAnim;
    }

    mCurrAnim->reset();
}

//---------------------------------------------------------------------------
//
// Set the animation's stacking order to be just above the target window's
// window decoration, or on top.
//
void Amor::restack()
{
    if (mTargetWin == None)
    {
        return;
    }

    if (mConfig.mOnTop)
    {
        // simply raise the widget to the top
        mAmor->raise();
        return;
    }

    kdDebug(10000) << "restacking" << endl;

    Window sibling = mTargetWin;
    Window dw, parent = None, *wins;

    do {
        unsigned int nwins = 0;

        // We must use the target window's parent as our sibling.
        // Is there a faster way to get parent window than XQueryTree?
        if (XQueryTree(qt_xdisplay(), sibling, &dw, &parent, &wins, &nwins))
        {
            if (nwins)
            {
                XFree(wins);
            }
        }

        if (parent != None && parent != dw )
            sibling = parent;
    } while ( parent != None && parent != dw );

    // Set animation's stacking order to be above the window manager's
    // decoration of target window.
    XWindowChanges values;
    values.sibling = sibling;
    values.stack_mode = Above;
    XConfigureWindow(qt_xdisplay(), mAmor->winId(), CWSibling | CWStackMode,
                     &values);
}

//---------------------------------------------------------------------------
//
QRect Amor::windowGeometry(WId win)
{
    QRect rect;
    XWindowAttributes attr;
    if (XGetWindowAttributes(qt_xdisplay(), win, &attr))
    {
        int x, y;
        Window child;
        XTranslateCoordinates(qt_xdisplay(), win, qt_xrootwin(),
                              0, 0, &x, &y, &child);
        rect.setRect(x, y, attr.width, attr.height);
    }

    kdDebug(10000) << "Window geometry: " << rect.x() << ", " << rect.y() << ", " << rect.width() << " x " << rect.height() << endl;

    return rect;
}

//---------------------------------------------------------------------------
//
// Handle various timer events.
//
void Amor::timerEvent(QTimerEvent *te)
{
    if (te->timerId() == mResizeId)
    {
        killTimer(mResizeId);
        mResizeId = 0;
        restack();
    }
    else if (te->timerId() == mCursId)
    {
        QPoint currPos = QCursor::pos();
        QPoint diff = currPos - mCursPos;
        time_t now = time(0);

        if (abs(diff.x()) > 1 || abs(diff.y()) > 1)
        {
            if (mState == Sleeping)
            {
                // Set waking immediatedly
                selectAnimation(Waking);
            }
            mActiveTime = now;
            mCursPos = currPos;
        }
        else if (mState != Sleeping && now - mActiveTime > SLEEP_TIMEOUT)
        {
            // The next animation will become sleeping
            mState = Sleeping;
        }
    }
    else if (te->timerId() == mBubbleId)
    {
        hideBubble();
    }
}

//---------------------------------------------------------------------------
//
// The user clicked on our animation.
//
void Amor::slotMouseClicked(const QPoint &pos)
{
    bool restartTimer = mTimer->isActive();

    // Stop the animation while the menu is open.
    if (restartTimer)
    {
        mTimer->stop();
    }

    if (!mMenu)
    {
        mMenu = new QPopupMenu();
        mMenu->insertItem(i18n("&Options..."), this, SLOT(slotConfigure()));
        mMenu->insertItem(i18n("&About..."), this, SLOT(slotAbout()));
        mMenu->insertSeparator();
        mMenu->insertItem(i18n("&Quit"), kapp, SLOT(quit()));
    }

    mMenu->exec(pos);

    if (restartTimer)
    {
        mTimer->start(1000, true);
    }
}

//---------------------------------------------------------------------------
//
// Display the next frame or a new animation
//
void Amor::slotTimeout()
{
    mPosition += mCurrAnim->movement();
    mAmor->setPixmap(mCurrAnim->frame());
    mAmor->move(mPosition + mTargetRect.x() - mCurrAnim->hotspot().x(),
                 mTargetRect.y() - mCurrAnim->hotspot().y() + mConfig.mOffset);
    if (!mAmor->isVisible())
    {
        mAmor->show();
        restack();
    }

    // At the start of a base animation, we can randomly display
    // a helpful tip.
    if (mCurrAnim == mBaseAnim && mCurrAnim->frameNum() == 0)
    {
        if (kapp->random()%TIP_FREQUENCY == 1 && mConfig.mTips && !mBubble)
        {
            showBubble(mTips.tip());
        }
    }

    mTimer->start(mCurrAnim->delay(), true);

    if (!mCurrAnim->next())
    {
        selectAnimation(mState);
    }
}

//---------------------------------------------------------------------------
//
// Display configuration dialog
//
void Amor::slotConfigure()
{
    if (!mAmorDialog)
    {
        mAmorDialog = new AmorDialog();
        connect(mAmorDialog, SIGNAL(changed()), SLOT(slotConfigChanged()));
        connect(mAmorDialog, SIGNAL(offsetChanged(int)),
                SLOT(slotOffsetChanged(int)));
    }

    mAmorDialog->show();
}

//---------------------------------------------------------------------------
//
// Configuration changed.
//
void Amor::slotConfigChanged()
{
    reset();
}

//---------------------------------------------------------------------------
//
// Offset changed
//
void Amor::slotOffsetChanged(int off)
{
    mConfig.mOffset = off;

    if (mCurrAnim->frame())
    {
        mAmor->move(mPosition + mTargetRect.x() - mCurrAnim->hotspot().x(),
                 mTargetRect.y() - mCurrAnim->hotspot().y() + mConfig.mOffset);
    }
}

//---------------------------------------------------------------------------
//
// Display About box
//
void Amor::slotAbout()
{
    QString about = i18n("Amor Version %1\n\n").arg(AMOR_VERSION) +
                i18n("Amusing Misuse Of Resources\n\n") +
                i18n("Copyright (c) 1999 Martin R. Jones <mjones@kde.org>\n") +
                "\nhttp://www.powerup.com.au/~mjones/amor/";
    QMessageBox mb;
    mb.setText(about);
    mb.setCaption(i18n("About Amor"));
    mb.setIcon(QMessageBox::Information);
    mb.show();
}

//---------------------------------------------------------------------------
//
// Focus changed to a different window
//
void Amor::slotWindowActivate(WId win)
{
    kdDebug(10000) << "Window activated" << endl;

    mTimer->stop();
    mNextTarget = win;

    // This is an active event that affects the target window
    time(&mActiveTime);

    // A window gaining focus implies that the current window has lost
    // focus.  Initiate a blur event if there is a current active window.
    if (mTargetWin)
    {
        // We are losing focus from the current window
        selectAnimation(Blur);
        mTimer->start(0, true);
    }
    else if (mNextTarget)
    {
        // We are setting focus to a new window
        mState = Focus;
        mTimer->start(0, true);
    }
    else
    {
        // No action - We can get this when we switch between two empty
        // desktops
        mAmor->hide();
    }
}

//---------------------------------------------------------------------------
//
// Window removed
//
void Amor::slotWindowRemove(WId win)
{
    kdDebug(10000) << "Window removed" << endl;
    if (win == mTargetWin)
    {
        // This is an active event that affects the target window
        time(&mActiveTime);

        selectAnimation(Destroy);
        mTimer->stop();
        mTimer->start(0, true);
    }
}

//---------------------------------------------------------------------------
//
// Window stacking changed
//
void Amor::slotStackingChanged()
{
    kdDebug(10000) << "Stacking changed" << endl;

    // This is an active event that affects the target window
    time(&mActiveTime);

    // We seem to get this signal before the window has been restacked,
    // so we just schedule a restack.
    mResizeId = startTimer(20);
}

//---------------------------------------------------------------------------
//
// Properties of a window changed
//
void Amor::slotWindowChange(WId win)
{
    kdDebug(10000) << "Window changed" << endl;

    if (win != mTargetWin)
    {
        return;
    }

    // This is an active event that affects the target window
    time(&mActiveTime);

    KWin::Info info = KWin::info( mTargetWin );

    if (info.isIconified() ||
        info.mappingState == NET::Withdrawn)
    {
        kdDebug(10000) << "Iconic" << endl;
        // The target window has been iconified
        selectAnimation(Destroy);
        mTargetWin = None;
        mTimer->stop();
        mTimer->start(0, true);
    }
    else
    {
        // The size or position of the window has changed.
//        mTargetRect = windowGeometry(mTargetWin);
        mTargetRect = info.frameGeometry;

        // make sure the animation is still on the window.
        if (mCurrAnim->frame())
        {
            hideBubble();
            if (mPosition > mTargetRect.width() -
                    (mCurrAnim->frame()->width() - mCurrAnim->hotspot().x()))
            {
                mPosition = mTargetRect.width() -
                    (mCurrAnim->frame()->width() - mCurrAnim->hotspot().x());
            }
            mAmor->move(mPosition + mTargetRect.x() - mCurrAnim->hotspot().x(),
                     mTargetRect.y() - mCurrAnim->hotspot().y() +
                     mConfig.mOffset);
        }
    }
}

//===========================================================================

AmorSessionWidget::AmorSessionWidget()
{
    // the only function of this widget is to catch & forward the
    // saveYourself() signal from the session manager
    connect(kapp, SIGNAL(saveYourself()), SLOT(wm_saveyourself()));
}

void AmorSessionWidget::wm_saveyourself()
{
    // no action required currently.
}


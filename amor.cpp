/* amor.cpp
**
** Copyright (c) 1999 Martin R. Jones <mjones@kde.org>
**
*/

/*
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program in a file called COPYING; if not, write to
** the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston,
** MA 02111-1307, USA.
*/

/*
** Bug reports and questions can be sent to kde-devel@kde.org
*/
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include <kdebug.h>

#include <kpopupmenu.h>
#include <qtimer.h>
#include <qcursor.h>

#include <klocale.h>
#include <kmessagebox.h>
#include <kstartupinfo.h>
#include <kwin.h>
#include <kwinmodule.h>
#include <kstandarddirs.h>

#include "amor.h"
#include "amor.moc"
#include "amorpm.h"
#include "amorbubble.h"
#include "amorwidget.h"
#include "amordialog.h"
#include "version.h"
#include <X11/Xlib.h>
#include <kdebug.h>

// #define DEBUG_AMOR

#define SLEEP_TIMEOUT   180     // Animation sleeps after SLEEP_TIMEOUT seconds
                                // of mouse inactivity.
#define TIPS_FILE       "tips"  // Display tips in TIP_FILE-LANG, e.g "tips-en"
#define TIP_FREQUENCY   20      // Frequency tips are displayed small == more
                                // often.

#define BUBBLE_TIME_STEP 250

// Standard animation groups
#define ANIM_BASE       "Base"
#define ANIM_NORMAL     "Sequences"
#define ANIM_FOCUS      "Focus"
#define ANIM_BLUR       "Blur"
#define ANIM_DESTROY    "Destroy"
#define ANIM_SLEEP      "Sleep"
#define ANIM_WAKE       "Wake"

//---------------------------------------------------------------------------
// QueueItem
// Constructor
//

QueueItem::QueueItem(itemType ty, QString te, int ti)
{
    // if the time field was not given, calculate one based on the type 
    // and length of the item
    int effectiveLength = 0, nesting = 0;

    // discard html code from the lenght count
    for (unsigned int i = 0; i < te.length(); i++)
    {
	if (te[i] == '<')	nesting++;
	else if (te[i] == '>')	nesting--;
	else if (!nesting)	effectiveLength++;
    }
    if (nesting) // malformed html
    {
#ifdef DEBUG_AMOR
	kdDebug(15000) << "QueueItem::QueueItem(): Malformed HTML!" << endl;
#endif
	effectiveLength = te.length();
    }

    if (ti == -1)
    {
	switch (ty)  {
	    case Talk : // shorter times
			ti = 1500 + 45 * effectiveLength;
			break;
	    case Tip  : // longer times
			ti = 4000 + 30 * effectiveLength;
			break;
	}
    }

    iType = ty;
    iText = te;
    iTime = ti;
}

//---------------------------------------------------------------------------
// AMOR
// Constructor
//
Amor::Amor() : DCOPObject( "AmorIface" ), QObject()
{
    mAmor = 0;
    mBubble = 0;
    mForceHideAmorWidget = false;
    if (readConfig())
    {
        mTargetWin   = 0;
        mNextTarget  = 0;
        mAmorDialog  = 0;
        mMenu        = 0;
        mCurrAnim    = mBaseAnim;
        mPosition    = mCurrAnim->hotspot().x();
        mState       = Normal;

        mWin = new KWinModule;
        connect(mWin, SIGNAL(activeWindowChanged(WId)),
                this, SLOT(slotWindowActivate(WId)));
        connect(mWin, SIGNAL(windowRemoved(WId)),
                this, SLOT(slotWindowRemove(WId)));
        connect(mWin, SIGNAL(stackingOrderChanged()),
                this, SLOT(slotStackingChanged()));
        connect(mWin, SIGNAL(windowChanged(WId, const unsigned long *)),
                this, SLOT(slotWindowChange(WId, const unsigned long *)));
        connect(mWin, SIGNAL(currentDesktopChanged(int)),
                this, SLOT(slotDesktopChange(int)));

        mAmor = new AmorWidget();
        connect(mAmor, SIGNAL(mouseClicked(const QPoint &)),
                        SLOT(slotMouseClicked(const QPoint &)));
        connect(mAmor, SIGNAL(dragged(const QPoint &, bool)),
                        SLOT(slotWidgetDragged(const QPoint &, bool)));
        mAmor->resize(mTheme.maximumSize());

        mTimer = new QTimer(this);
        connect(mTimer, SIGNAL(timeout()), SLOT(slotTimeout()));

        mStackTimer = new QTimer(this);
        connect(mStackTimer, SIGNAL(timeout()), SLOT(restack()));
    
	mBubbleTimer = new QTimer(this);
	connect(mBubbleTimer, SIGNAL(timeout()), SLOT(slotBubbleTimeout()));

        time(&mActiveTime);
        mCursPos = QCursor::pos();
        mCursorTimer = new QTimer(this);
        connect(mCursorTimer, SIGNAL(timeout()), SLOT(slotCursorTimeout()));
	mCursorTimer->start( 500 );

        if (mWin->activeWindow())
        {
            mNextTarget = mWin->activeWindow();
            selectAnimation(Focus);
            mTimer->start(0, true);
        }
	if (!connectDCOPSignal(0,0, "KDE_stop_screensaver()", "screenSaverStopped()",false))
		kdDebug(10000) << "Could not attach signal...KDE_stop_screensaver()" << endl;
	else
		kdDebug(10000) << "attached dcop signals..." << endl;

	if (!connectDCOPSignal(0,0, "KDE_start_screensaver()", "screenSaverStarted()",false))
		kdDebug(10000) << "Could not attach signal...KDE_start_screensaver()" << endl;
	else
		kdDebug(10000) << "attached dcop signals..." << endl;

	mTipsQueue.setAutoDelete(true);

	KStartupInfo::appStarted();
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

void Amor::screenSaverStopped()
{
#ifdef DEBUG_AMOR
    kdDebug(10000)<<"void Amor::screenSaverStopped() \n";
#endif

    mAmor->show();
    mForceHideAmorWidget = false;

    mTimer->start(0, true);
}

void Amor::screenSaverStarted()
{
#ifdef DEBUG_AMOR
    kdDebug(10000)<<"void Amor::screenSaverStarted() \n";
#endif

    mAmor->hide();
    mTimer->stop();
    mForceHideAmorWidget = true;

    // GP: hide the bubble (if there's any) leaving any current message in the queue
    hideBubble();
}

//---------------------------------------------------------------------------
//
void Amor::showTip( QString tip )
{
    if (mTipsQueue.count() < 5) // GP: start dropping tips if the queue is too long
        mTipsQueue.enqueue(new QueueItem(QueueItem::Tip, tip));

    if (mState == Sleeping)
    {
	selectAnimation(Waking);	// Set waking immediatedly
	mTimer->start(0, true);
    }
}

void Amor::showMessage( QString message )
{
    //  never drop any notice, it might be important

    mTipsQueue.enqueue(new QueueItem(QueueItem::Talk, message));

    if (mState == Sleeping)
    {
	selectAnimation(Waking);	// Set waking immediatedly
	mTimer->start(0, true);
    }
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

    mTipsQueue.clear();

    readConfig();

    mTargetWin  = 0;
    mNextTarget = 0;
    mCurrAnim   = mBaseAnim;
    mPosition   = mCurrAnim->hotspot().x();
    mState      = Normal;

    mAmor = new AmorWidget();
    mForceHideAmorWidget = false;
    connect(mAmor, SIGNAL(mouseClicked(const QPoint &)),
                    SLOT(slotMouseClicked(const QPoint &)));
    connect(mAmor, SIGNAL(dragged(const QPoint &, bool)),
		    SLOT(slotWidgetDragged(const QPoint &, bool)));
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

    if (mConfig.mTips)
    {
        mTips.setFile(TIPS_FILE);
    }

    // Select a random theme if user requested it
	if (mConfig.mRandomTheme)
	{
		QStringList files(KGlobal::dirs()->findAllResources("appdata", "*rc"));
		int randomTheme = kapp->random() % files.count();
		mConfig.mTheme = (QString)*files.at(randomTheme);
	}
	
    // read selected theme
    if (!mTheme.setTheme(mConfig.mTheme))
    {
        KMessageBox::error(0, i18n("Error reading theme: ") + mConfig.mTheme);
        return false;
    }

    if ( !mTheme.isStatic() )
    {
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
    }
    else
    {
	if ( mTheme.readGroup( ANIM_BASE ) == false )
	{
	    KMessageBox::error(0, i18n("Error reading group: ") + ANIM_BASE);
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
void Amor::showBubble()
{
    if (!mTipsQueue.isEmpty())
    {
#ifdef DEBUG_AMOR
    kdDebug(10000) << "Amor::showBubble(): Displaying tips bubble." << endl;
#endif

        if (!mBubble)
        {
            mBubble = new AmorBubble;
        }

        mBubble->setOrigin(mAmor->x()+mAmor->width()/2,
                           mAmor->y()+mAmor->height()/2);
        mBubble->setMessage(mTipsQueue.head()->text());

//	mBubbleTimer->start(mTipsQueue.head()->time(), true);
	mBubbleTimer->start(BUBBLE_TIME_STEP, true);
    }
}

//---------------------------------------------------------------------------
//
// Hide the bubble text if visible
//
void Amor::hideBubble(bool forceDequeue)
{
    if (mBubble)
    {
#ifdef DEBUG_AMOR
    kdDebug(10000) << "Amor::hideBubble(): Hiding tips bubble" << endl;
#endif

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
	if (forceDequeue || !mBubble->isVisible() || 
	    (mTipsQueue.head()->type() == QueueItem::Tip)) /* there's always an item in the queue here */
	    mTipsQueue.dequeue();

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
                mTargetRect = KWin::windowInfo(mTargetWin).frameGeometry();
		if ( mTheme.isStatic() )
		{
		    if ( mConfig.mStaticPos < 0 )
			mPosition = mTargetRect.width() + mConfig.mStaticPos;
		    else
			mPosition = mConfig.mStaticPos;
		    if ( mPosition >= mTargetRect.width() )
			mPosition = mTargetRect.width()-1;
		    else if ( mPosition < 0 )
			mPosition = 0;
		}
		else
		{
		    if (mCurrAnim->frame())
		    {
		        if (mTargetRect.width() == mCurrAnim->frame()->width())
			    mPosition = mCurrAnim->hotspot().x();
			else
			    mPosition = ( kapp->random() %
					  (mTargetRect.width() - mCurrAnim->frame()->width()) )
					 + mCurrAnim->hotspot().x();
		    }
		    else
		    {
			mPosition = mTargetRect.width()/2;
		    }
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
// GP:      if (mCurrAnim == mBaseAnim && (!mBubble || !mBubble->isVisible() ) )
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

#ifdef DEBUG_AMOR
    kdDebug(10000) << "restacking" << endl;
#endif

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
        mMenu = new KPopupMenu();
	mMenu->insertTitle("Amor"); // I really don't want this i18n'ed
        mMenu->insertItem(i18n("&Options..."), this, SLOT(slotConfigure()));
        mMenu->insertItem(i18n("&About"), this, SLOT(slotAbout()));
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
// Check cursor position
//
void Amor::slotCursorTimeout()
{
    QPoint currPos = QCursor::pos();
    QPoint diff = currPos - mCursPos;
    time_t now = time(0);

    if (mForceHideAmorWidget) return; // we're hidden, do nothing

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
	// GP: can't go to sleep if there are tips in the queue
	if (mTipsQueue.isEmpty())
	    mState = Sleeping;	// The next animation will become sleeping
    }
}

//---------------------------------------------------------------------------
//
// Display the next frame or a new animation
//
void Amor::slotTimeout()
{
    if ( mForceHideAmorWidget )
        return;
    if (!mTheme.isStatic())
	mPosition += mCurrAnim->movement();
    mAmor->setPixmap(mCurrAnim->frame());
    mAmor->move(mPosition + mTargetRect.x() - mCurrAnim->hotspot().x(),
                 mTargetRect.y() - mCurrAnim->hotspot().y() + mConfig.mOffset);
    if (!mAmor->isVisible())
    {
        mAmor->show();
        restack();
    }

    if (mCurrAnim == mBaseAnim && mCurrAnim->validFrame())
    {
	// GP: Application tips/messages can be shown in any frame number; amor tips are 
	// only displayed on the first frame of mBaseAnim (the old way of doing this).
	if ( !mTipsQueue.isEmpty() && !mBubble &&  mConfig.mAppTips)
	    showBubble();
	else if (kapp->random()%TIP_FREQUENCY == 1 && mConfig.mTips && !mBubble && !mCurrAnim->frameNum())
        {
	    mTipsQueue.enqueue(new QueueItem(QueueItem::Tip, mTips.tip())); 
	    showBubble();
        }
    }

    if (mTheme.isStatic())
	mTimer->start(mState == Normal ? 10000 : 0, true);
    else
	mTimer->start(mCurrAnim->delay(), true);

    if (!mCurrAnim->next())
    {
	if ( mBubble )
	    mCurrAnim->reset();
	else
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

//--------------------------------------------------------------------------
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
                i18n("Copyright (c) 1999 Martin R. Jones <mjones@kde.org>\n\n") +
		i18n("Original Author: Martin R. Jones <mjones@kde.org>\n") +
		i18n("Current Maintainer: Gerardo Puga <gpuga@gioia.ing.unlp.edu.ar>\n" ) +
                "\nhttp://www.powerup.com.au/~mjones/amor/";
    KMessageBox::about(0, about, i18n("About Amor"));
}

//---------------------------------------------------------------------------
//
// Widget dragged
//
void Amor::slotWidgetDragged( const QPoint &delta, bool release )
{
    if (mCurrAnim->frame())
    {
	int newPosition = mPosition + delta.x();
	if (mCurrAnim->totalMovement() + newPosition > mTargetRect.width())
	    newPosition = mTargetRect.width() - mCurrAnim->totalMovement();
	else if (mCurrAnim->totalMovement() + newPosition < 0)
	    newPosition = -mCurrAnim->totalMovement();
	mPosition = newPosition;
        mAmor->move(mPosition + mTargetRect.x() - mCurrAnim->hotspot().x(),
                 mAmor->y());

	if ( mTheme.isStatic() && release ) {
	    // static animations save the new position as preferred.
	    int savePos = mPosition;
	    if ( savePos > mTargetRect.width()/2 )
		savePos -= (mTargetRect.width()+1);

	    mConfig.mStaticPos = savePos;
	    mConfig.write();
	}
    }
}

//---------------------------------------------------------------------------
//
// Focus changed to a different window
//
void Amor::slotWindowActivate(WId win)
{
#ifdef DEBUG_AMOR
    kdDebug(10000) << "Window activated:" << win << endl;
#endif

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
        if (mState != Focus )
	    selectAnimation(Focus);
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
#ifdef DEBUG_AMOR
    kdDebug(10000) << "Window removed" << endl;
#endif

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
#ifdef DEBUG_AMOR
    kdDebug(10000) << "Stacking changed" << endl;
#endif

    // This is an active event that affects the target window
    time(&mActiveTime);

    // We seem to get this signal before the window has been restacked,
    // so we just schedule a restack.
    mStackTimer->start( 20, TRUE );
}

//---------------------------------------------------------------------------
//
// Properties of a window changed
//
void Amor::slotWindowChange(WId win, const unsigned long * properties)
{

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
#ifdef DEBUG_AMOR
        kdDebug(10000) << "Target window iconified" << endl;
#endif

        // The target window has been iconified
        selectAnimation(Destroy);
        mTargetWin = None;
        mTimer->stop();
        mTimer->start(0, true);

	return;
    }
    
    if (properties[0] & NET::WMGeometry) 
    {
#ifdef DEBUG_AMOR
        kdDebug(10000) << "Target window moved or resized" << endl;
#endif

        // The size or position of the window has changed.
        mTargetRect = info.frameGeometry;

        // make sure the animation is still on the window.
        if (mCurrAnim->frame())
        {
            hideBubble();
	    if (mTheme.isStatic())
	    {
		if ( mConfig.mStaticPos < 0 )
		    mPosition = mTargetRect.width() + mConfig.mStaticPos;
		else
		    mPosition = mConfig.mStaticPos;
		if ( mPosition >= mTargetRect.width() )
		    mPosition = mTargetRect.width()-1;
		else if ( mPosition < 0 )
		    mPosition = 0;
	    }
            else if (mPosition > mTargetRect.width() -
                    (mCurrAnim->frame()->width() - mCurrAnim->hotspot().x()))
            {
                mPosition = mTargetRect.width() -
                    (mCurrAnim->frame()->width() - mCurrAnim->hotspot().x());
            }
            mAmor->move(mPosition + mTargetRect.x() - mCurrAnim->hotspot().x(),
                     mTargetRect.y() - mCurrAnim->hotspot().y() +
                     mConfig.mOffset);
        }

	return;
    }
}

//---------------------------------------------------------------------------
//
// Changed to a different desktop
//
void Amor::slotDesktopChange(int desktop)
{
    // GP: signal currentDesktopChanged seems to be emitted even if you 
    // change to the very same desktop you are in.
    if (mWin->currentDesktop() == desktop)
	return;

#ifdef DEBUG_AMOR
    kdDebug(10000) << "Desktop change" << endl;
#endif

    mNextTarget = None;
    mTargetWin = None;
    selectAnimation( Normal );
    mTimer->stop();
    mAmor->hide();
}

// GP ===========================================================================

void Amor::slotBubbleTimeout()
{
    // has the queue item been displayed for long enough?
    QueueItem *first = mTipsQueue.head();
#ifdef DEBUG_AMOR
    if (!first)	kdDebug(15000) << "Amor::slotBubbleTimeout(): empty queue!" << endl;
#endif
    if ((first->time() > BUBBLE_TIME_STEP) && (mBubble->isVisible()))
    {
    	first->setTime(first->time() - BUBBLE_TIME_STEP);
	mBubbleTimer->start(BUBBLE_TIME_STEP, true);
	return;
    }

    // do not do anything if the mouse pointer is in the bubble
    if (mBubble->mouseWithin())
    {
	first->setTime(500);		// show this item for another 500ms
	mBubbleTimer->start(BUBBLE_TIME_STEP, true);
	return;	
    }
    
    // are there any other tips pending?
    if (mTipsQueue.count() > 1)
    {
	mTipsQueue.dequeue();
	showBubble();	// shows the next item in the queue
    } else
	hideBubble(true); // hideBubble calls dequeue() for itself.
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


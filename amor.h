/* amor.h
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
** the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
** MA 02110-1301, USA.
*/

/*
** Bug reports and questions can be sent to kde-devel@kde.org
*/
#ifndef AMOR_H
#define AMOR_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <qwidget.h>
#include <qptrqueue.h>

#include "amoranim.h"
#include "amortips.h"
#include "amorconfig.h"
#include "AmorIface.h"

class AmorDialog;
class AmorBubble;
class AmorWidget;

class QTimer;
class KWinModule;
class KPopupMenu;

class QueueItem {
public:

    enum itemType { Talk , Tip };
    
    QueueItem(itemType ty, QString te, int ti = -1);

    itemType	type() { return iType; }
    QString	text() { return iText; };
    int		time() { return iTime; };

    void	setTime(int newTime) { if (iTime > 0) iTime = newTime; };

private:
    itemType	iType;
    QString 	iText;
    int		iTime;
};

//---------------------------------------------------------------------------
//
// Amor handles window manager input and animation selection and updates.
//
class Amor : public QObject, virtual public AmorIface
{
    Q_OBJECT
public:
    Amor();
    virtual ~Amor();

    virtual void showTip(QString tip);
    virtual void showMessage(QString message);
    virtual void showMessage(QString message, int msec);
    virtual void screenSaverStopped();
    virtual void screenSaverStarted();

    void reset();

public slots:
    void slotWindowActivate(WId);
    void slotWindowRemove(WId);
    void slotStackingChanged();
    void slotWindowChange(WId, const unsigned long * properties);
    void slotDesktopChange(int);

protected slots:
    void slotMouseClicked(const QPoint &pos);
    void slotTimeout();
    void slotCursorTimeout();
    void slotConfigure();
    void slotConfigChanged();
    void slotOffsetChanged(int);
    void slotAbout();
    void slotWidgetDragged( const QPoint &delta, bool release );
    void restack();
    void hideBubble(bool forceDequeue = false);

    void slotBubbleTimeout();

protected:
    enum State { Focus, Blur, Normal, Sleeping, Waking, Destroy };

    bool readConfig();
    void readGroupConfig(KConfigBase &config, QPtrList<AmorAnim> &animList,
                            const char *seq);
    void showBubble();
    AmorAnim *randomAnimation(QPtrList<AmorAnim> &animList);
    void selectAnimation(State state=Normal);
    void active();

private:
    KWinModule       *mWin;
    WId              mTargetWin;   // The window that the animations sits on
    QRect            mTargetRect;  // The goemetry of the target window
    WId              mNextTarget;  // The window that will become the target
    AmorWidget       *mAmor;       // The widget displaying the animation
    AmorThemeManager mTheme;       // Animations used by current theme
    AmorAnim         *mBaseAnim;   // The base animation
    AmorAnim         *mCurrAnim;   // The currently running animation
    int              mPosition;    // The position of the animation
    State            mState;       // The current state of the animation
    QTimer           *mTimer;      // Frame timer
    QTimer           *mCursorTimer;// Cursor timer
    QTimer           *mStackTimer; // Restacking timer
    QTimer           *mBubbleTimer;// Bubble tip timer (GP: I didn't create this one, it had no use when I found it)
    AmorDialog       *mAmorDialog; // Setup dialog
    KPopupMenu       *mMenu;       // Our menu
    time_t           mActiveTime;  // The time an active event occurred
    QPoint           mCursPos;     // The last recorded position of the pointer
    QString          mTipText;     // Text to display in a bubble when possible
    AmorBubble       *mBubble;     // Text bubble
    AmorTips         mTips;        // Tips to display in the bubble
    bool	     mInDesktopBottom; // the animation is not on top of the 
				       // title bar, but at the bottom of the desktop

    AmorConfig       mConfig;      // Configuration parameters
    bool             mForceHideAmorWidget;

    QPtrQueue<QueueItem> mTipsQueue; // GP: tips queue
};

//---------------------------------------------------------------------------

class AmorSessionWidget : public QWidget
{
    Q_OBJECT
public:
    AmorSessionWidget();
    ~AmorSessionWidget() {};
public slots:
    void wm_saveyourself();
};

#endif // AMOR_H


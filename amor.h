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

#include <config.h>

#include <time.h>

#include <qwidget.h>
#include <qqueue.h>
//Added by qt3to4:
#include <QList>

#include "amoranim.h"
#include "amortips.h"
#include "amorconfig.h"

class AmorDialog;
class AmorBubble;
class AmorWidget;

class QTimer;
class KWinModule;
class KMenu;

class QueueItem {
public:

    enum itemType { Talk , Tip };
    
    QueueItem(itemType ty, const QString &te, int ti = -1);

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
class Amor : public QObject
{
    Q_OBJECT
public:
    Amor();
    virtual ~Amor();

    virtual void showTip(const QString &tip);
    virtual void showMessage(const QString &message);
    virtual void showMessage(const QString &message, int msec);

    void reset();

public slots:
    void screenSaverStopped();
    void screenSaverStarted();
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
    void readGroupConfig(KConfigBase &config, QList<AmorAnim> &animList,
                            const char *seq);
    void showBubble();
    AmorAnim *randomAnimation(QList<AmorAnim> &animList);
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
    KMenu       *mMenu;       // Our menu
    time_t           mActiveTime;  // The time an active event occurred
    QPoint           mCursPos;     // The last recorded position of the pointer
    QString          mTipText;     // Text to display in a bubble when possible
    AmorBubble       *mBubble;     // Text bubble
    AmorTips         mTips;        // Tips to display in the bubble
    bool	     mInDesktopBottom; // the animation is not on top of the 
				       // title bar, but at the bottom of the desktop

    AmorConfig       mConfig;      // Configuration parameters
    bool             mForceHideAmorWidget;

    QQueue<QueueItem*> mTipsQueue; // GP: tips queue
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


//---------------------------------------------------------------------------
//
// amor.h
//
// Copyright (c) 1999 Martin R. Jones <mjones@kde.org>
//

#ifndef AMOR_H
#define AMOR_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <qwidget.h>

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
    virtual void screenSaverStopped();
    virtual void screenSaverStarted();

    void reset();

public slots:
    void slotWindowActivate(WId);
    void slotWindowRemove(WId);
    void slotStackingChanged();
    void slotWindowChange(WId);
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
    void hideBubble();

protected:
    enum State { Focus, Blur, Normal, Sleeping, Waking, Destroy };

    bool readConfig();
    void readGroupConfig(KConfigBase &config, QPtrList<AmorAnim> &animList,
                            const char *seq);
    void showBubble(const QString& msg);
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
    QTimer           *mBubbleTimer;// Bubble tip timer
    AmorDialog       *mAmorDialog; // Setup dialog
    KPopupMenu       *mMenu;       // Our menu
    time_t           mActiveTime;  // The time an active event occurred
    QPoint           mCursPos;     // The last recorded position of the pointer
    QString          mTipText;     // Text to display in a bubble when possible
    AmorBubble       *mBubble;     // Text bubble
    AmorTips         mTips;        // Tips to display in the bubble

    AmorConfig       mConfig;      // Configuration parameters
    bool             mForceHideAmorWidget;
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


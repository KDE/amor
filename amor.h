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

#include <qtimer.h>
#include <qwidget.h>
#include <kwmmapp.h>

#include "amoranim.h"
#include "amorwidget.h"
#include "amorbubble.h"
#include "amortips.h"
#include "amorconfig.h"
#include "amordialog.h"

//---------------------------------------------------------------------------
//
// Amor handles window manager input and animation selection and updates.
//
class Amor : public QObject
{
    Q_OBJECT
public:
    Amor(KWMModuleApplication &app);
    virtual ~Amor();

    void reset();

protected slots:
    void slotMouseClicked(const QPoint &pos);
    void slotTimeout();
    void slotConfigure();
    void slotConfigChanged();
    void slotOffsetChanged(int);
    void slotAbout();
    void slotWindowActivate(Window);
    void slotWindowRemove(Window);
    void slotRaise(Window);
    void slotLower(Window);
    void slotWindowChange(Window);

protected:
    enum State { Focus, Blur, Normal, Sleeping, Waking, Destroy };

    bool readConfig();
    bool readThemeConfig(const char *file);
    void readGroupConfig(KConfigBase &config, QList<AmorAnim> &animList,
                            const char *seq);
    void showBubble(const char *msg);
    void hideBubble();
    AmorAnim *randomAnimation(QList<AmorAnim> &animList);
    void selectAnimation(State state=Normal);
    void restack();
    void active();

    virtual void timerEvent(QTimerEvent *);

private:
    KWMModuleApplication  &mApp;
    Window           mTargetWin;   // The window that the animations sits on
    QRect            mTargetRect;  // The goemetry of the target window
    Window           mNextTarget;  // The window that will become the target
    AmorWidget       *mAmor;       // The widget displaying the animation
    AmorThemeManager mTheme;       // Animations used by current theme
    AmorAnim         *mBaseAnim;   // The base animation
    AmorAnim         *mCurrAnim;   // The currently running animation
    int              mPosition;    // The position of the animation
    State            mState;       // The current state of the animation
    QTimer           *mTimer;      // Frame timer
    AmorDialog       *mAmorDialog; // Setup dialog
    QPopupMenu       *mMenu;       // Our menu
    int              mResizeId;    // Resize timer Id
    time_t           mActiveTime;  // The time an active event occurred
    QPoint           mCursPos;     // The last recorded position of the pointer
    int              mCursId;      // Pointer position timer id
    AmorBubble       *mBubble;     // Text bubble
    int              mBubbleId;    // Bubble text timer Id
    AmorTips         mTips;        // Tips to display in the bubble

    AmorConfig       mConfig;      // Configuration parameters
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


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

#include "amoranim.h"
#include "amorwidget.h"
#include "amorbubble.h"
#include "amortips.h"
#include "amorconfig.h"
#include "amordialog.h"

class KWinModule;

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

    void reset();

public slots:
    void slotWindowActivate(WId);
    void slotWindowRemove(WId);
    void slotStackingChanged();
    void slotWindowChange(WId);

protected slots:
    void slotMouseClicked(const QPoint &pos);
    void slotTimeout();
    void slotConfigure();
    void slotConfigChanged();
    void slotOffsetChanged(int);
    void slotAbout();

protected:
    enum State { Focus, Blur, Normal, Sleeping, Waking, Destroy };

    bool readConfig();
    void readGroupConfig(KConfigBase &config, QList<AmorAnim> &animList,
                            const char *seq);
    void showBubble(const QString& msg);
    void hideBubble();
    AmorAnim *randomAnimation(QList<AmorAnim> &animList);
    void selectAnimation(State state=Normal);
    void restack();
    void active();
    QRect windowGeometry(WId);

    virtual void timerEvent(QTimerEvent *);

private:
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


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
#ifndef AMOR_H
#define AMOR_H

#include <ctime>

#include <QQueue>

#include <KWindowSystem>
#include <KX11Extras>

#include "amoranimation.h"
#include "amortips.h"
#include "amorconfig.h"
#include "amorthememanager.h"
#include "queueitem.h"

#include <xcb/xcb.h>

class AmorDialog;
class AmorBubble;
class AmorWidget;

class QTimer;
class KWindowSystem;
class QMenu;
class KConfigBase;


class Amor : public QObject
{
    Q_OBJECT

    public:
        Amor();
        ~Amor();

        void showTip(const QString &tip);
        void showMessage(const QString &message, int msec = -1);

        void reset();

    public Q_SLOTS:
        void screenSaverStopped();
        void screenSaverStarted();
        void slotWindowActivate(WId);
        void slotWindowRemove(WId);
        void slotStackingChanged();
        void slotWindowChange(WId, NET::Properties, NET::Properties2);
        void slotDesktopChange(int desktop);

    protected Q_SLOTS:
        void slotMouseClicked(const QPoint &pos);
        void slotTimeout();
        void slotCursorTimeout();
        void slotConfigure();
        void slotConfigChanged();
        void slotOffsetChanged(int);
        void slotWidgetDragged( const QPoint &delta, bool release );
        void restack();
        void hideBubble(bool forceDequeue = false);
        void screenSaverStatusChanged(bool active);

        void slotBubbleTimeout();

    protected:
        enum State { Focus, Blur, Normal, Sleeping, Waking, Destroy };

        bool readConfig();
        void showBubble();
        void selectAnimation(State state=Normal);

    private:
        KWindowSystem *mWin;
        KX11Extras *mX11Win;
        xcb_window_t mTargetWin;                 // The window that the animations sits on
        QRect mTargetRect;              // The goemetry of the target window
        xcb_window_t mNextTarget;                // The window that will become the target
        AmorWidget *mAmor;              // The widget displaying the animation
        AmorThemeManager mTheme;        // Animations used by current theme
        AmorAnimation *mBaseAnim;       // The base animation
        AmorAnimation *mCurrAnim;       // The currently running animation
        int mPosition;                  // The position of the animation
        State mState;                   // The current state of the animation
        QTimer *mTimer;                 // Frame timer
        QTimer *mCursorTimer;           // Cursor timer
        QTimer *mStackTimer;            // Restacking timer
        QTimer *mBubbleTimer;           // Bubble tip timer (GP: I didn't create this one, it had no use when I found it)
        QMenu *mMenu;                   // Our menu
        std::time_t mActiveTime;        // The time an active event occurred
        QPoint mCursPos;                // The last recorded position of the pointer
        QString mTipText;               // Text to display in a bubble when possible
        AmorBubble *mBubble;            // Text bubble
        AmorTips mTips;                 // Tips to display in the bubble
        bool mInDesktopBottom;          // the animation is not on top of the
                                        // title bar, but at the bottom of the desktop
        AmorConfig mConfig;             // Configuration parameters
        bool mForceHideAmorWidget;
        QQueue<QueueItem> mTipsQueue;   // GP: tips queue
};


#endif

// kate: word-wrap off; encoding utf-8; indent-width 4; tab-width 4; line-numbers on; mixed-indent off; remove-trailing-space-save on; replace-tabs-save on; replace-tabs on; space-indent on;
// vim:set spell et sw=4 ts=4 nowrap cino=l1,cs,U1:

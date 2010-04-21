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

#include <QtGui/QWidget>
#include <QtCore/QQueue>
#include <QtCore/QList>

#include "amoranimation.h"
#include "amortips.h"
#include "amorconfig.h"
#include "amorthememanager.h"

class AmorDialog;
class AmorBubble;
class AmorWidget;

class QTimer;
class KWindowSystem;
class KMenu;
class KConfigBase;
class QueueItem;


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
        void slotWidgetDragged( const QPoint &delta, bool release );
        void restack();
        void hideBubble(bool forceDequeue = false);

        void slotBubbleTimeout();

    protected:
        enum State { Focus, Blur, Normal, Sleeping, Waking, Destroy };

        bool readConfig();
        void readGroupConfig(KConfigBase &config, QList<AmorAnimation> &animList, const char *seq);
        void showBubble();
        AmorAnimation *randomAnimation(QList<AmorAnimation> &animList);
        void selectAnimation(State state=Normal);
        void active();

    private:
        KWindowSystem *mWin;
        WId mTargetWin;                 // The window that the animations sits on
        QRect mTargetRect;              // The goemetry of the target window
        WId mNextTarget;                // The window that will become the target
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
        AmorDialog *mAmorDialog;        // Setup dialog
        KMenu *mMenu;                   // Our menu
        std::time_t mActiveTime;        // The time an active event occurred
        QPoint mCursPos;                // The last recorded position of the pointer
        QString mTipText;               // Text to display in a bubble when possible
        AmorBubble *mBubble;            // Text bubble
        AmorTips mTips;                 // Tips to display in the bubble
        bool mInDesktopBottom;          // the animation is not on top of the
                                        // title bar, but at the bottom of the desktop
        AmorConfig mConfig;             // Configuration parameters
        bool mForceHideAmorWidget;
        QQueue<QueueItem*> mTipsQueue;  // GP: tips queue
};


#endif

// kate: word-wrap off; encoding utf-8; indent-width 4; tab-width 4; line-numbers on; mixed-indent off; remove-trailing-space-save on; replace-tabs-save on; replace-tabs on; space-indent on;
// vim:set spell et sw=4 ts=4 nowrap cino=l1,cs,U1:

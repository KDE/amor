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
#ifndef AMORANIMATION_H
#define AMORANIMATION_H

#include <QPoint>
#include <QSize>
#include <QStringList>
#include <QVector>

class QPixmap;
class QSettings;

class AmorAnimation
{
    public:
        explicit AmorAnimation(const QSettings *config);

        void reset();
        bool next();
        int frameNum() const;
        bool validFrame() const;
        int totalMovement() const;
        QSize maximumSize() const;

        int delay() const;
        QPoint hotspot() const;
        int movement() const;

        const QPixmap *frame();

    protected:
        void readConfig(const QSettings *config);

    protected:
        int mCurrent;             // current frame in sequence
        QStringList mSequence;    // sequence of images to display
        QVector<int> mDelay;      // delay between frames
        QVector<QPoint> mHotspot; // the hotspot in a frame
        QVector<int> mMovement;   // the distance to move between frames
        int mTotalMovement;       // the total distance this animation moves
        QSize mMaximumSize;       // the maximum size of any frame
};


#endif

// kate: word-wrap off; encoding utf-8; indent-width 4; tab-width 4; line-numbers on; mixed-indent off; remove-trailing-space-save on; replace-tabs-save on; replace-tabs on; space-indent on;
// vim:set spell et sw=4 ts=4 nowrap cino=l1,cs,U1:

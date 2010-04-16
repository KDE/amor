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
#include "amoranimation.h"
#include "amorpixmapmanager.h"

#include <KConfigBase>
#include <KRandom>
#include <KConfig>
#include <KConfigGroup>
#include <KStandardDirs>
#include <QtGui/QPixmap>



AmorAnimation::AmorAnimation(KConfigGroup &config)
  : mCurrent( 0 ),
    mTotalMovement( 0 ),
    mMaximumSize(0, 0)
{
    readConfig(config);
}


void AmorAnimation::reset()
{
    mCurrent = 0;
}


bool AmorAnimation::next()
{
    return ++mCurrent < mSequence.count();
}


int AmorAnimation::frameNum() const
{
    return mCurrent;
}


bool AmorAnimation::validFrame() const
{
    return mCurrent < mSequence.count();
}


int AmorAnimation::totalMovement() const
{
    return mTotalMovement;
}


QSize AmorAnimation::maximumSize() const
{
    return mMaximumSize;
}


int AmorAnimation::delay() const
{
    return validFrame() && mCurrent < mDelay.size() ? mDelay.at( mCurrent ) : 100;
}


QPoint AmorAnimation::hotspot() const
{
    return validFrame() && mCurrent < mHotspot.size() ? mHotspot.at( mCurrent ) : QPoint( 16, 16 );
}


int AmorAnimation::movement() const
{
    return validFrame() && mCurrent < mMovement.size() ? mMovement.at( mCurrent ) : 0;
}


const QPixmap *AmorAnimation::frame()
{
    return validFrame() ? AmorPixmapManager::manager()->pixmap( mSequence.at( mCurrent ) ) : 0;
}


void AmorAnimation::readConfig(KConfigGroup &config)
{
    // Read the list of frames to display and load them into the pixmap manager.
    mSequence = config.readEntry( "Sequence", QStringList() );
    int frames = mSequence.count();
    for(QStringList::Iterator it = mSequence.begin(); it != mSequence.end(); ++it) {
        const QPixmap *pixmap = AmorPixmapManager::manager()->load( *it );
        if( pixmap ) {
            mMaximumSize = mMaximumSize.expandedTo( pixmap->size() );
        }
    }

    // Read the delays between frames.
    QStringList list;
    list = config.readEntry( "Delay", QStringList() );
    mDelay.resize( list.count() );
    for(int i = 0; i < list.count() && i < frames; ++i) {
        mDelay[i] = list.at( i ).toInt();
    }

    // Read the distance to move between frames and calculate the total
    // distance that this aniamtion moves from its starting position.
    list = config.readEntry( "Movement", QStringList() );
    mMovement.resize( frames );
    for(int i = 0; i < list.count() && i < frames; ++i) {
        mMovement[i] = list.at( i ).toInt();
        mTotalMovement += mMovement[i];
    }

    // Read the hotspot for each frame.
    QStringList entries = config.readEntry( "HotspotX", QStringList() );
    mHotspot.resize( frames );
    for(int i = 0; i < entries.count() && i < frames; ++i) {
        mHotspot[i].setX( entries.at( i ).toInt() );
    }

    entries = config.readEntry( "HotspotY", QStringList() );
    for(int i = 0; i < entries.count() && i < frames; ++i) {
        mHotspot[i].setY( entries.at( i ).toInt() );
    }

    // Add the overlap of the last frame to the total movement.
    const QPoint &lastHotspot = mHotspot[ mHotspot.size()-1 ];
    if( mTotalMovement > 0 ) {
        const QPixmap *lastFrame =  AmorPixmapManager::manager()->pixmap( mSequence.last() );
        if( lastFrame ) {
            mTotalMovement += ( lastFrame->width() - lastHotspot.x() );
        }
    }
    else if( mTotalMovement < 0 ) {
        mTotalMovement -= lastHotspot.x();
    }
}


// kate: word-wrap off; encoding utf-8; indent-width 4; tab-width 4; line-numbers on; mixed-indent off; remove-trailing-space-save on; replace-tabs-save on; replace-tabs on; space-indent on;
// vim:set spell et sw=4 ts=4 nowrap cino=l1,cs,U1:

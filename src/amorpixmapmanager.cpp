/*
 * Copyright 1999 by Martin R. Jones <mjones@kde.org>
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
#include "amorpixmapmanager.h"

#include <QPixmap>
#include <QBitmap>

AmorPixmapManager *AmorPixmapManager::mManager = 0;


AmorPixmapManager::AmorPixmapManager()
  : mPixmapDir(QLatin1String( "." ))
{
}


AmorPixmapManager::~AmorPixmapManager()
{
    qDeleteAll( mPixmaps );
}


void AmorPixmapManager::setPixmapDir(const QString &dir)
{
    mPixmapDir = dir;
}


void AmorPixmapManager::reset()
{
    mPixmapDir = QLatin1Char( '.' );
    mPixmaps.clear();
}


const QPixmap* AmorPixmapManager::load(const QString & img)
{
    QHash<QString, QPixmap*>::const_iterator it = mPixmaps.constFind( img );
    QPixmap *pixmap = it != mPixmaps.constEnd() ? *it : 0;

    if( !pixmap ) {
        // pixmap has not yet been loaded.
        QString path = mPixmapDir + QLatin1String( "/" ) + img;
        pixmap = new QPixmap( path );

        if( !pixmap->isNull() ) {
            pixmap->setMask(pixmap->createHeuristicMask(true));
            mPixmaps[img] = pixmap;
        }
        else {
            delete pixmap;
            pixmap = 0;
        }
    }

    return pixmap;
}


const QPixmap* AmorPixmapManager::pixmap(const QString & img) const
{
    return mPixmaps.contains( img ) ? mPixmaps[ img ] : 0;
}


AmorPixmapManager* AmorPixmapManager::manager()
{
    if( !mManager ) {
        mManager = new AmorPixmapManager;
    }

    return mManager;
}


// kate: word-wrap off; encoding utf-8; indent-width 4; tab-width 4; line-numbers on; mixed-indent off; remove-trailing-space-save on; replace-tabs-save on; replace-tabs on; space-indent on;
// vim:set spell et sw=4 ts=4 nowrap cino=l1,cs,U1:

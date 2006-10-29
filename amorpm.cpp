/* amorpm.cpp
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
#include "amorpm.h"
//Added by qt3to4:
#include <QPixmap>

// static
AmorPixmapManager *AmorPixmapManager::mManager = 0;

//---------------------------------------------------------------------------
//
// Constructor
//
AmorPixmapManager::AmorPixmapManager()
    : mPixmapDir(".")
{
}

//---------------------------------------------------------------------------
//
// Destructor
//
AmorPixmapManager::~AmorPixmapManager()
{
    qDeleteAll(mPixmaps);
}

//---------------------------------------------------------------------------
//
// Load an image into the image manager
//
// Returns:
//   pointer to pixmap if loaded successfully, 0 otherwise.
//
const QPixmap *AmorPixmapManager::load(const QString & img)
{
    QHash<QString, QPixmap*>::const_iterator it = mPixmaps.find(img);
    QPixmap *pixmap = it != mPixmaps.end() ? *it : 0;

    if (!pixmap)
    {
        // pixmap has not yet been loaded.
        QString path = mPixmapDir + QString("/") + img;
        pixmap = new QPixmap(path);

        if (!pixmap->isNull())
        {
            mPixmaps[img] = pixmap;
        }
        else
        {
            delete pixmap;
            pixmap = 0;
        }
    }

    return pixmap;
}

//---------------------------------------------------------------------------
//
// returns a pointer to the pixmap manager.
//
AmorPixmapManager *AmorPixmapManager::manager()
{
    if (!mManager)
    {
        mManager = new AmorPixmapManager();
    }

    return mManager;
}


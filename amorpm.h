/* amorpm.h
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
#ifndef AMORPM_H 
#define AMORPM_H 

#include <config.h>

#include <q3dict.h>
#include <qpixmap.h>

//---------------------------------------------------------------------------
//
// AmorPixmapManager stores the frames used in animations.
//
class AmorPixmapManager
{
public:
    AmorPixmapManager();
    virtual ~AmorPixmapManager();

    void setPixmapDir(const QString &dir)
        { mPixmapDir = dir; }
    void reset()
        { mPixmapDir = "."; mPixmaps.clear(); }
    const QPixmap *load(const QString & img);
    const QPixmap *pixmap(const QString & img) const
        { return mPixmaps.find(img); }

    static AmorPixmapManager *manager();

public:
    QString        mPixmapDir;           // get pixmaps from here
    Q3Dict<QPixmap> mPixmaps;             // list of pixmaps
    static AmorPixmapManager *mManager; // static pointer to instance
};

#endif // AMORPM_H 


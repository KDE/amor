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
#ifndef AMORPIXMAPMANAGER_H
#define AMORPIXMAPMANAGER_H

#include <QHash>
#include <QString>

class QPixmap;


class AmorPixmapManager
{
    public:
        AmorPixmapManager();
        virtual ~AmorPixmapManager();

        void setPixmapDir(const QString &dir);

        void reset();

        const QPixmap *load(const QString & img);
        const QPixmap *pixmap(const QString & img) const;

        static AmorPixmapManager* manager();

    public:
        QString mPixmapDir;                  // get pixmaps from here
        QHash<QString, QPixmap*> mPixmaps;   // list of pixmaps
        static AmorPixmapManager *mManager;  // static pointer to instance
};


#endif

// kate: word-wrap off; encoding utf-8; indent-width 4; tab-width 4; line-numbers on; mixed-indent off; remove-trailing-space-save on; replace-tabs-save on; replace-tabs on; space-indent on;
// vim:set spell et sw=4 ts=4 nowrap cino=l1,cs,U1:


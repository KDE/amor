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
#ifndef AMORTHEMEMANAGER_H
#define AMORTHEMEMANAGER_H

#include <QHash>
#include <QSize>
#include <QSettings>

class KConfig;
class AmorAnimation;
typedef QList<AmorAnimation*> AmorAnimationGroup;


class AmorThemeManager
{
    public:
        AmorThemeManager();
        virtual ~AmorThemeManager();

        bool setTheme(const QString &file);
        bool readGroup(const QString &seq);
        bool isStatic() const;

        AmorAnimation *random(const QString &group);

        QSize maximumSize() const;

    protected:
        QString mPath;
        QSettings *mConfig;
        QSize mMaximumSize;                              // The largest pixmap used
        QHash<QString, AmorAnimationGroup*> mAnimations; // list of animation groups
        bool mStatic;	                                 // static image
};


#endif

// kate: word-wrap off; encoding utf-8; indent-width 4; tab-width 4; line-numbers on; mixed-indent off; remove-trailing-space-save on; replace-tabs-save on; replace-tabs on; space-indent on;
// vim:set spell et sw=4 ts=4 nowrap cino=l1,cs,U1:

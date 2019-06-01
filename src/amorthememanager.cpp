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
#include "amorthememanager.h"
#include "amoranimation.h"
#include "amorpixmapmanager.h"

#include <KRandom>

#include <QFile>
#include <QSettings>
#include <QStandardPaths>

AmorThemeManager::AmorThemeManager()
  : mConfig( 0 ),
    mMaximumSize(0, 0)
{
}


AmorThemeManager::~AmorThemeManager()
{
    for( const AmorAnimationGroup &group : mAnimations ) {
        qDeleteAll( group );
    }
    delete mConfig;
}


bool AmorThemeManager::setTheme(const QString & file)
{
    if (QFile::exists(file)) {
        mPath = file;
    } else {
        mPath = QStandardPaths::locate(QStandardPaths::AppDataLocation, file);
    }

    if (mPath.isEmpty() || !QFile::exists(mPath)) {
        return false;
    }

    delete mConfig;
    mConfig = new QSettings(mPath, QSettings::IniFormat);
    mConfig->beginGroup("Config");

    // Get the directory where the pixmaps are stored and tell the pixmap manager.
    QString pixmapPath = mConfig->value( "PixmapPath" ).toString();
    if( pixmapPath.isEmpty() ) {
        return false;
    }

    if( pixmapPath[0] == QLatin1Char( '/' ) ) {
        mPath = pixmapPath; // absolute path to pixmaps
    }
    else {
        mPath.truncate( mPath.lastIndexOf( QLatin1Char( '/' ) ) + 1 ); // relative to config file
        mPath += pixmapPath;
    }

    mStatic = mConfig->value( "Static" ).toBool();

    mMaximumSize.setWidth( 0 );
    mMaximumSize.setHeight( 0 );

    for( const AmorAnimationGroup &group : mAnimations ) {
        qDeleteAll( group );
    }
    mAnimations.clear();
    mConfig->endGroup();

    return true;
}


AmorAnimation *AmorThemeManager::random(const QString & group)
{
    QString grp = mStatic ? QLatin1String( "Base" ) : group;

    const QHash<QString, AmorAnimationGroup>::const_iterator it = mAnimations.constFind( grp );

    if( it != mAnimations.constEnd() ) {
        int idx = KRandom::random() % it->count();
        return it->at( idx );
    }

    return 0;
}


bool AmorThemeManager::readGroup(const QString & seq)
{
    AmorPixmapManager::manager()->setPixmapDir( mPath );
    AmorAnimationGroup animList;

    // Read the list of available animations.
    mConfig->beginGroup("Config");
    QStringList list = mConfig->value(seq).toStringList();
    mConfig->endGroup();

    // Read each individual animation
    for(int i = 0; i < list.count(); ++i) {
        mConfig->beginGroup(list[i]);
        AmorAnimation *anim = new AmorAnimation( mConfig );
        animList.append( anim );
        mMaximumSize = mMaximumSize.expandedTo( anim->maximumSize() );
        mConfig->endGroup();
    }

    int entries = list.count();
    if ( entries == 0) {    // If no animations were available for this group, just add the base anim
        mConfig->beginGroup("Base");
        AmorAnimation *anim = new AmorAnimation( mConfig);
        if( anim ) {
            animList.append( anim );
            mMaximumSize = mMaximumSize.expandedTo( anim->maximumSize() );
            ++entries;
        }
        mConfig->endGroup();
    }

    // Couldn't read any entries at all
    if( entries == 0 ) {
        return false;
    }

    mAnimations[seq] = animList;

    return true;
}


bool AmorThemeManager::isStatic() const
{
    return mStatic;
}


QSize AmorThemeManager::maximumSize() const
{
    return mMaximumSize;
}


// kate: word-wrap off; encoding utf-8; indent-width 4; tab-width 4; line-numbers on; mixed-indent off; remove-trailing-space-save on; replace-tabs-save on; replace-tabs on; space-indent on;
// vim:set spell et sw=4 ts=4 nowrap cino=l1,cs,U1:

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

#include <KStandardDirs>
#include <KConfig>
#include <KConfigGroup>
#include <KRandom>


AmorThemeManager::AmorThemeManager()
  : mConfig( 0 ),
    mMaximumSize(0, 0)
{
}


AmorThemeManager::~AmorThemeManager()
{
    qDeleteAll( mAnimations );
    delete mConfig;
}


bool AmorThemeManager::setTheme(const QString & file)
{
    mPath = KStandardDirs::locate( "appdata", file );

    delete mConfig;
    mConfig = new KConfig( mPath, KConfig::SimpleConfig );
    KConfigGroup configGroup( mConfig, "Config" );

    // Get the directory where the pixmaps are stored and tell the pixmap manager.
    QString pixmapPath = configGroup.readPathEntry( "PixmapPath", QString() );
    if( pixmapPath.isEmpty() ) {
        return false;
    }

    if( pixmapPath[0] == '/' ) {
        mPath = pixmapPath; // absolute path to pixmaps
    }
    else {
        mPath.truncate( mPath.lastIndexOf( '/' ) + 1 ); // relative to config file
        mPath += pixmapPath;
    }

    mStatic = configGroup.readEntry( "Static", false );

    mMaximumSize.setWidth( 0 );
    mMaximumSize.setHeight( 0 );

    qDeleteAll( mAnimations );
    mAnimations.clear();

    return true;
}


AmorAnimation *AmorThemeManager::random(const QString & group)
{
    QString grp = mStatic ? QLatin1String( "Base" ) : group;

    QHash<QString, AmorAnimationGroup*>::const_iterator it = mAnimations.constFind( grp );
    AmorAnimationGroup *animGroup = it != mAnimations.constEnd() ? *it : 0;

    if( animGroup ) {
        int idx = KRandom::random() % animGroup->count();
        return animGroup->at( idx );
    }

    return 0;
}


bool AmorThemeManager::readGroup(const QString & seq)
{
    AmorPixmapManager::manager()->setPixmapDir( mPath );
    AmorAnimationGroup *animList = new AmorAnimationGroup;

#ifdef __GNUC__
#warning "kde4: fix autodelete for animList";
#endif
    //animList->setAutoDelete(true);

    // Read the list of available animations.
    KConfigGroup conf( mConfig, "Config" );
    QStringList list;
    list = conf.readEntry( seq, QStringList() );

    // Read each individual animation
    for(int i = 0; i < list.count(); ++i) {
        KConfigGroup group( mConfig, list.at( i ) );
        AmorAnimation *anim = new AmorAnimation( group );
        animList->append( anim );
        mMaximumSize = mMaximumSize.expandedTo( anim->maximumSize() );
    }

    int entries = list.count();
    if ( entries == 0) {    // If no animations were available for this group, just add the base anim
        KConfigGroup group( mConfig, "Base" );
        AmorAnimation *anim = new AmorAnimation( group );
        if( anim ) {
            animList->append( anim );
            mMaximumSize = mMaximumSize.expandedTo( anim->maximumSize() );
            ++entries;
        }
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

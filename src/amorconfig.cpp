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
#include "amorconfig.h"

#include <KConfigGroup>
#include <KSharedConfig>


AmorConfig::AmorConfig()
  : mTheme(QLatin1String( "blobrc" ) ),
    mOnTop( false ),
    mOffset( 0 ),
    mTips( false ),
    mRandomTheme( false ),
    mAppTips( true ),
    mStaticPos( 20 )
{
}


void AmorConfig::read()
{
    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup cs( config, "General" );

    mOnTop = cs.readEntry( "OnTop", false );
    mOffset = cs.readEntry( "Offset", 0 );
    mTheme = cs.readEntry( "Theme", "blobrc" );
    mTips = cs.readEntry( "Tips", false );
    mRandomTheme = cs.readEntry( "RandomTheme", false );
    mAppTips = cs.readEntry( "ApplicationTips", true );
    mStaticPos = cs.readEntry( "StaticPosition", 20 );
}


void AmorConfig::write()
{
    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup cs( config, "General" );

    cs.writeEntry( "OnTop", mOnTop );
    cs.writeEntry( "Offset", mOffset );
    cs.writeEntry( "Theme", mTheme );
    cs.writeEntry( "Tips", mTips );
    cs.writeEntry( "RandomTheme", mRandomTheme );
    cs.writeEntry( "ApplicationTips", mAppTips );
    cs.writeEntry( "StaticPosition", mStaticPos );

    config->sync();
}


// kate: word-wrap off; encoding utf-8; indent-width 4; tab-width 4; line-numbers on; mixed-indent off; remove-trailing-space-save on; replace-tabs-save on; replace-tabs on; space-indent on;
// vim:set spell et sw=4 ts=4 nowrap cino=l1,cs,U1:

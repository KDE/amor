/* amorconfig.cpp
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
#include <kapplication.h>
#include "amorconfig.h"
#include <kconfig.h>
#include <kglobal.h>

//---------------------------------------------------------------------------
//
// Constructor
//
AmorConfig::AmorConfig()
{
    mOnTop = false;
    mOffset = 0;
    mTheme = "blobrc";
    mTips = false;
    mAppTips = true;
    mStaticPos = 20;
}

//---------------------------------------------------------------------------
//
// Read the configuration
//
void AmorConfig::read()
{
    KConfig *config = KGlobal::config();
    KConfigGroup cs(config, "General");

    mOnTop = cs.readBoolEntry("OnTop", false);
    mOffset = cs.readNumEntry("Offset", 0);
    mTheme = cs.readEntry("Theme", "blobrc");
    mTips  = cs.readBoolEntry("Tips", false);
    mRandomTheme  = cs.readBoolEntry("RandomTheme", false);
    mAppTips  = cs.readBoolEntry("ApplicationTips", true);
    mStaticPos  = cs.readNumEntry("StaticPosition", 20);
}

//---------------------------------------------------------------------------
//
// Write the configuration
//
void AmorConfig::write()
{
    KConfig *config = KGlobal::config();
    KConfigGroup cs(config, "General");

    cs.writeEntry("OnTop", mOnTop);
    cs.writeEntry("Offset", mOffset);
    cs.writeEntry("Theme", mTheme);
    cs.writeEntry("Tips", mTips);
    cs.writeEntry("RandomTheme", mRandomTheme);
    cs.writeEntry("ApplicationTips", mAppTips);
    cs.writeEntry("StaticPosition", mStaticPos);

    config->sync();
}



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
** the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston,
** MA 02111-1307, USA.
*/

/*
** Bug reports and questions can be sent to kde-devel@kde.org
*/
#include <kapplication.h>
#include "amorconfig.h"
#include <kconfig.h>

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
    KConfig *config = kapp->config();
    KConfigGroupSaver cs(config, "General");

    mOnTop = config->readBoolEntry("OnTop", false);
    mOffset = config->readNumEntry("Offset", 0);
    mTheme = config->readEntry("Theme", "blobrc");
    mTips  = config->readBoolEntry("Tips", false);
    mRandomTheme  = config->readBoolEntry("RandomTheme", false);
    mAppTips  = config->readBoolEntry("ApplicationTips", true);
    mStaticPos  = config->readNumEntry("StaticPosition", 20);
}

//---------------------------------------------------------------------------
//
// Write the configuration
//
void AmorConfig::write()
{
    KConfig *config = kapp->config();
    KConfigGroupSaver cs(config, "General");

    config->writeEntry("OnTop", mOnTop);
    config->writeEntry("Offset", mOffset);
    config->writeEntry("Theme", mTheme);
    config->writeEntry("Tips", mTips);
    config->writeEntry("RandomTheme", mRandomTheme);
    config->writeEntry("ApplicationTips", mAppTips);
    config->writeEntry("StaticPosition", mStaticPos);

    config->sync();
}



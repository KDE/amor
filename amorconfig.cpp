//---------------------------------------------------------------------------
//
// amorconfig.cpp
//
// Copyright (c) 1999 Martin R. Jones <mjones@kde.org>
//

#include <kapp.h>
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
    mAppTips  = config->readBoolEntry("ApplicationTips", true);
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
    config->writeEntry("ApplicationTips", mAppTips);

    config->sync();
}



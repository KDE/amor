//---------------------------------------------------------------------------
//
// amorconfig.h
//
// Copyright (c) 1999 Martin R. Jones <mjones@kde.org>
//

#ifndef AMORCONFIG_H 
#define AMORCONFIG_H 

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif 

#include <qstring.h>

//---------------------------------------------------------------------------
//
// AmorConfig contains amor configuration parameters
//
class AmorConfig
{
public:
    AmorConfig();

    void read();
    void write();

    QString  mTheme;
    bool     mOnTop;
    int      mOffset;
    bool     mTips;
    bool     mAppTips;
};

#endif // AMORCONFIG_H 


/* amorconfig.h
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
    bool     mRandomTheme;
    bool     mAppTips;
    int      mStaticPos;
};

#endif // AMORCONFIG_H 


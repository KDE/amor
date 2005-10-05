/* AmorIface.h
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

#ifndef __AmorIface_h__
#define __AmorIface_h__

#include <dcopobject.h>

class AmorIface : virtual public DCOPObject
{
  K_DCOP
public:

k_dcop:
    virtual void showTip(QString tip) = 0;
    virtual void showMessage(QString message ) = 0;
    virtual void showMessage(QString message, int msec ) = 0;

    virtual void screenSaverStopped() = 0;
    virtual void screenSaverStarted() = 0;

};

#endif


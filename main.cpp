/* main.cpp
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


#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include <kuniqueapplication.h>
#include <klocale.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>

#include "version.h"
#include "amor.h"
#include <QtDBus>

static const char description[] = I18N_NOOP("KDE creature for your desktop");

int main(int argc, char *argv[])
{
    KAboutData aboutData( "amor", I18N_NOOP("amor"),
        AMOR_VERSION, description, KAboutData::License_GPL,
        "(c) 1999, Martin R. Jones");
    aboutData.addAuthor("Martin R. Jones",0, "mjones@kde.org");
    aboutData.addAuthor("Gerardo Puga", I18N_NOOP("Current maintainer"), "gpuga@gioia.ing.unlp.edu.ar");
    KCmdLineArgs::init( argc, argv, &aboutData );

    if (!KUniqueApplication::start()) {
	fprintf(stderr, "%s is already running!\n", aboutData.appName());
	exit(0);
    }
    KUniqueApplication app;

    // session management
    AmorSessionWidget *sessionWidget = new AmorSessionWidget;
    app.setTopWidget(sessionWidget);


    QDBusConnection::sessionBus().registerObject("/Amor",new Amor() );
    return app.exec();
}


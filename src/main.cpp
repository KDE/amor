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
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include <kuniqueapplication.h>
#include <klocale.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>

#include "version.h"
#include "amor.h"

#include <QtDBus/QtDBus>

static const char description[] = I18N_NOOP("KDE creature for your desktop");


int main(int argc, char *argv[])
{
    KAboutData aboutData( "amor", 0, ki18n("amor"),
        AMOR_VERSION, ki18n(description), KAboutData::License_GPL,
        ki18n("Copyright 1999, Martin R. Jones") );
    aboutData.addAuthor(ki18n("Martin R. Jones"),KLocalizedString(), "mjones@kde.org");
    aboutData.addAuthor(ki18n("Gerardo Puga"), ki18n("Current maintainer"), "gpuga@gioia.ing.unlp.edu.ar");
    KCmdLineArgs::init( argc, argv, &aboutData );

    if (!KUniqueApplication::start()) {
        fprintf( stderr, "%s is already running!\n", qPrintable( aboutData.appName() ) );
        exit(0);
    }
    KUniqueApplication app;

    // session management
    AmorSessionWidget *sessionWidget = new AmorSessionWidget;
    app.setTopWidget(sessionWidget);

    QDBusConnection::sessionBus().registerObject("/Amor",new Amor() );
    return app.exec();
}


// kate: word-wrap off; encoding utf-8; indent-width 4; tab-width 4; line-numbers on; mixed-indent off; remove-trailing-space-save on; replace-tabs-save on; replace-tabs on; space-indent on;
// vim:set spell et sw=4 ts=4 nowrap cino=l1,cs,U1:

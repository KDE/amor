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
#include "amor.h"
#include "amorsessionwidget.h"
#include "version.h"

#include <cstdio>

#include <QtDBus>

#include <kuniqueapplication.h>
#include <klocale.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>


static const char description[] = I18N_NOOP("KDE creature for your desktop");


int main(int argc, char *argv[])
{
    QApplication::setGraphicsSystem( QLatin1String( "native" ) );

    KAboutData about( "amor", 0, ki18n( "amor" ), AMOR_VERSION );
    about.setLicense( KAboutData::License_GPL );
    about.setShortDescription( ki18n( description ) );
    about.setCopyrightStatement( ki18n( "1999 by Martin R. Jones\n2010 by Stefan Böhmann" ) );

    about.addAuthor(
        ki18n( "Stefan Böhmann" ),
        ki18n( "Current maintainer" ),
        "kde@hilefoks.org",
        "http://www.hilefoks.org",
        "hilefoks"
    );

    about.addAuthor(
        ki18n("Martin R. Jones"),
        KLocalizedString(),
        "mjones@kde.org"
    );

    about.addAuthor(
        ki18n( "Gerardo Puga" ),
        KLocalizedString(),
        "gpuga@gioia.ing.unlp.edu.ar"
    );

    KCmdLineArgs::init( argc, argv, &about );

    if( !KUniqueApplication::start() ) {
        std::fprintf( stderr, "%s is already running!\n", qPrintable( about.appName() ) );
        exit( 0 );
    }

    KUniqueApplication app;
    AmorSessionWidget *sessionWidget = new AmorSessionWidget; // session management
    app.setTopWidget( sessionWidget );

    QDBusConnection::sessionBus().registerObject( QLatin1String( "/Amor" ),new Amor() );
    return app.exec();
}


// kate: word-wrap off; encoding utf-8; indent-width 4; tab-width 4; line-numbers on; mixed-indent off; remove-trailing-space-save on; replace-tabs-save on; replace-tabs on; space-indent on;
// vim:set spell et sw=4 ts=4 nowrap cino=l1,cs,U1:

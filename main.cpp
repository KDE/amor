//---------------------------------------------------------------------------
//
// main.cpp
//
// Copyright (c) 1999 Martin R. Jones <mjones@kde.org>
//
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include <kuniqueapplication.h>
#include <klocale.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <dcopclient.h>

#include "version.h"
#include "amor.h"


static const char description[] = I18N_NOOP("KDE creature for your desktop.");

int main(int argc, char *argv[])
{
    KAboutData aboutData( "amor", I18N_NOOP("amor"),
        AMOR_VERSION, description, KAboutData::License_GPL,
        "(c) 1999, Martin R. Jones");
    aboutData.addAuthor("Martin R. Jones",0, "mjones@kde.org");
    KCmdLineArgs::init( argc, argv, &aboutData );

    if (!KUniqueApplication::start()) {
	fprintf(stderr, "%s is already running!\n", aboutData.appName());
	exit(0);
    }
    KUniqueApplication app;

    // session management
    AmorSessionWidget *sessionWidget = new AmorSessionWidget;
    app.setTopWidget(sessionWidget);

    new Amor();

    app.dcopClient()->setDefaultObject( "AmorIface" );

    return app.exec();
}


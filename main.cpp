//---------------------------------------------------------------------------
//
// main.cpp
//
// Copyright (c) 1999 Martin R. Jones <mjones@kde.org>
//
#include <stdlib.h>
#include <time.h>

#include <kconfig.h>
#include <kapp.h>
#include <kwinmodule.h>
#include <klocale.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>

#include "version.h"
#include "amor.h"


static const char *description = I18N_NOOP("KDE creature for your desktop.");

int main(int argc, char *argv[])
{
    KAboutData aboutData( "amor", I18N_NOOP("amor"),
        AMOR_VERSION, description, KAboutData::License_GPL,
        "(c) 1999, Martin R. Jones");
    aboutData.addAuthor("Martin R. Jones",0, "mjones@kde.org");
    KCmdLineArgs::init( argc, argv, &aboutData );
    
    KApplication app;

    // session management
    AmorSessionWidget *sessionWidget = new AmorSessionWidget;
    app.setTopWidget(sessionWidget);

#warning fix session management    
#if 0    
    app.enableSessionManagement(true);
    app.setWmCommand(argv[0]);
#endif    

    KWinModule *winModule = new KWinModule;

    Amor *amor = new Amor();
    QObject::connect(winModule, SIGNAL(windowActivate(WId)),
                    amor,     SLOT(slotWindowActivate(WId)));
    QObject::connect(winModule, SIGNAL(windowRemove(WId)),
                    amor,     SLOT(slotWindowRemove(WId)));
    QObject::connect(winModule, SIGNAL(stackingOrderChanged()),
                    amor,     SLOT(slotStackingChanged()));
    QObject::connect(winModule, SIGNAL(windowChange(WId)),
                    amor,     SLOT(slotWindowChange(WId)));

	  return app.exec();
}


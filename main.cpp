//---------------------------------------------------------------------------
//
// main.cpp
//
// Copyright (c) 1999 Martin R. Jones <mjones@kde.org>
//
#include <stdlib.h>
#include <time.h>
#include <kconfig.h>
#include "amor.h"
#include <kwmmapp.h>

int main(int argc, char *argv[])
{
	KWMModuleApplication app(argc, argv, "amor");

    srandom(time(0));

    // session management
    app.setTopWidget(new AmorSessionWidget);
    app.enableSessionManagement(true);
    app.setWmCommand(argv[0]);

    Amor amor(app);

    app.connectToKWM();
	return app.exec();
}


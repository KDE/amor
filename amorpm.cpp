//---------------------------------------------------------------------------
//
// amorpm.cpp
//
// Copyright (c) 1999 Martin R. Jones <mjones@kde.org>
//

#include "amorpm.h"

// static
AmorPixmapManager *AmorPixmapManager::mManager = 0;

//---------------------------------------------------------------------------
//
// Constructor
//
AmorPixmapManager::AmorPixmapManager()
    : mPixmapDir(".")
{
    mPixmaps.setAutoDelete(true);
}

//---------------------------------------------------------------------------
//
// Destructor
//
AmorPixmapManager::~AmorPixmapManager()
{
}

//---------------------------------------------------------------------------
//
// Load an image into the image manager
//
// Returns:
//   pointer to pixmap if loaded successfully, 0 otherwise.
//
const QPixmap *AmorPixmapManager::load(const QString & img)
{
    QPixmap *pixmap = mPixmaps.find(img);

    if (!pixmap)
    {
        // pixmap has not yet been loaded.
        QString path = mPixmapDir + QString("/") + img;
        pixmap = new QPixmap(path);

        if (!pixmap->isNull())
        {
            mPixmaps.insert(img,pixmap);
        }
        else
        {
            delete pixmap;
            pixmap = 0;
        }
    }

    return pixmap;
}

//---------------------------------------------------------------------------
//
// returns a pointer to the pixmap manager.
//
AmorPixmapManager *AmorPixmapManager::manager()
{
    if (!mManager)
    {
        mManager = new AmorPixmapManager();
    }

    return mManager;
}


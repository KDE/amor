//---------------------------------------------------------------------------
//
// amorpm.h
//
// Copyright (c) 1999 Martin R. Jones <mjones@kde.org>
//

#ifndef AMORPM_H 
#define AMORPM_H 

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif 

#include <qdict.h>
#include <qpixmap.h>

//---------------------------------------------------------------------------
//
// AmorPixmapManager stores the frames used in animations.
//
class AmorPixmapManager
{
public:
    AmorPixmapManager();
    virtual ~AmorPixmapManager();

    void setPixmapDir(const QString &dir)
        { mPixmapDir = dir; }
    void reset()
        { mPixmapDir = "."; mPixmaps.clear(); }
    const QPixmap *load(const char *img);
    const QPixmap *pixmap(const char *img) const
        { return mPixmaps.find(img); }

    static AmorPixmapManager *manager();

public:
    QString        mPixmapDir;           // get pixmaps from here
    QDict<QPixmap> mPixmaps;             // list of pixmaps
    static AmorPixmapManager *mManager; // static pointer to instance
};

#endif // AMORPM_H 


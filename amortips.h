//---------------------------------------------------------------------------
//
// amortips.h
//
// Copyright (c) 1999 Martin R. Jones <mjones@kde.org>
//

#ifndef AMORTIPS_H 
#define AMORTIPS_H 

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif 

#include <qstrlist.h>
class QFile;

//---------------------------------------------------------------------------
//
// AmorTips selects random tips from a data file
//
class AmorTips
{
public:
    AmorTips();

    bool setFile(const char *file);
    void reset();
    const char *tip();

protected:
    bool read(const char *file);
    bool readTip(QFile &file);

protected:
    QStrList mTips;
};

#endif // AMORTIPS_H 


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

    bool setFile(const QString& file);
    void reset();
    QString tip();

protected:
    bool read(const QString& file);
    bool readTip(QFile &file);

protected:
    QStringList mTips;
};

#endif // AMORTIPS_H


//---------------------------------------------------------------------------
//
// amortips.cpp
//
// Copyright (c) 1999 Martin R. Jones <mjones@kde.org>
//

#include <stdlib.h>
#include <kapp.h>
#include "amortips.h"
#if QT_VERSION >= 199
#include <kglobal.h>
#include <kstddirs.h>
#endif
#include <klocale.h>
#include <qfile.h>

//---------------------------------------------------------------------------
//
AmorTips::AmorTips()
{
}

//---------------------------------------------------------------------------
//
// Set the file containing tips.  This reads all tips into memory at the
// moment - need to make more efficient.
//
bool AmorTips::setFile(const QString& file)
{
    QStringList list = KGlobal::locale()->languageList();
    list.append("default");
    QStringList::ConstIterator it = list.begin();

    for (it = list.begin(); it != list.end(); it++)
    {
        QString path( locate("appdata", file + "-" + *it) );
        if (path.length() && read(path))
        {
            return true;
        }
    }

    return false;
}

//---------------------------------------------------------------------------
//
// Clear all tips from memory
//
void AmorTips::reset()
{
    mTips.clear();
}

//---------------------------------------------------------------------------
//
// Get a tip randomly from the list
//
QString AmorTips::tip()
{
    if (mTips.count())
    {
        return *mTips.at(kapp->random() % mTips.count());
    }

    return QString::fromLatin1("No tip");
}

//---------------------------------------------------------------------------
//
// Read all tips from the specified file.
//
bool AmorTips::read(const QString& path)
{
    QFile file(path);

    if (file.open(IO_ReadOnly))
    {
        while (!file.atEnd())
        {
            readTip(file);
        }
    }

    return false;
}

//---------------------------------------------------------------------------
//
// Read a single tip.
//
bool AmorTips::readTip(QFile &file)
{
    char buffer[1024] = "";
    QString tip;

    while (!file.atEnd() && buffer[0] != '%')
    {
        file.readLine(buffer, 1024);
        if (buffer[0] != '%')
        {
            tip += QString::fromLocal8Bit(buffer);
        }
    }

    if (!tip.isEmpty())
    {
        if (tip[tip.length()-1] == '\n')
        {
            tip.truncate(tip.length()-1);
        }
        mTips.append(tip);
        return true;
    }

    return false;
}



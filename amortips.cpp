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
#include <klocale.h>
#endif
#include <kglobal.h>
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
bool AmorTips::setFile(const char *file)
{
#if QT_VERSION >= 199
    QStringList list = KGlobal::locale()->languageList();
    list.append("default");
    QStringList::ConstIterator it = list.begin();

    for (it = list.begin(); it != list.end(); it++)
    {
        QString path =  KApplication::localkdedir();
        path += "/share/apps/amor/";
        path += file + QString("-") + *it;
        if (read(path))
        {
            return true;
        }
        path =  KApplication::kde_datadir().copy();
        path += "/amor/";
        path += file + QString("-") + *it;
        if (read(path))
        {
            return true;
        }
    }

#else
    QStrList list = klocale->languageList();
    list.append("default");
    QStrListIterator it(list);

    for (; it.current(); ++it)
    {
        QString path =  KApplication::localkdedir().copy();
        path += "/share/apps/amor/";
        path += file + QString("-") + it.current();
        if (read(path))
        {
            return true;
        }
        path =  KApplication::kde_datadir().copy();
        path += "/amor/";
        path += file + QString("-") + it.current();
        if (read(path))
        {
            return true;
        }
    }
#endif

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
const char *AmorTips::tip()
{
    if (mTips.count())
    {
        return mTips.at(random()%mTips.count());
    }

    return "No tip";
}

//---------------------------------------------------------------------------
//
// Read all tips from the specified file.
//
bool AmorTips::read(const char *path)
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
            tip += buffer;
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



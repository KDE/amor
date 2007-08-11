/* amortips.cpp
**
** Copyright (c) 1999 Martin R. Jones <mjones@kde.org>
**
*/

/*
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program in a file called COPYING; if not, write to
** the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
** MA 02110-1301, USA.
*/

/*
** Bug reports and questions can be sent to kde-devel@kde.org
*/
#include "amortips.h"

#include <stdlib.h>
#include <krandom.h>
#include <kstandarddirs.h>
#include <klocale.h>
#include <qfile.h>
#include <qregexp.h>
#include <kdebug.h>

//---------------------------------------------------------------------------
//
AmorTips::AmorTips()
{
    KGlobal::locale()->insertCatalog("ktip"); // For ktip tip translations
}

//---------------------------------------------------------------------------
//
// Set the file containing tips.  This reads all tips into memory at the
// moment - need to make more efficient.
//
bool AmorTips::setFile(const QString& file)
{
    bool rv = false;

    QString path( KStandardDirs::locate("appdata", file) );
    if(path.length() && read(path))
        rv = true;

    rv |= readKTips();

    return rv;
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
        QString tip = mTips.at(KRandom::random() % mTips.count());
		return i18n(tip.toUtf8());
    }

    return i18n("No tip");
}

//---------------------------------------------------------------------------
//
// Read the tips from ktip's file
//
bool AmorTips::readKTips()
{
    QString fname;

    fname = KStandardDirs::locate("data", QString("kdewizard/tips"));

    if (fname.isEmpty())
	return false;

    QFile f(fname);
    if (f.open(QIODevice::ReadOnly))
    {
	// Reading of tips must be exactly as in KTipDatabase::loadTips for translation
	QString content = f.readAll();
	const QRegExp rx("\\n+");

	int pos = -1;
	while ((pos = content.indexOf("<html>", pos + 1, Qt::CaseInsensitive)) != -1)
	{
	    QString tip = content
		.mid(pos + 6, content.indexOf("</html>", pos, Qt::CaseInsensitive) - pos - 6)
		.replace(rx, "\n");
	    if (!tip.endsWith('\n'))
		tip += '\n';
	    if (tip.startsWith('\n'))
		tip = tip.mid(1);
	    if (tip.isEmpty())
	    {
		kDebug() << "Empty tip found! Skipping! " << pos;
		continue;
	    }
	    mTips.append(tip);
	}

	f.close();
    }

    return true;
}

//---------------------------------------------------------------------------
//
// Read all tips from the specified file.
//
bool AmorTips::read(const QString& path)
{
    QFile file(path);

    if (file.open(QIODevice::ReadOnly))
    {
        while (!file.atEnd())
        {
            readTip(file);
        }

	return true;
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
            tip += QString::fromUtf8(buffer);
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



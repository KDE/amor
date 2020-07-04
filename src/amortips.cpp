/*
 * Copyright 1999 by Martin R. Jones <mjones@kde.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#include "amortips.h"
#include "amor_debug.h"

#include <QFile>
#include <QRegExp>
#include <QStandardPaths>
#include <QRandomGenerator>

#include <stdlib.h>

#include <KLocalizedString>



AmorTips::AmorTips()
{
}

bool AmorTips::setFile(const QString& file)
{
    QString path(QStandardPaths::locate(QStandardPaths::AppDataLocation, file));
    if (path.isEmpty()) {
        qDebug() << "File not found in share/amor:" << file;
        return false;
    }
    return read(path);
}

void AmorTips::reset()
{
    mTips.clear();
}

QString AmorTips::tip()
{
    if (mTips.count()) {
        QString tip = mTips.at( QRandomGenerator::global()->bounded( mTips.count() ) );
        return i18n( tip.toUtf8() );
    }
    return QString();
}

bool AmorTips::read(const QString& path)
{
    QFile file( path );

    if( file.open( QIODevice::ReadOnly ) ) {
        while( !file.atEnd() ) {
            readTip( file );
        }

        qDebug() << "read" << mTips.count() << "tips";
        return true;
    }

    return false;
}


bool AmorTips::readTip(QFile &file)
{
    char buffer[1024] = "";
    QString tip;

    while( !file.atEnd() && buffer[0] != '%' ) {
        file.readLine( buffer, 1024 );
        if( buffer[0] != '%' ) {
            tip += QString::fromUtf8( buffer );
        }
    }

    if (!tip.isEmpty()) {
        if (tip.endsWith(QLatin1Char('\n'))) {
            tip.chop(1);
        }
        mTips.append(tip);
        return true;
    }

    return false;
}


// kate: word-wrap off; encoding utf-8; indent-width 4; tab-width 4; line-numbers on; mixed-indent off; remove-trailing-space-save on; replace-tabs-save on; replace-tabs on; space-indent on;
// vim:set spell et sw=4 ts=4 nowrap cino=l1,cs,U1:

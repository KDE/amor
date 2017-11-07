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

#include <QtCore/QFile>
#include <QtCore/QRegExp>
#include <QStandardPaths>

#include <stdlib.h>

#define TRANSLATION_DOMAIN "ktip"
#include <KLocalizedString>

#include <krandom.h>
#include <KLocalizedString>



AmorTips::AmorTips()
{
}

bool AmorTips::setFile(const QString& file)
{
    QString path(QStandardPaths::locate(QStandardPaths::AppDataLocation, file));

    bool rv = path.length() && read( path );

    rv |= readKTips();

    return rv;
}


void AmorTips::reset()
{
    mTips.clear();
}


QString AmorTips::tip()
{
    if( mTips.count() ) {
        QString tip = mTips.at( KRandom::random() % mTips.count() );
        return i18n( tip.toUtf8() );
    }

    return i18n( "No tip" );
}


bool AmorTips::readKTips()
{
    QString fname;

    fname = QStandardPaths::locate(QStandardPaths::GenericDataLocation, QStringLiteral("kdewizard/tips"));

    if( fname.isEmpty() ) {
        return false;
    }

    QFile f( fname );
    if( f.open( QIODevice::ReadOnly ) ) {
        // Reading of tips must be exactly as in KTipDatabase::loadTips for translation
        QString content =QLatin1String( f.readAll() );
        const QRegExp rx( QLatin1String( "\\n+" ) );

        int pos = -1;
        while( ( pos = content.indexOf( QLatin1String( "<html>" ), pos + 1, Qt::CaseInsensitive ) ) != -1 ) {
            QString tip = content
                            .mid( pos + 6, content.indexOf(QLatin1String( "</html>" ), pos, Qt::CaseInsensitive) - pos - 6 )
                            .replace( rx, QLatin1String( "\n" ) );

            if( !tip.endsWith(QLatin1Char( '\n' )) ) {
                tip += QLatin1Char( '\n' );
            }

            if( tip.startsWith( QLatin1Char( '\n' ) ) ) {
                tip = tip.mid( 1 );
            }

            if( tip.isEmpty() ) {
                qCDebug(AMOR_LOG) << "Empty tip found! Skipping! " << pos;
                continue;
            }

            mTips.append( tip );
        }

        f.close();
    }

    return true;
}


bool AmorTips::read(const QString& path)
{
    QFile file( path );

    if( file.open( QIODevice::ReadOnly ) ) {
        while( !file.atEnd() ) {
            readTip( file );
        }

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

    if( !tip.isEmpty() ) {
        if( tip[ tip.length()-1 ] == QLatin1Char( '\n' ) ) {
            tip.truncate( tip.length()-1 );
        }
        mTips.append( tip );

        return true;
    }

    return false;
}


// kate: word-wrap off; encoding utf-8; indent-width 4; tab-width 4; line-numbers on; mixed-indent off; remove-trailing-space-save on; replace-tabs-save on; replace-tabs on; space-indent on;
// vim:set spell et sw=4 ts=4 nowrap cino=l1,cs,U1:

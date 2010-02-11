/*
 * Copyright 1999 by Martin R. Jones <mjones@kde.org>
 * Copyright 2010 by Stefan Böhmann <kde@hilefoks.org>
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
#include "queueitem.h"


QueueItem::QueueItem(ItemType type, const QString &text, int time)
  : m_type( type ),
    m_text( text ),
    m_time( time )
{
    // if the time field was not given, calculate one based on the type
    // and length of the item
    int effectiveLength = 0;
    int nesting = 0;

    // discard html code from the length count
    for(int i = 0; i < m_text.length(); ++i) {
        if( m_text[i] == '<' ) {
            nesting++;
        }
        else if( m_text[i] == '>' ) {
            nesting--;
        }
        else if( !nesting ) {
            effectiveLength++;
        }
    }

    if( nesting ) { // malformed html
        effectiveLength = m_text.length();
    }

    if( m_time == -1 ) {
        switch( m_type ) {
        case Talk: // shorter times
            m_time = 1500 + 45 * effectiveLength;
            break;
        case Tip: // longer times
            m_time = 4000 + 30 * effectiveLength;
            break;
        }
    }
}


QueueItem::ItemType QueueItem::type()
{
    return m_type;
}


QString QueueItem::text()
{
    return m_text;
}


int QueueItem::time()
{
    return m_time;
}


void QueueItem::setTime(int newTime)
{
    if( m_time > 0 ) {
        m_time = newTime;
    }
}


// kate: word-wrap off; encoding utf-8; indent-width 4; tab-width 4; line-numbers on; mixed-indent off; remove-trailing-space-save on; replace-tabs-save on; replace-tabs on; space-indent on;
// vim:set spell et sw=4 ts=4 nowrap cino=l1,cs,U1:

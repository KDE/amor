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
#ifndef AMORBUBBLE_H
#define AMORBUBBLE_H

#include <QLabel>
#include <QPoint>

class QLabel;


class AmorBubble : public QLabel
{
    Q_OBJECT

    public:
        AmorBubble();

        void setOrigin(int x, int y) { move( x + 10, y - 10 ); }
        void setMessage(const QString &message) { setText( message ); show(); }

        bool mouseWithin() { return m_mouseWithin; }

    protected:
        void enterEvent(QEvent *event) override { Q_UNUSED( event ); m_mouseWithin = true; }
        void leaveEvent(QEvent *event) override { Q_UNUSED( event ); m_mouseWithin = false; }
        void mouseReleaseEvent(QMouseEvent *event) override { Q_UNUSED( event ); hide(); }

    protected:        
        QLabel *m_label;        // displays the message
        //QPoint m_position;
        bool m_mouseWithin;      // the mouse pointer is inside the bubble
        
        //QRect mBound;           // bounds of the text
        //QBitmap mMask;          // shape mask
        //VertPos mArrowVert;     // vertical position of the arrow
        //HorzPos mArrowHorz;     // horizontal position of the arrow
};


#endif

// kate: word-wrap off; encoding utf-8; indent-width 4; tab-width 4; line-numbers on; mixed-indent off; remove-trailing-space-save on; replace-tabs-save on; replace-tabs on; space-indent on;
// vim:set spell et sw=4 ts=4 nowrap cino=l1,cs,U1:

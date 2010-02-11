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

#include <QWidget>
#include <QBitmap>

class KTextBrowser;


class AmorBubble : public QWidget
{
    Q_OBJECT

    public:
        AmorBubble();

        void setOrigin(int x, int y);
        void setMessage(const QString &message);

        bool mouseWithin();

    protected:
        enum VertPos { Top, Bottom };
        enum HorzPos { Left, Right };

        void calcGeometry();
        void drawBubble(QPainter &p);
        virtual void paintEvent(QPaintEvent *e);
        virtual void mouseReleaseEvent(QMouseEvent *e);
        virtual bool eventFilter(QObject *obj, QEvent *e);

    protected:
        QString mMessage;       // message to display
        int mOriginX;           // X origin of bubble arrow
        int mOriginY;           // Y origin of bubble arrow
        QRect mBound;           // bounds of the text
        QBitmap mMask;          // shape mask
        VertPos mArrowVert;     // vertical position of the arrow
        HorzPos mArrowHorz;     // horizontal position of the arrow
        KTextBrowser *mBrowser; // displays the message
        bool mMouseWithin;      // the mouse pointer is inside the bubble
};


#endif

// kate: word-wrap off; encoding utf-8; indent-width 4; tab-width 4; line-numbers on; mixed-indent off; remove-trailing-space-save on; replace-tabs-save on; replace-tabs on; space-indent on;
// vim:set spell et sw=4 ts=4 nowrap cino=l1,cs,U1:

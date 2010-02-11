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
#include "amorwidget.h"

#include <QBitmap>
#include <QPainter>
#include <QMouseEvent>
#include <QX11Info>

#include <X11/Xlib.h>
#include <X11/extensions/shape.h>


AmorWidget::AmorWidget()
  : QWidget( 0, Qt::WindowTitleHint | Qt::X11BypassWindowManagerHint ),
    m_pixmap( 0 ),
    m_dragging( false )
{
    setAttribute( Qt::WA_NoSystemBackground, true );
}


void AmorWidget::setPixmap(const QPixmap *pixmap)
{
    m_pixmap = pixmap;

    if( m_pixmap ) {
        if( !m_pixmap->mask().isNull() ) {
            XShapeCombineMask( QX11Info::display(), winId(), ShapeBounding, 0, 0, m_pixmap->mask().handle(), ShapeSet );
            repaint();
        }
        update();
    }
}


void AmorWidget::paintEvent(QPaintEvent *)
{
    if( m_pixmap ) {
        QPainter p( this );
        p.drawPixmap( 0, 0, *m_pixmap );
    }
}


void AmorWidget::mousePressEvent(QMouseEvent *me)
{
    m_clickPos = me->globalPos();
}


void AmorWidget::mouseMoveEvent(QMouseEvent *me)
{
    if( me->buttons() & Qt::LeftButton ) {
        if( !m_dragging && ( m_clickPos-me->globalPos() ).manhattanLength() > 3 ) {
            m_dragging = true;
        }
        if( m_dragging ) {
            emit dragged( me->globalPos() - m_clickPos, false );
            m_clickPos = me->globalPos();
        }
    }
}


void AmorWidget::mouseReleaseEvent(QMouseEvent *me)
{
    if( m_dragging ) {
        emit dragged( me->globalPos() - m_clickPos, true );
    }
    else if( me->button() == Qt::RightButton ) {
        emit mouseClicked(me->globalPos());
    }

    m_clickPos = QPoint();
    m_dragging = false;
}


// kate: word-wrap off; encoding utf-8; indent-width 4; tab-width 4; line-numbers on; mixed-indent off; remove-trailing-space-save on; replace-tabs-save on; replace-tabs on; space-indent on;
// vim:set spell et sw=4 ts=4 nowrap cino=l1,cs,U1:

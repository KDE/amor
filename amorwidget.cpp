/* amorwidget.cpp
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
** the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston,
** MA 02111-1307, USA.
*/

/*
** Bug reports and questions can be sent to kde-devel@kde.org
*/
#include "amorwidget.h"
#include "amorwidget.moc"
#include <qbitmap.h>
#include <X11/Xlib.h>
#include <X11/extensions/shape.h>

//---------------------------------------------------------------------------
//
// Constructor
//
AmorWidget::AmorWidget()
	: QWidget(0, 0, WStyle_Customize | WStyle_NoBorder | WX11BypassWM ),
      mPixmap(0)
{
    setBackgroundMode( NoBackground );
    dragging = false;
}

//---------------------------------------------------------------------------
//
// Destructor
//
AmorWidget::~AmorWidget()
{
}

//---------------------------------------------------------------------------
//
// Set the pixmap to display
//
void AmorWidget::setPixmap(const QPixmap *pixmap)
{
    mPixmap = pixmap;

    if (mPixmap)
    {
        if (mPixmap->mask())
        {
            XShapeCombineMask( x11Display(), winId(), ShapeBounding, 0, 0,
                                mPixmap->mask()->handle(), ShapeSet );
            repaint(false);
        }
    
	update();
    }
}

//---------------------------------------------------------------------------
//
// Draw the pixmap
//
void AmorWidget::paintEvent(QPaintEvent *)
{
    if (mPixmap)
        bitBlt( this, 0, 0, mPixmap );
    else
	erase();
}

//---------------------------------------------------------------------------
//
// The user clicked on the widget
//
void AmorWidget::mousePressEvent(QMouseEvent *me)
{
    clickPos = me->globalPos();
}

//---------------------------------------------------------------------------
//
// The user moved the mouse
//
void AmorWidget::mouseMoveEvent(QMouseEvent *me)
{
    if ( me->state() == LeftButton ) {
	if ( !dragging && (clickPos-me->globalPos()).manhattanLength() > 3 )
	    dragging = true;
	if ( dragging ) {
	    emit dragged( me->globalPos() - clickPos, false );
	    clickPos = me->globalPos();
	}
    }
}

//---------------------------------------------------------------------------
//
// The user clicked on the widget
//
void AmorWidget::mouseReleaseEvent(QMouseEvent *me)
{
    if ( dragging )
	emit dragged( me->globalPos() - clickPos, true );
    else if ( me->state() == RightButton )
	emit mouseClicked(clickPos);

    clickPos = QPoint();
    dragging = false;
}


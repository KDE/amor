//---------------------------------------------------------------------------
//
// amorwidget.cpp
//
// Copyright (c) 1999 Martin R. Jones <mjones@kde.org>
//

#include <amorwidget.h>
#include <amorwidget.moc>
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
    if (pixmap)
    {
        mPixmap = pixmap;
        if (mPixmap->mask())
        {
            XShapeCombineMask( x11Display(), winId(), ShapeBounding, 0, 0,
                                mPixmap->mask()->handle(), ShapeSet );
            repaint(false);
        }
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


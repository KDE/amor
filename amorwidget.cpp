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
	: QWidget(0, 0, WStyle_Customize | WStyle_NoBorder ),
      mPixmap(0)
{
    setBackgroundMode( NoBackground );
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
void AmorWidget::mouseReleaseEvent(QMouseEvent *me)
{
    emit mouseClicked(me->globalPos());
}


//---------------------------------------------------------------------------
//
// amorbubble.cpp
//
// Copyright (c) 1999 Martin R. Jones <mjones@kde.org>
//

#include <amorbubble.h>
#include <amorbubble.moc>
#include <qbitmap.h>
#include <qpainter.h>
#include <kapp.h>
#include <X11/Xlib.h>
#include <X11/extensions/shape.h>

#define ARROW_WIDTH     8
#define ARROW_HEIGHT    10
#define BORDER_SIZE     4
#define BUBBLE_OFFSET   16

//---------------------------------------------------------------------------
//
// Constructor
//
AmorBubble::AmorBubble()
	: QWidget(0, 0, WStyle_Customize | WStyle_NoBorder )
{
    mOriginX = 0;
    mOriginY = 0;
}

//---------------------------------------------------------------------------
//
// Destructor
//
AmorBubble::~AmorBubble()
{
}

//---------------------------------------------------------------------------
//
// Set the message to display in the bubble.  Causes the geometry of the
// widget to be recalculated.
//
void AmorBubble::setMessage(QString message)
{
    mMessage = message;
    calcGeometry();
}

//---------------------------------------------------------------------------
//
// Calculates the size, position and mask of the bubble
//
void AmorBubble::calcGeometry()
{
    QPainter painter(this);
    mBound = painter.boundingRect(0, 0, 200, 400, WordBreak, mMessage);

    // enforce minimum size
    mBound = mBound.unite(QRect(0, 0, 80, 20));
    mBound.moveBy(ARROW_WIDTH+BORDER_SIZE, BORDER_SIZE);

    // initialise the default geometry of the bubble
    int w = mBound.width() + BORDER_SIZE * 2 + ARROW_WIDTH * 2;
    int h = mBound.height() + BORDER_SIZE * 2;
    int xpos = mOriginX + BUBBLE_OFFSET;
    int ypos = mOriginY - BORDER_SIZE - ARROW_HEIGHT / 2;

    mArrowVert = Top;
    mArrowHorz = Left;

    // The actual position of the bubble may change if it is too close to
    // the desktop boundary.
    if (mOriginX + w > kapp->desktop()->width())
    {
        // source on right
        xpos = mOriginX - w - BUBBLE_OFFSET;
        mArrowHorz = Right;
    }

    if (mOriginY + h > kapp->desktop()->height())
    {
        // source at bottom
        ypos = mOriginY - h + BORDER_SIZE + ARROW_HEIGHT / 2;
        mArrowVert = Bottom;
    }

    setGeometry(xpos, ypos, w, h);

    // create and apply the shape mask
    mMask.resize(w, h);
    mMask.fill(color0);
    QPainter maskPainter(&mMask);
    maskPainter.setPen(color1);
    maskPainter.setBrush(color1);
    drawBubble(maskPainter);
    XShapeCombineMask( x11Display(), winId(), ShapeBounding, 0, 0,
                       mMask.handle(), ShapeSet );
}

//---------------------------------------------------------------------------
//
// Draw the bubble that text will be draw into using the current pen
// as the outline and the current brush as the fill.
//
void AmorBubble::drawBubble(QPainter &p)
{
    QPointArray pointArray(3);

    if (mArrowHorz == Left)
    {
        pointArray.setPoint(0, ARROW_WIDTH, 0);
        pointArray.setPoint(1, 0, -2);
        pointArray.setPoint(2, ARROW_WIDTH, ARROW_HEIGHT);
    }
    else
    {
        pointArray.setPoint(0, 0, 0);
        pointArray.setPoint(1, ARROW_WIDTH, -2);
        pointArray.setPoint(2, 0, ARROW_HEIGHT);
        pointArray.translate(width() - ARROW_WIDTH - 1, 0);
    }

    if (mArrowVert == Top)
    {
        pointArray.translate(0, BORDER_SIZE + ARROW_HEIGHT / 2);
    }
    else
    {
        pointArray.translate(0, height() - BORDER_SIZE - ARROW_HEIGHT / 2);
    }

    p.drawRoundRect(ARROW_WIDTH, 0, width() - ARROW_WIDTH*2, height(), 20, 20);

    QPen pen(p.pen());
    p.setPen(NoPen);
    p.drawPolygon(pointArray);

    p.setPen(pen);
    p.drawPolyline(pointArray);
}

//---------------------------------------------------------------------------
//
// Draw the message in a bubble
//
void AmorBubble::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(black);
    painter.setBrush(yellow);
    drawBubble(painter);
    painter.drawText(mBound, AlignVCenter | WordBreak, mMessage);
}

//---------------------------------------------------------------------------
//
// The user clicked on the widget
//
void AmorBubble::mouseReleaseEvent(QMouseEvent *)
{
    hide();
}


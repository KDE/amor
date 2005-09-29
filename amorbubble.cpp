/* amorbubble.cpp
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
** the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
** MA 02110-1301, USA.
*/

/*
** Bug reports and questions can be sent to kde-devel@kde.org
*/
#include "amorbubble.h"
#include "amorbubble.moc"
#include <qpainter.h>
#include <ktextbrowser.h>
#include <qtooltip.h>
#include <kstandarddirs.h>
#include <X11/Xlib.h>
#include <X11/extensions/shape.h>
#include <qtimer.h>

#define ARROW_WIDTH     10
#define ARROW_HEIGHT    12
#define BORDER_SIZE     4
#define BUBBLE_OFFSET   16
#define BUBBLE_TIMEOUT  4000    // Minimum milliseconds to display a tip

//---------------------------------------------------------------------------
//
// Constructor
//
AmorBubble::AmorBubble()
	: QWidget(0, 0, WStyle_Customize | WStyle_NoBorder | WX11BypassWM )
{
    mOriginX = 0;
    mOriginY = 0;
    mBrowser = new KTextBrowser( this );
    mBrowser->setFrameStyle( QFrame::NoFrame | QFrame::Plain );
    mBrowser->setMargin( 0 );

    mBrowser->setWrapPolicy(QTextEdit::AtWordOrDocumentBoundary); // too long to fit in one line?

    QColorGroup clgrp = mBrowser->colorGroup();
    clgrp.setColor(QColorGroup::Text, Qt::black);
    //Laurent QTextBrowser didn't have this function FIX me
    //mBrowser->setPaperColorGroup( clgrp );
    mBrowser->setPaper( QToolTip::palette().active().brush( QColorGroup::Background ) );
    mBrowser->setVScrollBarMode( QTextBrowser::AlwaysOff );
    mBrowser->setHScrollBarMode( QTextBrowser::AlwaysOff );
    mBrowser->viewport()->installEventFilter( this );

    mBrowser->mimeSourceFactory()->addFilePath(KGlobal::dirs()->findResourceDir("data", "kdewizard/pics")+"kdewizard/pics/");
    QStringList icons = KGlobal::dirs()->resourceDirs("icon");
    QStringList::Iterator it;
    for (it = icons.begin(); it != icons.end(); ++it)
	mBrowser->mimeSourceFactory()->addFilePath(*it);

    mMouseWithin = false;
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
void AmorBubble::setMessage(const QString& message)
{
    mMessage = QString( "<html>%1</html>" ).arg( message );
    // hacks because heightForWidth() doesn't work.
    setGeometry( -1000, 0, 300, 1000 );
    show();
    mBrowser->setGeometry( 0, 0, 250, 1000 );
    mBrowser->setText( mMessage );
    calcGeometry();
}

//---------------------------------------------------------------------------
//
// Calculates the size, position and mask of the bubble
//
void AmorBubble::calcGeometry()
{
    mBound = QRect( 0, 0, 250, 0 );
//    mBound.setHeight( mBrowser->heightForWidth( mBound.width() ) );
    mBound.setHeight( mBrowser->contentsHeight() );
    mBound.moveBy(ARROW_WIDTH+BORDER_SIZE, BORDER_SIZE);

    // initialise the default geometry of the bubble
    int w = mBound.width() + BORDER_SIZE * 2 + ARROW_WIDTH;
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
	mBound.moveBy( -ARROW_WIDTH, 0 );
    }

    if (mOriginY + h > kapp->desktop()->height())
    {
        // source at bottom
        ypos = mOriginY - h + BORDER_SIZE + ARROW_HEIGHT / 2;
        mArrowVert = Bottom;
    }

    // Check for negative vertical bubble position (top of the screen)
    if ( ypos < 0 )
       ypos = 0;

    setGeometry(xpos, ypos, w, h);
    mBrowser->setGeometry( mBound );

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

    int left = ARROW_WIDTH;

    if (mArrowHorz == Left)
    {
        pointArray.setPoint(0, ARROW_WIDTH+1, 0);
        pointArray.setPoint(1, 0, -3);
        pointArray.setPoint(2, ARROW_WIDTH+1, ARROW_HEIGHT);
    }
    else
    {
        pointArray.setPoint(0, 0, 0);
        pointArray.setPoint(1, ARROW_WIDTH+1, -3);
        pointArray.setPoint(2, 0, ARROW_HEIGHT);
        pointArray.translate(width() - ARROW_WIDTH - 1, 0);
	left = 0;
    }

    if (mArrowVert == Top)
    {
        pointArray.translate(0, BORDER_SIZE + ARROW_HEIGHT / 2);
    }
    else
    {
        pointArray.translate(0, height() - BORDER_SIZE - ARROW_HEIGHT / 2);
    }

//    p.drawRoundRect(left, 0, width() - ARROW_WIDTH, height(), 10, 20);
    p.drawRect(left, 0, width() - ARROW_WIDTH, height());

    QPen pen(p.pen());
    p.setPen(NoPen);
    p.drawPolygon(pointArray);

    p.setPen(pen);
    p.drawPolyline(pointArray, 0, 3);
}

//---------------------------------------------------------------------------
//
// Draw the message in a bubble
//
void AmorBubble::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(black);
    painter.setBrush( QToolTip::palette().active().brush( QColorGroup::Background ) );
    drawBubble(painter);
}

//---------------------------------------------------------------------------
//
// The user clicked on the widget
//
void AmorBubble::mouseReleaseEvent(QMouseEvent *)
{
    hide();
}

//---------------------------------------------------------------------------
//
bool AmorBubble::eventFilter( QObject *, QEvent *e )
{
    switch ( e->type() )
    {

// GP	case QEvent::Enter:
// GP	    mBubbleTimer->stop();
// GP	    break;
// GP	case QEvent::Leave:
// GP	    if ( isVisible() )
// GP		mBubbleTimer->start( 1000, true );
// GP	    break;
	case QEvent::Enter:
	    mMouseWithin = true;
	    break;
	case QEvent::Leave:
	    mMouseWithin = false;
	    break;
	case QEvent::MouseButtonRelease:
	    hide();				// GP This is the only reason a bubble might posibly be created but hidden
	    break;
	default:
	    break;
    }

    return false;
}

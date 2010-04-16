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
#include "amorbubble.h"

#include <QtGui/QApplication>
#include <QtGui/QPainter>
#include <QtGui/QPaintEvent>
#include <QtGui/QPolygon>
#include <QtCore/QEvent>
#include <QtGui/QFrame>
#include <QtGui/QMouseEvent>
#include <QtGui/QDesktopWidget>
#include <QtGui/QToolTip>
#include <QtGui/QX11Info>
#include <QtCore/QTimer>

#include <ktextbrowser.h>
#include <kstandarddirs.h>
#include <kurl.h>

#include <X11/Xlib.h>
#include <X11/extensions/shape.h>

#define ARROW_WIDTH     10
#define ARROW_HEIGHT    12
#define BORDER_SIZE     4
#define BUBBLE_OFFSET   16
#define BUBBLE_TIMEOUT  4000    // Minimum milliseconds to display a tip



AmorBubble::AmorBubble()
  : QWidget( 0, Qt::WindowTitleHint | Qt::X11BypassWindowManagerHint ),
    mOriginX( 0 ),
    mOriginY( 0 )
{
    mBrowser = new KTextBrowser( this );
    mBrowser->setFrameStyle( QFrame::NoFrame | QFrame::Plain );
    //mBrowser->setMargin( 0 );

    mBrowser->setWordWrapMode( QTextOption::WrapAtWordBoundaryOrAnywhere ); // too long to fit in one line?

    QPalette clgrp = mBrowser->palette();
    clgrp.setColor( QPalette::Text, Qt::black );
    //Laurent QTextBrowser didn't have this function FIX me
    //mBrowser->setPaperColorGroup( clgrp );
    //mBrowser->setPaper( QToolTip::palette().active().brush( QPalette::Background ) );
    mBrowser->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );

    mBrowser->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    mBrowser->viewport()->installEventFilter( this );
#if 0 //I don(t know how to port it
    QList<KUrl> lst;
    lst << KUrl(KGlobal::dirs()->findResourceDir("data", "kdewizard/pics")+"kdewizard/pics/");

    QStringList icons = KGlobal::dirs()->resourceDirs("icon");
    QStringList::Iterator it;
    for (it = icons.begin(); it != icons.end(); ++it)
        lst << KUrl(*it);
    mBrowser->createMimeDataFromSelection ()->setUrls(lst);
#endif
    mMouseWithin = false;
}


void AmorBubble::setOrigin(int x, int y)
{
    mOriginX = x;
    mOriginY = y;
}


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


bool AmorBubble::mouseWithin()
{
    return mMouseWithin;
}


void AmorBubble::calcGeometry()
{
    mBound = QRect( 0, 0, 250, 0 );
    //mBound.setHeight( mBrowser->heightForWidth( mBound.width() ) );
#ifdef __GNUC__
#warning "kde4: porting"
#endif
    //mBound.setHeight( mBrowser->contentsHeight() );
    mBound.translate(ARROW_WIDTH+BORDER_SIZE, BORDER_SIZE);

    // initialise the default geometry of the bubble
    int w = mBound.width() + BORDER_SIZE * 2 + ARROW_WIDTH;
    int h = mBound.height() + BORDER_SIZE * 2;
    int xpos = mOriginX + BUBBLE_OFFSET;
    int ypos = mOriginY - BORDER_SIZE - ARROW_HEIGHT / 2;

    mArrowVert = Top;
    mArrowHorz = Left;

    // The actual position of the bubble may change if it is too close to
    // the desktop boundary.
    if( mOriginX + w > qApp->desktop()->width() ) {
        // source on right
        xpos = mOriginX - w - BUBBLE_OFFSET;
        mArrowHorz = Right;
        mBound.translate( -ARROW_WIDTH, 0 );
    }

    if( mOriginY + h > qApp->desktop()->height() ) {
        // source at bottom
        ypos = mOriginY - h + BORDER_SIZE + ARROW_HEIGHT / 2;
        mArrowVert = Bottom;
    }

    // Check for negative vertical bubble position (top of the screen)
    if( ypos < 0 ) {
       ypos = 0;
    }

    setGeometry( xpos, ypos, w, h );
    mBrowser->setGeometry( mBound );

    // create and apply the shape mask
    mMask = QPixmap( w, h );
    mMask.fill( Qt::color0 );
    QPainter maskPainter( &mMask );
    maskPainter.setPen( Qt::color1 );
    maskPainter.setBrush( Qt::color1 );
    drawBubble( maskPainter );
    XShapeCombineMask( QX11Info::display(), winId(), ShapeBounding, 0, 0, mMask.handle(), ShapeSet );
}


void AmorBubble::drawBubble(QPainter &painter)
{
    QPolygon pointArray(3);
    int left = ARROW_WIDTH;

    if( mArrowHorz == Left ) {
        pointArray.setPoint( 0, ARROW_WIDTH+1, 0 );
        pointArray.setPoint( 1, 0, -3 );
        pointArray.setPoint( 2, ARROW_WIDTH+1, ARROW_HEIGHT );
    }
    else {
        pointArray.setPoint( 0, 0, 0 );
        pointArray.setPoint( 1, ARROW_WIDTH+1, -3 );
        pointArray.setPoint( 2, 0, ARROW_HEIGHT );
        pointArray.translate( width() - ARROW_WIDTH - 1, 0 );
        left = 0;
    }

    if( mArrowVert == Top ) {
        pointArray.translate( 0, BORDER_SIZE + ARROW_HEIGHT / 2 );
    }
    else {
        pointArray.translate( 0, height() - BORDER_SIZE - ARROW_HEIGHT / 2 );
    }

    //p.drawRoundRect(left, 0, width() - ARROW_WIDTH, height(), 10, 20);
    painter.drawRect( left, 0, width() - ARROW_WIDTH, height() );

    QPen pen( painter.pen() );
    painter.setPen( Qt::NoPen );
    painter.drawPolygon( pointArray );

    painter.setPen( pen );
    painter.drawPolyline( pointArray );
}


void AmorBubble::paintEvent(QPaintEvent *e)
{
    Q_UNUSED( e );

    QPainter painter( this );
    painter.setPen( Qt::black );
    painter.setBrush( QToolTip::palette().brush( QPalette::Active, QPalette::Background ) );
    drawBubble( painter );
}


void AmorBubble::mouseReleaseEvent(QMouseEvent *e)
{
    Q_UNUSED( e );
    hide();
}


bool AmorBubble::eventFilter(QObject *obj, QEvent *e)
{
    Q_UNUSED( obj );

    switch( e->type() ) {
// GP   case QEvent::Enter:
// GP       mBubbleTimer->stop();
// GP       break;
// GP   case QEvent::Leave:
// GP       if( isVisible() )
// GP           mBubbleTimer->start( 1000, true );
// GP       break;
    case QEvent::Enter:
        mMouseWithin = true;
        break;
    case QEvent::Leave:
        mMouseWithin = false;
        break;
    case QEvent::MouseButtonRelease:
        hide();           // GP This is the only reason a bubble might posibly be created but hidden
        break;
    default:
        break;
    }

    return false;
}


// kate: word-wrap off; encoding utf-8; indent-width 4; tab-width 4; line-numbers on; mixed-indent off; remove-trailing-space-save on; replace-tabs-save on; replace-tabs on; space-indent on;
// vim:set spell et sw=4 ts=4 nowrap cino=l1,cs,U1:
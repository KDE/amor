/* amorbubble.h
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
#ifndef AMORBUBBLE_H
#define AMORBUBBLE_H

#include <config.h>

#include <kapplication.h>
#include <qwidget.h>
#include <qbitmap.h>
//Added by qt3to4:
#include <QMouseEvent>
#include <QEvent>
#include <QPaintEvent>
#include <QTextBrowser>
class QTimer;

//---------------------------------------------------------------------------
//
// AmorBubble displays a message in a shaped window
//
class AmorBubble : public QWidget
{
	Q_OBJECT
public:
	AmorBubble();
	virtual ~AmorBubble();

    void setOrigin(int x, int y) { mOriginX = x; mOriginY = y; }
    void setMessage(const QString& message);

    bool mouseWithin() { return mMouseWithin; }    

protected:
    enum VertPos { Top, Bottom };
    enum HorzPos { Left, Right };

    void calcGeometry();
    void drawBubble(QPainter &p);
    virtual void paintEvent(QPaintEvent *);
    virtual void mouseReleaseEvent(QMouseEvent *);
    virtual bool eventFilter( QObject *, QEvent * );

protected:
    QString mMessage;           // message to display
    int   mOriginX;             // X origin of bubble arrow
    int   mOriginY;             // Y origin of bubble arrow
    QRect mBound;               // bounds of the text
    QBitmap mMask;              // shape mask
    VertPos mArrowVert;         // vertical position of the arrow
    HorzPos mArrowHorz;         // horizontal position of the arrow
    QTextBrowser *mBrowser;	// displays the message

    bool mMouseWithin;		// the mouse pointer is inside the bubble
};

#endif // AMORBUBBLE_H


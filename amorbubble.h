//---------------------------------------------------------------------------
//
// amorbubble.h
//
// Copyright (c) 1999 Martin R. Jones <mjones@kde.org>
//

#ifndef AMORBUBBLE_H 
#define AMORBUBBLE_H 

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif 

#include <kapp.h>
#include <qwidget.h>
#include <qbitmap.h>

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
    void setMessage(QString message);

protected:
    enum VertPos { Top, Bottom };
    enum HorzPos { Left, Right };

    void calcGeometry();
    void drawBubble(QPainter &p);
    virtual void paintEvent(QPaintEvent *);
    virtual void mouseReleaseEvent(QMouseEvent *);

protected:
    QString mMessage;           // message to display
    int   mOriginX;             // X origin of bubble arrow
    int   mOriginY;             // Y origin of bubble arrow
    QRect mBound;               // bounds of the text
    QBitmap mMask;              // shape mask
    VertPos mArrowVert;         // vertical position of the arrow
    HorzPos mArrowHorz;         // horizontal position of the arrow
};

#endif // AMORBUBBLE_H 


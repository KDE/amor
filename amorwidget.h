//---------------------------------------------------------------------------
//
// amorwidget.h
//
// Copyright (c) 1999 Martin R. Jones <mjones@kde.org>
//

#ifndef AMORWIDGET_H 
#define AMORWIDGET_H 

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif 

#include <kapplication.h>
#include <qwidget.h>

//---------------------------------------------------------------------------
//
// AmorWidget displays a shaped pixmap
//
class AmorWidget : public QWidget
{
	Q_OBJECT
public:
	AmorWidget();
	virtual ~AmorWidget();

    void setPixmap(const QPixmap *pixmap);

signals:
    void mouseClicked(const QPoint &pos);
    void dragged( const QPoint &delta, bool release );

protected:
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);

protected:
    const QPixmap *mPixmap;
    QPoint clickPos;
    bool dragging;
};

#endif // AMORWIDGET_H 


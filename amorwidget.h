/* amorwidget.h
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
#ifndef AMORWIDGET_H 
#define AMORWIDGET_H 

#include <QtGui/QWidget>

class QPixmap;

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


/* amordialog.h
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
#ifndef AMORDIALOG_H
#define AMORDIALOG_H

#include <qdialog.h>
#include <qlistbox.h>
#include <qptrlist.h>
#include <qmultilineedit.h>
#include "amorconfig.h"
#include <kdialogbase.h>

//---------------------------------------------------------------------------
//
// AmorDialog provides a setup dialog.
//
class AmorDialog : public KDialogBase
{
    Q_OBJECT

public:
    AmorDialog();
    virtual ~AmorDialog();

signals:
    //-----------------------------------------------------------------------
    //
    // The configuration has changed in some way.
    //
    void changed();

    //-----------------------------------------------------------------------
    //
    // The animation offset has been changed.
    //
    void offsetChanged(int);

protected slots:
    void slotHighlighted(int);
    void slotOnTop(bool);
    void slotRandomTips(bool);
    void slotApplicationTips(bool);
    void slotOffset(int);
    void slotOk();
    void slotCancel();

protected:
    void readThemes();
    void addTheme(const QString& file);

protected:
    QListBox *mThemeListBox;
    QMultiLineEdit *mAboutEdit;
    QStringList mThemes;
    QStringList mThemeAbout;
    AmorConfig mConfig;
};

//---------------------------------------------------------------------------
//
// AmorListBoxItem implements a list box items for selection of themes
//
class AmorListBoxItem : public QListBoxItem
{
public:
    AmorListBoxItem(const QString & s, const QPixmap& p)
        : QListBoxItem(), mPixmap(p)
        { setText(s); }

protected:
    virtual void paint(QPainter *);
    virtual int height(const QListBox *) const;
    virtual int width(const QListBox *) const;
    virtual const QPixmap *pixmap() const { return &mPixmap; }

private:
    QPixmap mPixmap;
};

#endif // AMORDIALOG_H


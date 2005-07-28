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
** the Free Software Foundation, Inc., 51 Franklin Steet, Fifth Floor, Boston,
** MA 02110-1301, USA.
*/

/*
** Bug reports and questions can be sent to kde-devel@kde.org
*/
#ifndef AMORDIALOG_H
#define AMORDIALOG_H

#include <qdialog.h>
#include <q3listbox.h>
#include <q3ptrlist.h>
#include <q3multilineedit.h>
//Added by qt3to4:
#include <QPixmap>
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
    void slotRandomTheme(bool);
    void slotApplicationTips(bool);
    void slotOffset(int);
    void slotOk();
    void slotApply();
    void slotCancel();

protected:
    void readThemes();
    void addTheme(const QString& file);

protected:
    Q3ListBox *mThemeListBox;
    Q3MultiLineEdit *mAboutEdit;
    QStringList mThemes;
    QStringList mThemeAbout;
    AmorConfig mConfig;
};

//---------------------------------------------------------------------------
//
// AmorListBoxItem implements a list box items for selection of themes
//
class AmorListBoxItem : public Q3ListBoxItem
{
public:
    AmorListBoxItem(const QString & s, const QPixmap& p)
        : Q3ListBoxItem(), mPixmap(p)
        { setText(s); }

protected:
    virtual void paint(QPainter *);
    virtual int height(const Q3ListBox *) const;
    virtual int width(const Q3ListBox *) const;
    virtual const QPixmap *pixmap() const { return &mPixmap; }

private:
    QPixmap mPixmap;
};

#endif // AMORDIALOG_H


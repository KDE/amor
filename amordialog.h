//---------------------------------------------------------------------------
//
// amordialog.h
//
// Copyright (c) 1999 Martin R. Jones <mjones@kde.org>
//

#ifndef AMORDIALOG_H
#define AMORDIALOG_H

#include <qdialog.h>
#include <qlistbox.h>
#include <qlist.h>
#include <qmultilinedit.h>
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
    void slotOffset(int);
    void slotOk();
    void slotCancel();

protected:
    void readThemes();
    void addTheme(const QString& file);

protected:
    QListBox *mThemeListBox;
    QMultiLineEdit *mAboutEdit;
    QStrList mThemes;
    QStrList mThemeAbout;
    AmorConfig mConfig;
};

//---------------------------------------------------------------------------
//
// AmorListBoxItem implements a list box items for selection of themes
//
class AmorListBoxItem : public QListBoxItem
{
public:
    AmorListBoxItem(const char *s, const QPixmap& p)
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


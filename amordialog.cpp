/* amordialog.cpp
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

#include <qcheckbox.h>
#include <qlabel.h>
#include <qslider.h>
#include <qpainter.h>
//Added by qt3to4:
#include <QPixmap>
#include <QHBoxLayout>
#include <Q3VBox>
//Added by qt3to4:
#include <QVBoxLayout>
#include <kapplication.h>
#include <ksimpleconfig.h>
#include "amordialog.h"
#include "amordialog.moc"
#include "version.h"
#include <klocale.h>

#include <kstandarddirs.h>

//---------------------------------------------------------------------------
//
// Constructor
//
AmorDialog::AmorDialog()
    : KDialogBase(0, "amordlg", false, i18n("Options"), Ok|Apply|Cancel, Ok )
{
    mConfig.read();
    Q3VBox *mainwidget = makeVBoxMainWidget();

    QWidget *hb = new QWidget(mainwidget);
    QHBoxLayout *hboxLayout1 = new QHBoxLayout(hb);
    hb->setLayout(hboxLayout1);

    // Theme list
    QWidget *themeBox = new QWidget(hb);
    QVBoxLayout *vboxLayout2 = new QVBoxLayout(themeBox);
    themeBox->setLayout(vboxLayout2);
    vboxLayout2->setSpacing(spacingHint());

    QLabel *label = new QLabel(i18n("Theme:"), themeBox);

    mThemeListBox = new Q3ListBox(themeBox);
    connect(mThemeListBox,SIGNAL(highlighted(int)),SLOT(slotHighlighted(int)));
    mThemeListBox->setMinimumSize( fontMetrics().maxWidth()*20,
				   fontMetrics().lineSpacing()*6 );

    mAboutEdit = new Q3MultiLineEdit(themeBox);
    mAboutEdit->setReadOnly(true);
    mAboutEdit->setMinimumHeight( fontMetrics().lineSpacing()*4 );

    vboxLayout2->setStretchFactor(mThemeListBox, 4);
    vboxLayout2->setStretchFactor(mAboutEdit, 1);

    // Animation offset
    QWidget *offsetBox = new QWidget(hb);
    QVBoxLayout *vboxLayout3 = new QVBoxLayout(offsetBox);
    offsetBox->setLayout(vboxLayout3);
    vboxLayout3->setSpacing(spacingHint());
    label = new QLabel(i18n("Offset:"), offsetBox);

    QSlider *slider = new QSlider(-40, 40, 5, mConfig.mOffset,
                                    Qt::Vertical, offsetBox);
    connect(slider, SIGNAL(valueChanged(int)), SLOT(slotOffset(int)));

    // Always on top
    QCheckBox *checkBox = new QCheckBox(i18n("Always on top"), mainwidget);
    connect(checkBox, SIGNAL(toggled(bool)), SLOT(slotOnTop(bool)));
    checkBox->setChecked(mConfig.mOnTop);

    checkBox = new QCheckBox(i18n("Show random tips"), mainwidget);
    connect(checkBox, SIGNAL(toggled(bool)), SLOT(slotRandomTips(bool)));
    checkBox->setChecked(mConfig.mTips); // always keep this one after the connect, or the QList would not be grayed when it should

    checkBox = new QCheckBox(i18n("Use a random character"), mainwidget);
    connect(checkBox, SIGNAL(toggled(bool)), SLOT(slotRandomTheme(bool)));
    checkBox->setChecked(mConfig.mRandomTheme);

    checkBox = new QCheckBox(i18n("Allow application tips"), mainwidget);
    connect(checkBox, SIGNAL(toggled(bool)), SLOT(slotApplicationTips(bool)));
    checkBox->setChecked(mConfig.mAppTips);

    readThemes();
}

//---------------------------------------------------------------------------
//
// Destructor
//
AmorDialog::~AmorDialog()
{
}

//---------------------------------------------------------------------------
//
// Get list of all themes
//
void AmorDialog::readThemes()
{
    QStringList files(KGlobal::dirs()->findAllResources("appdata", "*rc"));
    for (QStringList::ConstIterator it = files.begin();
	 it != files.end();
	 it++)
      addTheme(*it);
}

//---------------------------------------------------------------------------
//
// Add a single theme to the list
//
void AmorDialog::addTheme(const QString& file)
{
    KSimpleConfig config(file, true);

    config.setGroup("Config");

    QString pixmapPath = config.readPathEntry("PixmapPath");
    if (pixmapPath.isEmpty())
    {
        return;
    }

    pixmapPath += "/";

    if (pixmapPath[0] != '/')
    {
        // relative to config file. We add a / to indicate the dir
        pixmapPath = locate("appdata", pixmapPath);
    }

    QString description = config.readEntry("Description");
    QString about = config.readEntry("About", " ");
    QString pixmapName = config.readEntry("Icon");

    pixmapPath += pixmapName;

    QPixmap pixmap(pixmapPath);

    AmorListBoxItem *item = new AmorListBoxItem(description, pixmap);
    mThemeListBox->insertItem(item);
    mThemes.append(file);
    mThemeAbout.append(about);

    if (mConfig.mTheme == file)
    {
        mThemeListBox->setSelected(mThemeListBox->count()-1, true);
    }
}

//---------------------------------------------------------------------------
//
// User highlighted a theme
//
void AmorDialog::slotHighlighted(int index)
{
    mConfig.mTheme = mThemes.at(index);
    mAboutEdit->setText(mThemeAbout.at(index));
}

//---------------------------------------------------------------------------
//
// User changed offset
//
void AmorDialog::slotOffset(int off)
{
    mConfig.mOffset = off;
    emit offsetChanged(mConfig.mOffset);
}

//---------------------------------------------------------------------------
//
// User toggled always on top
//
void AmorDialog::slotOnTop(bool onTop)
{
    mConfig.mOnTop = onTop;
}

//---------------------------------------------------------------------------
//
// User toggled random tips
//
void AmorDialog::slotRandomTips(bool tips)
{
    mConfig.mTips = tips;
}

//---------------------------------------------------------------------------
//
// User toggled random character
//
void AmorDialog::slotRandomTheme(bool randomTheme)
{
    mThemeListBox->setEnabled(!randomTheme);
    mConfig.mRandomTheme = randomTheme;
}

//---------------------------------------------------------------------------
//
// User toggled application tips
//
void AmorDialog::slotApplicationTips(bool tips)
{
    mConfig.mAppTips = tips;
}

//---------------------------------------------------------------------------
//
// User clicked Ok
//
void AmorDialog::slotOk()
{
    mConfig.write();
    emit changed();
    accept();
}

//---------------------------------------------------------------------------
//
// User clicked Ok
//
void AmorDialog::slotApply()
{
    mConfig.write();
    emit changed();
}

//---------------------------------------------------------------------------
//
// User clicked Cancel
//
void AmorDialog::slotCancel()
{
    // restore offset
    KConfig *config = kapp->config();
    KConfigGroupSaver cs(config, "General");
    emit offsetChanged(config->readNumEntry("Offset"));
    reject();
}

//===========================================================================
//
// AmorListBoxItem implements a list box items for selection of themes
//
void AmorListBoxItem::paint( QPainter *p )
{
    p->drawPixmap( 3, 0, mPixmap );
    QFontMetrics fm = p->fontMetrics();
    int yPos;                       // vertical text position
    if ( mPixmap.height() < fm.height() )
        yPos = fm.ascent() + fm.leading()/2;
    else
        yPos = mPixmap.height()/2 - fm.height()/2 + fm.ascent();
    p->drawText( mPixmap.width() + 5, yPos, text() );
}

int AmorListBoxItem::height(const Q3ListBox *lb ) const
{
    return QMAX( mPixmap.height(), lb->fontMetrics().lineSpacing() + 1 );
}

int AmorListBoxItem::width(const Q3ListBox *lb ) const
{
    return mPixmap.width() + lb->fontMetrics().width( text() ) + 6;
}


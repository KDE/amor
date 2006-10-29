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
** the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
** MA 02110-1301, USA.
*/

/*
** Bug reports and questions can be sent to kde-devel@kde.org
*/

#include <qcheckbox.h>
#include <qlabel.h>
#include <qlistwidget.h>
#include <qslider.h>
#include <qtextedit.h>
#include <QPixmap>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <ksimpleconfig.h>
#include "amordialog.h"
#include "amordialog.moc"
#include "version.h"
#include <kicon.h>
#include <klocale.h>
#include <kstandarddirs.h>
#include <kglobal.h>

//---------------------------------------------------------------------------
//
// Constructor
//
AmorDialog::AmorDialog()
    : KDialog(0)
{
    setCaption( i18n("Options") );
    setButtons( Ok|Apply|Cancel );
    setDefaultButton( Ok );

    mConfig.read();
    QWidget *mainwidget = new QWidget(this);
    setMainWidget( mainwidget );
    QGridLayout *gridLayout = new QGridLayout(mainwidget);
    gridLayout->setMargin(0);

    // Theme list
    QLabel *label = new QLabel(i18n("Theme:"), mainwidget);
    gridLayout->addWidget(label, 0, 0);

    mThemeListView = new QListWidget(mainwidget);
    mThemeListView->setIconSize(QSize(32, 32));
    mThemeListView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    mThemeListView->setAlternatingRowColors(true);
    connect(mThemeListView,SIGNAL(currentRowChanged(int)),SLOT(slotHighlighted(int)));
    mThemeListView->setMinimumSize( fontMetrics().maxWidth()*20,
				   fontMetrics().lineSpacing()*6 );
    gridLayout->addWidget(mThemeListView, 1, 0);

    mAboutEdit = new QTextEdit(mainwidget);
    mAboutEdit->setReadOnly(true);
    mAboutEdit->setMinimumHeight( fontMetrics().lineSpacing()*4 );
    gridLayout->addWidget(mAboutEdit, 2, 0);

    // Animation offset
    label = new QLabel(i18n("Offset:"), mainwidget);
    gridLayout->addWidget(label, 0, 1);

    QSlider *slider = new QSlider(Qt::Vertical, mainwidget);
    slider->setRange(-40, 40);
    slider->setPageStep(5);
    slider->setValue(mConfig.mOffset);
    connect(slider, SIGNAL(valueChanged(int)), SLOT(slotOffset(int)));
    gridLayout->addWidget(slider, 1, 1, 2, 1);

    // Always on top
    QCheckBox *checkBox = new QCheckBox(i18n("Always on top"), mainwidget);
    connect(checkBox, SIGNAL(toggled(bool)), SLOT(slotOnTop(bool)));
    checkBox->setChecked(mConfig.mOnTop);
    gridLayout->addWidget(checkBox, 3, 0, 1, 2);

    checkBox = new QCheckBox(i18n("Show random tips"), mainwidget);
    connect(checkBox, SIGNAL(toggled(bool)), SLOT(slotRandomTips(bool)));
    checkBox->setChecked(mConfig.mTips); // always keep this one after the connect, or the QList would not be grayed when it should
    gridLayout->addWidget(checkBox, 4, 0, 1, 2);

    checkBox = new QCheckBox(i18n("Use a random character"), mainwidget);
    connect(checkBox, SIGNAL(toggled(bool)), SLOT(slotRandomTheme(bool)));
    checkBox->setChecked(mConfig.mRandomTheme);
    gridLayout->addWidget(checkBox, 5, 0, 1, 2);

    checkBox = new QCheckBox(i18n("Allow application tips"), mainwidget);
    connect(checkBox, SIGNAL(toggled(bool)), SLOT(slotApplicationTips(bool)));
    checkBox->setChecked(mConfig.mAppTips);
    gridLayout->addWidget(checkBox, 6, 0, 1, 2);

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
    QStringList files;

    // Non-recursive search for theme files, with the relative paths stored
    // in files so that absolute paths are not used.
    KGlobal::dirs()->findAllResources("appdata", "*rc", false, false, files);

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
    KSimpleConfig config(KStandardDirs::locate("appdata", file), true);

    config.setGroup("Config");

    QString pixmapPath = config.readPathEntry("PixmapPath");
    if (pixmapPath.isEmpty())
    {
        return;
    }

    pixmapPath += '/';

    if (pixmapPath[0] != '/')
    {
        // relative to config file. We add a / to indicate the dir
        pixmapPath = KStandardDirs::locate("appdata", pixmapPath);
    }

    QString description = config.readEntry("Description");
    QString about = config.readEntry("About", " ");
    QString pixmapName = config.readEntry("Icon");

    pixmapPath += pixmapName;

    QPixmap pixmap(pixmapPath);

    QListWidgetItem *item = new QListWidgetItem(KIcon(pixmap), description, mThemeListView);
    mThemes.append(file);
    mThemeAbout.append(about);

    if (mConfig.mTheme == file)
    {
        mThemeListView->setCurrentItem(item);
    }
}

//---------------------------------------------------------------------------
//
// User highlighted a theme
//
void AmorDialog::slotHighlighted(int index)
{
    if (index < 0)
        return;
    mConfig.mTheme = mThemes.at(index);
    mAboutEdit->setPlainText(mThemeAbout.at(index));
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
    mThemeListView->setEnabled(!randomTheme);
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
    KConfig *config = KGlobal::config();
    KConfigGroup cs(config, "General");
    emit offsetChanged(cs.readEntry("Offset",0));
    reject();
}

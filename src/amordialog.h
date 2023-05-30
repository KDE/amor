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
#ifndef AMORDIALOG_H
#define AMORDIALOG_H

#include <QDialog>
#include "amorconfig.h"

class QListWidget;
class QTextBrowser;


class AmorDialog : public QDialog
{
    Q_OBJECT

    public:
        explicit AmorDialog(QWidget *parent = nullptr);

    Q_SIGNALS:
        void changed();
        void offsetChanged(int);

    protected Q_SLOTS:
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
        void addTheme(const QString& folder, const QString& file);

    protected:
        QListWidget *mThemeListView;
        QTextBrowser *mAboutEdit;
        QStringList mThemes;
        QStringList mThemeAbout;
        AmorConfig mConfig;
};


#endif

// kate: word-wrap off; encoding utf-8; indent-width 4; tab-width 4; line-numbers on; mixed-indent off; remove-trailing-space-save on; replace-tabs-save on; replace-tabs on; space-indent on;
// vim:set spell et sw=4 ts=4 nowrap cino=l1,cs,U1:

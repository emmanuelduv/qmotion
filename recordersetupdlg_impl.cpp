/***************************************************************************
 *   Copyright (C) 2008, 2009, 2010 by Stephane List                       *
 *   slist@lilotux.net                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "recordersetupdlg_impl.h"
#include "recordersetupdlg.h"
#include "ui_recordersetupdlg.h"

#include <QSettings>
#include <QFileDialog>

//------------------------------------------------------------------------
/// Constructeur
//------------------------------------------------------------------------
RecorderSetupDlg::Impl::Impl(RecorderSetupDlg& Intf)
    : Intf_(Intf)
{
    QSettings settings;

    ui_.setupUi(&Intf);

    settings.beginGroup("recorder");
    ui_.dir->setText(settings.value("dir", "").toString());
    settings.endGroup();
}

//------------------------------------------------------------------------
/// Evt : bouton Valider
//------------------------------------------------------------------------
void RecorderSetupDlg::Impl::on_buttonBox_accepted()
{
    QSettings settings;

    settings.beginGroup("recorder");
    settings.setValue("dir", ui_.dir->text());
    settings.endGroup();
    settings.sync();
}

void RecorderSetupDlg::Impl::on_buttonDir_clicked()
{

    QString dir = QFileDialog::getExistingDirectory(&Intf_,
                                                    tr("Choose Directory"),
                                                    "/home");

    if (!dir.isEmpty())
        ui_.dir->setText(dir);
}


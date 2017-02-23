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

#include "ftpsetupdlg_impl.h"
#include "ftpsetupdlg.h"
#include "ui_ftpsetupdlg.h"

#include <QSettings>
#include <QFileDialog>

//------------------------------------------------------------------------
/// Constructeur
//------------------------------------------------------------------------
FTPSetupDlg::Impl::Impl(FTPSetupDlg& Intf)
    : Intf_(Intf)
{
    QSettings settings;

    ui_.setupUi(&Intf);

    settings.beginGroup("ftp");
    ui_.server->setText(settings.value("server", "").toString());
    ui_.serverport->setValue(settings.value("serverport", 21).toUInt());
    ui_.login->setText(settings.value("login", "").toString());
    ui_.password->setText(settings.value("password", "").toString());
    ui_.directory->setText(settings.value("directory", "").toString());
    settings.endGroup();
}

//------------------------------------------------------------------------
/// Evt : bouton Valider
//------------------------------------------------------------------------
void FTPSetupDlg::Impl::on_buttonBox_accepted()
{
    QSettings settings;

    settings.beginGroup("ftp");
    settings.setValue("server", ui_.server->text());
    settings.setValue("serverport", ui_.serverport->value());
    settings.setValue("login", ui_.login->text());
    settings.setValue("password", ui_.password->text());
    settings.setValue("directory", ui_.directory->text());
    settings.endGroup();
    settings.sync();
}


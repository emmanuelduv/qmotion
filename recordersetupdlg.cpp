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

//------------------------------------------------------------------------
/// recordersetupdlg.cpp : Implantation de RecorderSetupDlg
//------------------------------------------------------------------------

#include "recordersetupdlg.h"
#include "recordersetupdlg_impl.h"
#include "ui_recordersetupdlg.h"

//------------------------------------------------------------------------
/// Constructeur
//------------------------------------------------------------------------
RecorderSetupDlg::RecorderSetupDlg(QWidget* Parent,Qt::WindowFlags Flags)
    :	QDialog(Parent,Flags)
{
    pImpl_ = new Impl(*this);
}

//------------------------------------------------------------------------
/// Destructeur
//------------------------------------------------------------------------
RecorderSetupDlg::~RecorderSetupDlg()
{
    delete pImpl_;
}

//------------------------------------------------------------------------
/// Evt : bouton Valider
//------------------------------------------------------------------------
void RecorderSetupDlg::on_buttonBox_accepted()
{
    pImpl_->on_buttonBox_accepted();
}

void RecorderSetupDlg::on_buttonDir_clicked()
{
    pImpl_->on_buttonDir_clicked();
}

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

#ifndef _FTPSETUPDLG_H_
#define _FTPSETUPDLG_H_

#include <QDialog>
#include <QWidget>

class FTPSetupDlg : public QDialog
{
    Q_OBJECT

public:
    FTPSetupDlg(QWidget* Parent = 0, Qt::WindowFlags Flags = 0);
    ~FTPSetupDlg();

private slots:
    void on_buttonBox_accepted();

private:
    class Impl;
    Impl* pImpl_;
};
#endif

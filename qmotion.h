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

#ifndef _QMOTION_H_
#define _QMOTION_H_

#include <QTimer>
#include <QColor>
#include <QImage>
#include <QMutex>

#include "ui_qmotion.h"
#include "recordersetupdlg.h"
#include "ftpsetupdlg.h"
#include "capturethread.h"

class QMotion : public QMainWindow, private Ui::QMainWindowBase
{
    Q_OBJECT

public:
    QMotion(QWidget *parent = 0);

public slots:
    void update_images(const QImage& image, const QImage& motion);
    void acquired();
    void newSize(QSize size);

private slots:
    void about();
    void dir_settings();
    void ftp_settings();
    void on_pushButton_color_clicked();
    void on_global_marker_stateChanged(int);
    void on_component_markers_stateChanged(int);

signals:
    void motion();

protected:
    void timerEvent(QTimerEvent *event);

private:
    QColor color_;
    int counter_;
    CaptureThread captureThread_;
    QSettings settings;
};

#endif


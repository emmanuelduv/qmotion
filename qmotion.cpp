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

#include <QMessageBox>
#include <QDebug>
#include <QDir>
#include <QColorDialog>
#include <QUrl>

#include "qmotion.h"
#include "formatconverter.h"
#include "mailsender.h"
#include "version.h"

QMotion::QMotion(QWidget *parent)
    : QMainWindow(parent),
      counter_(0)
{
    qRegisterMetaType< cv::Mat >("cv::Mat");
    setupUi(this);
    startTimer(1000);

    color_ = QColor(settings.value("color", "blue").toString());
    frame_color->setPalette(QPalette(color_));
    frame_color->setAutoFillBackground(true);
    limit_fps->setValue(settings.value("limit_fps", 25).toInt());
    flip->setChecked(settings.value("flip",0).toBool());
    checkBox_disable_display->setChecked(settings.value("display",0).toBool());
    threshold->setValue(settings.value("detection_threshold", 30).toInt());
    checkBox_timestamp->setChecked(settings.value("timestamp", false).toBool());
    checkBox_save->setChecked(settings.value("save_motion_infile", false).toBool());
    checkBox_ftp->setChecked(settings.value("save_motion_inftp", false).toBool());
    component_markers->setChecked(settings.value("show_component", true).toBool());
    global_marker->setChecked(settings.value("show_global", true).toBool());

    QObject::connect(actionQuit, &QAction::triggered, qApp, &QApplication::quit);
    QObject::connect(actionAbout_QT, &QAction::triggered, qApp, &QApplication::aboutQt);
    QObject::connect(actionAbout_QMotion, &QAction::triggered, this, &QMotion::about);
    QObject::connect(action_Directory_settings, &QAction::triggered, this, &QMotion::dir_settings);
    QObject::connect(action_FTP_settings, &QAction::triggered, this, &QMotion::ftp_settings);

    QObject::connect(&captureThread_, &CaptureThread::webcamError, label_video, &QLabel::setText, Qt::QueuedConnection);
    QObject::connect(&captureThread_, &CaptureThread::motionOutput, this, &QMotion::update_images, Qt::QueuedConnection);
    QObject::connect(&captureThread_, &CaptureThread::acquired, this, &QMotion::acquired, Qt::QueuedConnection);
    QObject::connect(&captureThread_, &CaptureThread::newSize, this, &QMotion::newSize, Qt::QueuedConnection);
    QObject::connect(this, &QMotion::updateFps, &captureThread_, &CaptureThread::updateFps, Qt::QueuedConnection);

    QObject::connect(limit_fps, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int value){
        settings.setValue("limit_fps", value);
        emit this->updateFps();
    });
    QObject::connect(threshold, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int value){
        settings.setValue("detection_threshold", value);
    });
    QObject::connect(flip, &QCheckBox::toggled, [=](bool checked){
        settings.setValue("flip", checked);
    });
    QObject::connect(checkBox_timestamp, &QCheckBox::toggled, [=](bool checked){
        settings.setValue("timestamp", checked);
    });
    QObject::connect(checkBox_disable_display, &QCheckBox::toggled, [=](bool checked){
        settings.setValue("display", checked);
    });
    QObject::connect(checkBox_save, &QCheckBox::toggled, [=](bool checked){
        settings.setValue("save_motion_infile", checked);
    });
    QObject::connect(checkBox_ftp, &QCheckBox::toggled, [=](bool checked){
        settings.setValue("save_motion_inftp", checked);
    });
    QObject::connect(component_markers, &QCheckBox::toggled, [=](bool checked){
        settings.setValue("show_component", checked);
    });
    QObject::connect(global_marker, &QCheckBox::toggled, [=](bool checked){
        settings.setValue("show_global", checked);
    });
}

void QMotion::update_images(const QImage& image, const QImage& motion)
{
    label_video->setPixmap(QPixmap::fromImage(image));
    label_motion->setPixmap(QPixmap::fromImage(motion));
}

void QMotion::acquired()
{
    counter_++;
}

void QMotion::newSize(QSize size)
{
    label_size->setText(tr("size: %1x%2").arg(size.width()).arg(size.height()));
}

void QMotion::timerEvent(QTimerEvent *event)
{
    lcdNumber->display(counter_);
    counter_ = 0;
}

void QMotion::about()
{
    QMessageBox::about(this,
       tr("About"),
       tr("<p><b>QMotion</b></p>"
          "<p>Version QMotion: %1"
          "<p>Version OpenCV: %2"
          "<p>Version %3: %4.%5"
          "</p>")
       .arg(VERSION)
       .arg(CV_VERSION)
#ifdef __GNUC__
       .arg("GCC")
       .arg(__GNUC__)
       .arg(__GNUC_MINOR__)
#elif defined(_MSC_VER)
       .arg("VC++")
       .arg(_MSC_VER / 100)
       .arg(_MSC_VER % 100,2,10,QChar('0'))
#else
       .arg("Compiler")
       .arg("?")
       .arg("?")
#endif
    );
}

void QMotion::dir_settings()
{
    RecorderSetupDlg dialog(0,  Qt::WindowTitleHint | Qt::WindowSystemMenuHint);
    dialog.exec();
}

void QMotion::ftp_settings()
{
    FTPSetupDlg dialog(0,  Qt::WindowTitleHint | Qt::WindowSystemMenuHint);
    dialog.exec();
}

void QMotion::on_pushButton_color_clicked()
{
    QColor color = QColorDialog::getColor(color_, this);
    if (color.isValid()) {
        frame_color->setPalette(QPalette(color));
        frame_color->setAutoFillBackground(true);
        color_ = color;
        settings.setValue("color", color_.name());
    }
}

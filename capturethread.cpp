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

#include "capturethread.h"
#include "formatconverter.h"

#include <QDebug>
#include <QTime>
#include <QDir>
#include <QBuffer>
#include <QMetaType>
#include <QNetworkRequest>
#include <QPainter>
#include <QDateTime>
#include <QEventLoop>

CaptureThread::CaptureThread(QObject *parent)
    : QThread(parent),
      ftp_(nullptr)
{
    start();
    qDebug() << "CaptureThread::CaptureThread";
    if (!capture_.open(0))
    {
        qWarning("No webcam");
        emit webcamError(tr("No webcam"));
        return;
    }
    capture_.set(cv::CAP_PROP_FRAME_WIDTH, 10000);
    capture_.set(cv::CAP_PROP_FRAME_HEIGHT, 10000);

    startTimer(1000/settings.value("limit_fps", 25).toInt());

    QObject::connect(this, &CaptureThread::output, &motionDetector_, &MotionDetector::input);
    QObject::connect(&motionDetector_, &MotionDetector::motion, this, &CaptureThread::motion_treatment);
    QObject::connect(&motionDetector_, &MotionDetector::output, this, &CaptureThread::motionOutput);
}


CaptureThread::~CaptureThread()
{
    qDebug() << "CaptureThread::~CaptureThread";
    emit this->abort();
    if (!wait(1000)) // The thread has 1 second to finish
    {
        qDebug() << "thread takes more than 1 second to finish, use the force !";
    }
    if (capture_.isOpened())
    {
        capture_.release();
    }
}


void CaptureThread::run()
{
    qDebug() << "CaptureThread::run";

    QEventLoop wait;
    QObject::connect(this, &CaptureThread::abort, &wait, &QEventLoop::quit);
    wait.exec();
}

void CaptureThread::timerEvent(QTimerEvent *event)
{
    if ( !capture_.isOpened() )
    {
        qDebug() << "capture_ not initialised";
        emit webcamError(tr("No webcam"));
        return;
    }

    capture_ >> image_;

    emit acquired();
    motionDetector_.input(image_);
}

void CaptureThread::motion_treatment()
{
    qImage_ = cvMatToQImage(image_, settings.value("flip",0).toBool());

    if(settings.value("timestamp", false).toBool())
        add_timestamp(qImage_);

    if (settings.value("save_motion_infile", false).toBool()){
        QString snapshot_file;
        settings.beginGroup("recorder");
        snapshot_file = settings.value("dir", QDir::homePath()).toString();
        if (!snapshot_file.endsWith('/'))
        {
            snapshot_file += "/";
        }
        snapshot_file += "qmotion__" + QDate::currentDate().toString("yyyy_MM_dd");

        if(settings.value("flat", 0).toBool())
           snapshot_file += "__" + QTime::currentTime().toString("hh_");
        else {
             snapshot_file += "/" + QTime::currentTime().toString("hh") + "/";

             QDir out(snapshot_file);
             if(!out.exists()){
                 out.mkpath(snapshot_file);
             }
        }

        snapshot_file += QTime::currentTime().toString("mm_ss_zzz") + ".jpg";
        settings.endGroup();
        if(!qImage_.save(snapshot_file))
            qWarning() << "Error saving in " << snapshot_file;
    }

    // FTP Upload
    if (settings.value("save_motion_inftp", false).toBool()){
        QString snapshot_file;
        settings.beginGroup("ftp");
        snapshot_file = "qmotion__" + QDate::currentDate().toString("yyyy_MM_dd") + "__" + QTime::currentTime().toString("hh_mm_ss_zzz") + ".jpg";

        if (!ftp_)
        {
            QUrl url(settings.value("server").toString());
            if(url.isValid()){
                QString directory = settings.value("directory").toString();
                ftp_ = new QNetworkAccessManager(this);
                if(!directory.endsWith("/")){
                    directory += "/";
                    settings.setValue("directory", directory);
                }

                ftpUrl_ = QUrl("ftp://"+settings.value("server").toString()+":"+QString::number(settings.value("serverport").toUInt()) + settings.value("directory").toString());

                if (!settings.value("login").toString().isEmpty()){
                    ftpUrl_.setUserName(settings.value("login").toString());
                    ftpUrl_.setPassword(settings.value("password").toString());
               }
            }
        }
        if(ftp_){
            QByteArray ba;
            QBuffer buffer(&ba);
            buffer.open(QIODevice::WriteOnly);
            qImage_.save(&buffer, "JPG");

            QUrl url(ftpUrl_);
            url.setUrl(url.url() + snapshot_file);
            QNetworkRequest req(url);
            ftp_->put(req, ba);
        }
        settings.endGroup();
    }

    // send mail
    /*if (checkBox_mail->isChecked())
    {
        QString snapshot_file;
        snapshot_file = QDir::tempPath();
        if (!snapshot_file.endsWith('/'))
        {
            snapshot_file += "/";
        }
        snapshot_file += "qmotion__" + QDate::currentDate().toString("yyyy_MM_dd") + "__" + QTime::currentTime().toString("hh_mm_ss_zzz") + ".png";

        if (qImage_.save(snapshot_file))
        {
            qDebug() << "Saved in " << snapshot_file;
        }
        else
        {
            qWarning() << "Error saving in " << snapshot_file;
        }
        emit mail_file(snapshot_file);
    }*/
}

void CaptureThread::add_timestamp(QImage & img)
{
    QFont def_font;
    QFontMetrics fm(def_font);
    int textHeightInPixels = fm.height();

    QPainter painter(&img);
    QRect bottomrect(0, img.rect().bottom() - textHeightInPixels, img.width(), textHeightInPixels);
    painter.setOpacity(0.5);
    painter.fillRect(bottomrect, Qt::white);

    painter.setOpacity(1);
    painter.setPen(Qt::black);

    QDate d(QDate::currentDate());
    QTime t(QTime::currentTime());
    painter.drawText(img.rect(), Qt::AlignHCenter|Qt::AlignBottom, d.toString(Qt::DefaultLocaleLongDate) + " " + t.toString("hh:mm:ss.zzz"));
}

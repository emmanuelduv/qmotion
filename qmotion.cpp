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
#include <QSettings>
#include <QDebug>
#include <QDir>
#include <QColorDialog>
#include <QPainter>
#include <QDateTime>
#include <QUrl>
#include <QBuffer>
#include <QMetaType>

#include "qmotion.h"
#include "formatconverter.h"
#include "mailsender.h"
#include "version.h"

QMotion::QMotion(QWidget *parent)
    : QMainWindow(parent),
      //    ftp_(NULL),
      counter_(0),
      counter_last_(0)
{
    qRegisterMetaType< cv::Mat >("cv::Mat");
    QSettings settings;
    setupUi(this);

    QColor blue(0,0,255);
    color_ = QColor(settings.value("color", blue.name()).toString());
    frame_color->setPalette(QPalette(color_));
    frame_color->setAutoFillBackground(true);
    motionDetector_.set_motion_color(color_);

    int lim = settings.value("limit_fps", 25).toInt();
    limit_fps->setValue(lim);
    captureThread_.set_fps_limit(lim);
    QObject::connect(limit_fps, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), &captureThread_, &CaptureThread::set_fps_limit, Qt::QueuedConnection);

    Flip->setChecked(settings.value("flip",0).toBool());
    checkBox_disable_display->setChecked(settings.value("display",0).toBool());

    int thr = settings.value("detection_threshold", 30).toInt();
    motionDetector_.set_threshold(thr);
    threshold->setValue(thr);
    QObject::connect(threshold, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), &motionDetector_, &MotionDetector::set_threshold);

    checkBox_timestamp->setChecked(settings.value("timestamp", false).toBool());
    checkBox_save->setChecked(settings.value("save_motion_infile", false).toBool());
    checkBox_ftp->setChecked(settings.value("save_motion_inftp", false).toBool());

    QObject::connect(actionQuit, &QAction::triggered, qApp, &QApplication::quit);
    QObject::connect(actionAbout_QT, &QAction::triggered, qApp, &QApplication::aboutQt);
    QObject::connect(actionAbout_QMotion, &QAction::triggered, this, &QMotion::about);
    QObject::connect(action_Directory_settings, &QAction::triggered, this, &QMotion::dir_settings);
    QObject::connect(action_FTP_settings, &QAction::triggered, this, &QMotion::ftp_settings);

    QObject::connect(&motionDetector_, &MotionDetector::motion, this, &QMotion::motion_treatment);

    timer_fps_.start(1000);
    QObject::connect(&timer_fps_, &QTimer::timeout, this, &QMotion::fps_update);

    QObject::connect(&captureThread_, &CaptureThread::output, this, &QMotion::update_image, Qt::QueuedConnection);
    QObject::connect(&captureThread_, &CaptureThread::webcamError, label_video, &QLabel::setText, Qt::QueuedConnection);

    QObject::connect(&captureThread_, &CaptureThread::output, &motionDetector_, &MotionDetector::input, Qt::QueuedConnection);
    QObject::connect(&motionDetector_, &MotionDetector::output, this, &QMotion::update_motion);
    captureThread_.start();

    QObject::connect(this, &QMotion::mail_file, this, &QMotion::mail);
}

void QMotion::update_image(const cv::Mat & image)
{
    qImage_ = cvMatToQImage(image, Flip->isChecked());

    if (counter_ == 0)
    {
        label_size->setText(tr("size: %1x%2").arg(qImage_.width()).arg(qImage_.height()));
        label_video->setMinimumHeight(qImage_.height() * qImage_.width() / label_video->width());
        video_size_ = qImage_.size();
    }
    if (checkBox_timestamp->isChecked())
    {
        add_timestamp(qImage_);
    }
    if (!checkBox_disable_display->isChecked())
    {
        label_video->setPixmap(QPixmap::fromImage(qImage_.scaled(label_video->width(),label_video->height()),Qt::AutoColor));
    }
    counter_++;
}

void QMotion::update_motion(const cv::Mat& image)
{
    if (!checkBox_disable_display->isChecked())
    {
        QImage tmp = cvMatToQImage(image, Flip->isChecked());
        label_motion->setPixmap(QPixmap::fromImage(tmp.scaled(label_motion->width(),label_motion->height()),Qt::AutoColor));
    }
}

QMotion::~QMotion()
{
    qDebug() << "~QMotion()";
    QSettings settings;
    settings.setValue("limit_fps", limit_fps->value());
    settings.setValue("timestamp", checkBox_timestamp->isChecked());
    settings.setValue("color", color_.name());
    settings.setValue("flip",Flip->isChecked());
    settings.setValue("display",checkBox_disable_display->isChecked());
    settings.setValue("detection_threshold",threshold->value());
    settings.setValue("save_motion_infile", checkBox_save->isChecked());
    settings.setValue("save_motion_inftp", checkBox_ftp->isChecked());
}

void QMotion::add_timestamp(QImage & img)
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

void QMotion::fps_update()
{
    lcdNumber->display(counter_ - counter_last_);
    counter_last_ = counter_;
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

void QMotion::motion_treatment()
{
    if (checkBox_save->isChecked())
    {
        QSettings settings;
        QString snapshot_file;
        settings.beginGroup("recorder");
        snapshot_file = settings.value("dir", QDir::homePath()).toString();
        if (!snapshot_file.endsWith('/'))
        {
            snapshot_file += "/";
        }
        snapshot_file += "qmotion__" + QDate::currentDate().toString("yyyy_MM_dd") + "__" + QTime::currentTime().toString("hh_mm_ss_zzz") + ".jpg";
        settings.endGroup();
        if (qImage_.save(snapshot_file))
        {
            qDebug() << "Saved in " << snapshot_file;
        }
        else
        {
            qWarning() << "Error saving in " << snapshot_file;
        }
    }

    // FTP Upload
    if (checkBox_ftp->isChecked())
    {
        QSettings settings;
        QString snapshot_file;
        settings.beginGroup("ftp");
        snapshot_file = "qmotion__" + QDate::currentDate().toString("yyyy_MM_dd") + "__" + QTime::currentTime().toString("hh_mm_ss_zzz") + ".jpg";
#if 0
        if (!ftp_)
        {
            ftp_ = new QFtp(this);
            ftp_state_ = QFtp::Unconnected;
            connect(ftp_, SIGNAL(stateChanged(int)), this, SLOT(FTPstateChanged(int)));
            QUrl url(settings.value("server").toString());
            if (!url.isValid())
            {
                qDebug() << "Invalid ftp url :" << url;
                ftp_->deleteLater();
                ftp_ = NULL;
            }
            else
            {
                qDebug() << "FTP connect";
                ftp_->connectToHost(settings.value("server").toString(), settings.value("serverport").toUInt());

                if (!settings.value("login").toString().isEmpty())
                    ftp_->login(QUrl::fromPercentEncoding(settings.value("login").toString().toLatin1()), settings.value("password").toString());
                else
                    ftp_->login();
                if (!settings.value("directory").toString().isEmpty())
                    ftp_->cd(settings.value("directory").toString());
            }
        }
        if (ftp_ && ((ftp_state_ == QFtp::Connected) || (ftp_state_ == QFtp::LoggedIn)))
        {
            QByteArray ba;
            QBuffer buffer(&ba);
            buffer.open(QIODevice::WriteOnly);
            qImage_.save(&buffer, "JPG");
            qDebug() << "FTP put in " << snapshot_file;
            ftp_->put(ba, snapshot_file);
        }
#endif
        settings.endGroup();
    }

    // send mail
    if (checkBox_mail->isChecked())
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
    }
}

void QMotion::FTPstateChanged(int state)
{
    qDebug() << "FTP state" << state;
    ftp_state_ = state;
#if 0
    if ((state == QFtp::Closing) || (state == QFtp::Unconnected	))
    {
        ftp_->deleteLater();
        ftp_ = NULL;
    }
#endif
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
        motionDetector_.set_motion_color(color);
    }
}

void QMotion::resizeEvent( QResizeEvent * event )
{
    QWidget::resizeEvent(event);
    if ((video_size_.isValid()) && (video_size_.width() != 0))
    {
        int w = video_size_.width();
        int h = video_size_.height();
        label_video->setMinimumHeight(h * label_video->width() / w);
        label_video->setMaximumHeight(h * label_video->width() / w);
        label_motion->setMinimumHeight(h * label_motion->width() / w);
        label_motion->setMaximumHeight(h * label_motion->width() / w);
    }
}

void QMotion::on_global_marker_stateChanged(int i)
{
    switch (i) {
    case (Qt::Checked):
        motionDetector_.show_global_ = true;
        break;
    case (Qt::Unchecked):
    default:
        motionDetector_.show_global_ = false;
    }
}

void QMotion::on_component_markers_stateChanged(int i)
{
    switch (i) {
    case (Qt::Checked):
        motionDetector_.show_component_ = true;
        break;
    case (Qt::Unchecked):
    default:
        motionDetector_.show_component_ = false;
    }
}

void QMotion::mail(const QString & f)
{
    const QString server("smtp.neuf.fr");
    const QString from("maison@neuf.fr");
    QStringList tolist;
    tolist << "stephane.list@gmail.com";
    const QString subject("Détection de mouvement");
    const QString body("Ci-joint la photo prise par le système d'alarme !\r\n");

    MailSender m(server, from, tolist, subject, body);
    QStringList attachfiles;
    attachfiles << f;
    m.setAttachments(attachfiles);
    if (m.send())
        qDebug() << "Send OK";
    else
        qDebug() << "Send FAILED";
    QFile(f).remove();

}

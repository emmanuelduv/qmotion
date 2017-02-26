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

#ifndef CAPTURETHREAD_H
#define CAPTURETHREAD_H

#include <QThread>
#include <QImage>

#include "motiondetector.h"

#include <cv.h>
#include <opencv2/videoio/videoio.hpp>

#include <QNetworkAccessManager>
#include <QUrl>

class CaptureThread : public QThread
{
    Q_OBJECT
public:
    CaptureThread(QObject *parent = 0);
    ~CaptureThread();

public slots:
    void motion_treatment();

protected:
    void run();
    void timerEvent(QTimerEvent* event);
    void add_timestamp(QImage& img);

private:
    cv::VideoCapture capture_;
    cv::Mat image_;
    QImage qImage_;
    MotionDetector motionDetector_;
    QSettings settings;
    QNetworkAccessManager* ftp_;
    QUrl ftpUrl_;

signals:
    void output(const cv::Mat& );
    void motionOutput(const QImage& image, const QImage& motion);
    void webcamError(const QString &);
    void acquired();
    void abort();
    void newSize(QSize size);
};

#endif


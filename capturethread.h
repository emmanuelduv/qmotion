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

#include <cv.h>
#include <highgui.h>
#include <opencv2/videoio/videoio.hpp>

class CaptureThread : public QThread
{
    Q_OBJECT
public:
    CaptureThread(QObject *parent = 0);
    ~CaptureThread();

public slots:
    void set_fps_limit(int);

protected:
    void run();

private:
    void capture();
    bool abort_;
    cv::VideoCapture capture_;
    cv::Mat image_;
    QImage qImage_;
    int fps_limit_;

signals:
    void output(const cv::Mat& );
    void webcamError(const QString &);
};

#endif


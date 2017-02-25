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

CaptureThread::CaptureThread(QObject *parent)
    : QThread(parent),
      abort_(false),
      fps_limit_(25)
{
    qDebug() << "CaptureThread::CaptureThread";
    if (!capture_.open(0))
    {
        qWarning("No webcam");
        emit webcamError(tr("No webcam"));
        return;
    }
    capture_.set(cv::CAP_PROP_FRAME_WIDTH, 10000);
    capture_.set(cv::CAP_PROP_FRAME_HEIGHT, 10000);
}


CaptureThread::~CaptureThread()
{
    qDebug() << "CaptureThread::~CaptureThread";
    abort_ = true;
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
    while (!abort_)
    {
        capture();
    }
}

void CaptureThread::capture()
{
    if ( !capture_.isOpened() )
    {
        qDebug() << "capture_ not initialised";
        emit webcamError(tr("No webcam"));
        return;
    }
    //qDebug() << "recording a frame - " << QTime::currentTime();
    capture_ >> image_;

    emit output(image_);
    msleep(1000/fps_limit_);
}

void CaptureThread::set_fps_limit(int limit)
{
    qDebug() << "Limit fps to : " << limit;
    fps_limit_ = limit;
}

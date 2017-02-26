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

#ifndef MOTION_DETECTOR_H
#define MOTION_DETECTOR_H

#include <QObject>
#include <QImage>
#include <QSettings>

#include <cv.h>

class MotionDetector : public QObject
{
    Q_OBJECT
public:
    MotionDetector(QObject *parent = 0);
    ~MotionDetector();

public slots:
    void input(const cv::Mat&);

private:
    void  update_mhi(const cv::Mat& img, int diff_threshold);
    QSettings settings;

    // temporary images
    cv::Mat last;
    cv::Mat mhi; // MHI
    cv::Mat orient; // orientation
    cv::Mat mask; // valid orientation mask
    cv::Mat segmask; // motion segmentation map

signals:
    void output(const QImage& image, const QImage& motion);
    void error(const cv::Mat &);
    void motion();
    void newSize(QSize size);
};
#endif


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
#include <QColor>

#include <cv.h>

class MotionDetector : public QObject
{
    Q_OBJECT
public:
    MotionDetector(QObject *parent = 0);
    ~MotionDetector();
    bool show_global_;
    bool show_component_;

public slots:
    void set_motion_color(const QColor &);
    void input(const cv::Mat&);
    void set_threshold(int);

private:
    int threshold_;
    void  update_mhi(const cv::Mat& img, cv::Mat &dst, int diff_threshold);
    cv::Mat motion_;
    QColor color_;
    int last;
    bool init;

    // ring image buffer
    QVector<cv::Mat> buf;

    // temporary images
    cv::Mat mhi; // MHI
    cv::Mat orient; // orientation
    cv::Mat mask; // valid orientation mask
    cv::Mat segmask; // motion segmentation map
    std::vector<cv::Rect> storage; // temporary storage

signals:
    void output(const cv::Mat &);
    void error(const cv::Mat &);
    void motion();
};
#endif


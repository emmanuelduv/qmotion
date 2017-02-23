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
#include <highgui.h>

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
    void input(const IplImage &);
    void set_threshold(int);

private:
    int threshold_;
    void  update_mhi(const IplImage* img, IplImage* dst, int diff_threshold);
    IplImage *motion_;
    QColor color_;
    int last;

    // ring image buffer
    IplImage **buf;

    // temporary images
    IplImage *mhi; // MHI
    IplImage *orient; // orientation
    IplImage *mask; // valid orientation mask
    IplImage *red;
    IplImage *green;
    IplImage *blue;
    IplImage *segmask; // motion segmentation map
    CvMemStorage* storage; // temporary storage

signals:
    void output(const IplImage &);
    void error(const QString &);
    void motion();
};
#endif


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

#include "motiondetector.h"
#include "formatconverter.h"

#include <QDebug>
#include <QTime>
#include <QColor>

#include <ctime>
#include <opencv2/optflow/motempl.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core/core.hpp>

#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>

// various tracking parameters (in seconds)
const double MHI_DURATION = 1;
const double MAX_TIME_DELTA = 0.5;
const double MIN_TIME_DELTA = 0.05;
// number of cyclic frame buffer used for motion detection
// (should, probably, depend on FPS)
const int MAGNITUDE_GLOBAL = 100;
const int MAGNITUDE_COMPONENT = 30;

MotionDetector::MotionDetector(QObject *parent)
    : QObject(parent)
{
    qDebug() << "MotionDetector::MotionDetector";
}


MotionDetector::~MotionDetector()
{
    qDebug() << "MotionDetector::~MotionDetector";
}


void MotionDetector::input(const cv::Mat &img)
{
    bool show = !settings.value("display",0).toBool();
    double timestamp = (double)clock()/CLOCKS_PER_SEC; // get current time in seconds
    cv::Mat actual;
    cv::Mat dst = cv::Mat::zeros(img.size(), img.type());
    cv::Mat silh;
    cv::Rect comp_rect;
    double count, angle, magnitude;
    cv::Point center;
    cv::Scalar color;

    cv::cvtColor(img, actual, cv::COLOR_BGR2GRAY/*CV_BGR2GRAY*/); // convert frame to grayscale
    // allocate images at the beginning or
    // reallocate them if the frame size is changed
    if(mhi.size[0] == 0 || mhi.size[0] != img.size[0] || mhi.size[1] != img.size[1]) {
        last = actual;
        mhi = cv::Mat(img.size[0], img.size[1], CV_32FC1, 0.);
        orient = cv::Mat(img.size[0], img.size[1], CV_32FC1, 0.);
        segmask = cv::Mat(img.size[0], img.size[1], CV_32FC1, 0.);
        mask = cv::Mat(img.size[0], img.size[1], /*IPL_DEPTH_8U*/CV_8UC1, 0.);
        QSize size;
        size.setWidth(img.size[1]);
        size.setHeight(img.size[0]);
        emit this->newSize(size);
    }

    cv::absdiff(actual, last, silh); // get difference between frames

    last = actual;

    cv::threshold(silh, silh, settings.value("detection_threshold", 30).toInt(), 1, cv::THRESH_BINARY); // and threshold it

    cv::motempl::updateMotionHistory(silh, mhi, timestamp, MHI_DURATION); // update MHI

    // convert MHI to blue 8u image
    mask = mhi * (255. / MHI_DURATION) + ((MHI_DURATION - timestamp) * 255. / MHI_DURATION);
    mask.convertTo(mask, /*IPL_DEPTH_8U*/CV_8UC1);

    QColor color_ = QColor(settings.value("color", "blue").toString());
    if(show){
        // convert 8u image to 32u image using custom color
        cv::cvtColor(mask, dst, cv::COLOR_GRAY2RGB);
        dst = dst.mul(cv::Scalar(color_.red(), color_.green(), color_.blue()));
    }

    // calculate motion gradient orientation and valid orientation mask
    cv::motempl::calcMotionGradient(mhi, mask, orient, MAX_TIME_DELTA, MIN_TIME_DELTA, 3);

    // segment motion: get sequence of motion components
    // segmask is marked motion components map. It is not used further
    std::vector<cv::Rect> storage;
    cv::motempl::segmentMotion(mhi, segmask, storage, timestamp, MAX_TIME_DELTA);

    // iterate through the motion components,
    // One more iteration (i == -1) corresponds to the whole image (global motion)
    for(int i = -1; i != storage.size() - 1; i++){
        if(i < 0) { // case of the whole image
            comp_rect = cv::Rect(0, 0, img.size[1], img.size[0]);
            color = cv::Scalar(255,255,255); // white
            magnitude = MAGNITUDE_GLOBAL;
        }
        else { // i-th motion component
            comp_rect = storage.at(i);
            if( comp_rect.width + comp_rect.height < 100 ) // reject very small components
                continue;
            color = cv::Scalar(color_.blue(), color_.green(), color_.red());
            magnitude = MAGNITUDE_COMPONENT;
        }

        // select component ROI
        cv::Mat silh2 = silh(comp_rect);
        cv::Mat mhi2 = mhi(comp_rect);
        cv::Mat orient2 = orient(comp_rect);
        cv::Mat mask2 = mask(comp_rect);

        // calculate orientation
        angle = cv::motempl::calcGlobalOrientation(orient2, mask2, mhi2, timestamp, MHI_DURATION);
        angle = 360.0 - angle;  // adjust for images with top-left origin

        count = cv::norm(silh2, cv::NORM_L1); // calculate number of points within silhouette ROI

        // check for the case of little motion
        if(count < comp_rect.width * comp_rect.height * 0.03)
            continue;

        if (i == -1) {
            emit motion();
            if(!show)
                break;
        }

        if(
            ((magnitude == MAGNITUDE_GLOBAL) && settings.value("show_global", 1).toBool())
            || ((magnitude == MAGNITUDE_COMPONENT) && settings.value("show_component", 1).toBool())
        ){
            // draw a clock with arrow indicating the direction
            center = cv::Point(comp_rect.x + comp_rect.width / 2,comp_rect.y + comp_rect.height / 2);
            magnitude = (magnitude * img.size[1]) / (3 * MAGNITUDE_GLOBAL);

            cv::circle(
                dst,
                center,
                cvRound(magnitude * 1.2),
                color,
                3,
                /*CV_AA*/cv::LINE_AA,
                0
            );
            cv::line(
                dst,
                center,
                cv::Point(
                    cvRound(center.x + magnitude * cos(angle * CV_PI / 180)),
                    cvRound(center.y - magnitude * sin(angle * CV_PI / 180))
                ),
                color,
                3,
                /*CV_AA*/cv::LINE_AA,
                0
            );
        }
    }

    if(show)
        emit output(cvMatToQImage(img, settings.value("flip",0).toBool()), cvMatToQImage(dst, settings.value("flip",0).toBool()));
}

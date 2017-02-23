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

#include <QDebug>
#include <QTime>

// various tracking parameters (in seconds)
const double MHI_DURATION = 1;
const double MAX_TIME_DELTA = 0.5;
const double MIN_TIME_DELTA = 0.05;
// number of cyclic frame buffer used for motion detection
// (should, probably, depend on FPS)
const int N = 4;
const int MAGNITUDE_GLOBAL = 100;
const int MAGNITUDE_COMPONENT = 30;

MotionDetector::MotionDetector(QObject *parent)
    : QObject(parent),
      show_global_(true),
      show_component_(true),
      motion_(0),
      last(0),
      buf(0),
      mhi(0),
      orient(0),
      mask(0),
      red(0),
      green(0),
      blue(0),
      segmask(0),
      storage(0)
{
    qDebug() << "MotionDetector::MotionDetector";
}


MotionDetector::~MotionDetector()
{
    qDebug() << "MotionDetector::~MotionDetector";
}


void MotionDetector::input(const IplImage & image)
{
    //qDebug() << "MotionDetector::input";
    if( !motion_ )
    {
        motion_ = cvCreateImage(cvSize(image.width,image.height), 8, 3 );
        cvZero(motion_);
        motion_->origin = image.origin;
    }

    update_mhi(&image, motion_, threshold_);

    //emit output(image);
    emit output(*motion_);

}

void MotionDetector::set_motion_color(const QColor & col)
{
    //qDebug() << "color set to : " << col;
    color_ = col;
};

void MotionDetector::set_threshold(int t)
{
    //qDebug() << "threshold set to : " << t;
    threshold_ = t;
}

// parameters:
//  img - input video frame
//  dst - resultant motion picture
//  diff_threshold - threshold to avoid motion detection because of noise in the video
void MotionDetector::update_mhi(const IplImage* img, IplImage* dst, int diff_threshold )
{
    double timestamp = (double)clock()/CLOCKS_PER_SEC; // get current time in seconds
    CvSize size = cvSize(img->width,img->height); // get current frame size
    int i, idx1 = last, idx2;
    IplImage* silh;
    CvSeq* seq;
    CvRect comp_rect;
    double count, angle, magnitude;
    CvPoint center;
    CvScalar color;

    //qDebug() << "MotionDetector::update_mhi";

    // allocate images at the beginning or
    // reallocate them if the frame size is changed
    if( !mhi || mhi->width != size.width || mhi->height != size.height ) {
        if( buf == 0 ) {
            buf = (IplImage**)malloc(N*sizeof(buf[0]));
            memset( buf, 0, N*sizeof(buf[0]));
        }
        for( i = 0; i < N; i++ ) {
            cvReleaseImage( &buf[i] );
            buf[i] = cvCreateImage( size, IPL_DEPTH_8U, 1 );
            cvZero( buf[i] );
        }
        cvReleaseImage( &mhi );
        cvReleaseImage( &orient );
        cvReleaseImage( &segmask );
        cvReleaseImage( &mask );
        cvReleaseImage( &red );
        cvReleaseImage( &green );
        cvReleaseImage( &blue );

        mhi = cvCreateImage( size, IPL_DEPTH_32F, 1 );
        cvZero( mhi ); // clear MHI at the beginning
        orient = cvCreateImage( size, IPL_DEPTH_32F, 1 );
        segmask = cvCreateImage( size, IPL_DEPTH_32F, 1 );
        mask = cvCreateImage( size, IPL_DEPTH_8U, 1 );
        red = cvCreateImage( size, IPL_DEPTH_8U, 1 );
        green = cvCreateImage( size, IPL_DEPTH_8U, 1 );
        blue = cvCreateImage( size, IPL_DEPTH_8U, 1 );
    }

    cvCvtColor( img, buf[last], CV_BGR2GRAY ); // convert frame to grayscale

    //emit output(*buf[last]);

    idx2 = (last + 1) % N; // index of (last - (N-1))th frame
    last = idx2;

    silh = buf[idx2];
    cvAbsDiff( buf[idx1], buf[idx2], silh ); // get difference between frames

    //emit output(*silh);

    cvThreshold( silh, silh, diff_threshold, 1, CV_THRESH_BINARY ); // and threshold it

    //emit output(*silh);

    cvUpdateMotionHistory( silh, mhi, timestamp, MHI_DURATION ); // update MHI

    // convert MHI to blue 8u image
    cvCvtScale( mhi, mask, 255./MHI_DURATION, (MHI_DURATION - timestamp)*255./MHI_DURATION );

    // convert 8u image to 32u image using custom color
    cvCvtScale( mask, red,   color_.red() / 255.0, 0.);
    cvCvtScale( mask, green, color_.green() / 255.0, 0.);
    cvCvtScale( mask, blue,  color_.blue() / 255.0, 0.);

    cvZero( dst );
    cvCvtPlaneToPix( blue, green,  red, 0, dst );

    // calculate motion gradient orientation and valid orientation mask
    cvCalcMotionGradient( mhi, mask, orient, MAX_TIME_DELTA, MIN_TIME_DELTA, 3 );

    if( !storage )
        storage = cvCreateMemStorage(0);
    else
        cvClearMemStorage(storage);

    // segment motion: get sequence of motion components
    // segmask is marked motion components map. It is not used further
    seq = cvSegmentMotion( mhi, segmask, storage, timestamp, MAX_TIME_DELTA );

    // iterate through the motion components,
    // One more iteration (i == -1) corresponds to the whole image (global motion)
    for( i = -1; i < seq->total; i++ ) {

        if( i < 0 ) { // case of the whole image
            comp_rect = cvRect( 0, 0, size.width, size.height );
            color = CV_RGB(255,255,255); // white
            magnitude = MAGNITUDE_GLOBAL;
        }
        else { // i-th motion component
            comp_rect = ((CvConnectedComp*)cvGetSeqElem( seq, i ))->rect;
            if( comp_rect.width + comp_rect.height < 100 ) // reject very small components
                continue;
            color = CV_RGB(255,0,0); // red
            magnitude = MAGNITUDE_COMPONENT;
        }

        // select component ROI
        cvSetImageROI( silh, comp_rect );
        cvSetImageROI( mhi, comp_rect );
        cvSetImageROI( orient, comp_rect );
        cvSetImageROI( mask, comp_rect );

        // calculate orientation
        angle = cvCalcGlobalOrientation( orient, mask, mhi, timestamp, MHI_DURATION);
        angle = 360.0 - angle;  // adjust for images with top-left origin

        count = cvNorm( silh, 0, CV_L1, 0 ); // calculate number of points within silhouette ROI

        cvResetImageROI( mhi );
        cvResetImageROI( orient );
        cvResetImageROI( mask );
        cvResetImageROI( silh );

        // check for the case of little motion
        if( count < comp_rect.width*comp_rect.height * 0.05 )
            continue;

        if (((magnitude == MAGNITUDE_GLOBAL) && show_global_)
                || ((magnitude == MAGNITUDE_COMPONENT) && show_component_))
        {
            // draw a clock with arrow indicating the direction
            center = cvPoint(comp_rect.x + comp_rect.width / 2,comp_rect.y + comp_rect.height / 2);

            magnitude = (magnitude * size.height) / (3 * MAGNITUDE_GLOBAL);

            cvCircle(dst,center,cvRound(magnitude * 1.2),color,3,CV_AA,0);
            cvLine(dst,center
                   ,cvPoint(cvRound(center.x + magnitude * cos(angle * CV_PI / 180))
                            ,cvRound(center.y - magnitude * sin(angle * CV_PI / 180)))
                   ,color,3,CV_AA,0);
        }
        if (i == -1)
        {
            //qDebug() << "emit Motion Detected !";
            emit motion();
        }
    }
}


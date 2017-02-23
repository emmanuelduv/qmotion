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

#include "formatconverter.h"

QImage Ipl2QImage(const IplImage* newImage,bool upsideDown)
{
    if ((0 != newImage) && (cvGetSize(newImage).width > 0))
    {
        const char* src = newImage->imageData;
        QImage qtemp(newImage->width,newImage->height,QImage::Format_RGB32);

        if (upsideDown)
        {
            for (int y = 0; y < newImage->height; ++y, src += newImage->widthStep)
            {
                uint* dst = reinterpret_cast<uint*>(qtemp.scanLine (newImage->height - y - 1));
                for (int x = 0; x < newImage->width; ++x, ++dst)
                {
                    *dst = qRgb(src[x * newImage->nChannels + 2]
                            ,src[x * newImage->nChannels + 1]
                            ,src[x * newImage->nChannels]);
                }
            }
        }
        else
        {
            for (int y = 0; y < newImage->height; ++y, src += newImage->widthStep)
            {
                uint* dst = reinterpret_cast<uint*>(qtemp.scanLine(y));
                for (int x = 0; x < newImage->width; ++x, ++dst)
                {
                    *dst = qRgb(src[x * newImage->nChannels + 2]
                            ,src[x * newImage->nChannels + 1]
                            ,src[x * newImage->nChannels]);
                }
            }
        }
        return qtemp;
    }
    else
        return QImage();
}
/* TODO
IplImage QImage2Ipl(const QImage & img, bool upsideDown)
{
    IplImage i;
    return i;
}
*/

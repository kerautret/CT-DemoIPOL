#include "imagegradient.h"

#include <cmath>
#include <float.h>


ImageGradient::ImageGradient(QImage &image) :
    source (image),
    width (image.width ()),
    height (image.height ()),
//    gradient (new float[width * height]),
    widthMinus1 (width - 1),
    heightMinus1 (height - 1)
//,    maximalValue (0.0)
{
//    initialize ();
//    float norm;
    /*unsigned char p1, p2, p3, p4, p6, p7, p8, p9;

    bool x0, y0, xWidth, yHeight;

    short sumX, sumY;
    for (short x = 0; x < width; x++)
        for (short y = 0; y < height; y++){
            x0 = (x > 0);
            y0 = (y > 0);
            xWidth = (x < widthMinus1);
            yHeight = (y < heightMinus1);

            if (x0 && y0)
                p1 = source.pixel (x - 1, y - 1);
            else
                p1 = 0;
            if (y0)
                p2 = source.pixel (x, y - 1);
            else
                p2 = 0;
            if (xWidth && y0)
                p3 = source.pixel (x + 1, y - 1);
            else
                p3 = 0;
            if (x0)
                p4 = source.pixel (x - 1, y);
            else
                p4 = 0;
            if (xWidth)
                p6 = source.pixel (x + 1, y);
            else
                p6 = 0;
            if (x0 && yHeight)
                p7 = source.pixel (x - 1, y + 1);
            else
                p7 = 0;
            if (yHeight)
                p8 = source.pixel (x, y + 1);
            else
                p8 = 0;
            if (xWidth && yHeight)
                p9 = source.pixel (x + 1, y + 1);
            else
                p9 = 0;

            sumX = -p1 + p3 - 2 * p4 + 2 * p6 - p7 + p9;
            sumY = p1 + 2 * p2 + p3 - p7 - 2 * p8 - p9;

//            norm = sqrt (sumX * sumX + sumY * sumY);
//            gradient[x + y * width] = norm;
//            if (maximalValue < norm)
//                maximalValue = norm;

            gradient[x + y * width] = sqrt (sumX * sumX + sumY * sumY);
        }*/

//    normalize ();
}


//ImageGradient::~ImageGradient (){
//    delete []gradient;
//}


float ImageGradient::GetValue(unsigned short x, unsigned short y){
//    float *currentGradient = gradient + x + y * width;
//    if (*currentGradient > FLT_MIN)
//        return *currentGradient;
    bool x0 = (x > 0);
    bool y0 = (y > 0);
    bool xWidth = (x < widthMinus1);
    bool yHeight = (y < heightMinus1);

    unsigned char p1 = ((x0 && y0) ? source.pixel (x - 1, y - 1) : 0);
    unsigned char p2 = ((y0) ? source.pixel (x, y - 1) : 0);
    unsigned char p3 = ((xWidth && y0) ? source.pixel (x + 1, y - 1) : 0);
    unsigned char p4 = ((x0) ? source.pixel (x - 1, y) : 0);
    unsigned char p6 = ((xWidth) ? source.pixel (x + 1, y) : 0);
    unsigned char p7 = ((x0 && yHeight) ? source.pixel (x - 1, y + 1) : 0);
    unsigned char p8 = ((yHeight) ? source.pixel (x, y + 1) : 0);
    unsigned char p9 = ((xWidth && yHeight) ? source.pixel (x + 1, y + 1) : 0);

    short sumX = -p1 + p3 - 2 * p4 + 2 * p6 - p7 + p9;
    short sumY = p1 + 2 * p2 + p3 - p7 - 2 * p8 - p9;

//    *currentGradient = sqrt (sumX * sumX + sumY * sumY);
//    return *currentGradient;

    return sqrt (sumX * sumX + sumY * sumY);
}


//void ImageGradient::initialize (){
//    float *currentGradient = gradient;
//    float *endGradient = currentGradient + width * height;
//    while (currentGradient < endGradient){
//        *currentGradient = FLT_MIN;
//        currentGradient++;
//    }
//}


//void ImageGradient::normalize (){
//    float *currentGradient = gradient;
//    float *endGradient = currentGradient + width * height;
//    while (currentGradient < endGradient){
//        *currentGradient /= maximalValue;
//        currentGradient++;
//    }
//    maximalValue = 1.0;
//}

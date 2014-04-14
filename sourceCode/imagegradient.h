#ifndef IMAGEGRADIENT_H
#define IMAGEGRADIENT_H

#include <QImage>

class ImageGradient{
public:
    ImageGradient (QImage &image);
//    ~ImageGradient ();

//    inline float GetValue (unsigned short x, unsigned short y){
//        return gradient[x + y * width];
//    }
    float GetValue (unsigned short x, unsigned short y);

protected:
    void initialize ();
//    void normalize ();

    QImage &source;
    unsigned short width;
    unsigned short height;

//    float *gradient;

    unsigned short widthMinus1;
    unsigned short heightMinus1;

//    float maximalValue;
};

#endif // IMAGEGRADIENT_H

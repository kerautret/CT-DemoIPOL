#ifndef SEGMENTATIONALPHAPARAMETERFINDER_H
#define SEGMENTATIONALPHAPARAMETERFINDER_H

#include "imagegradient.h"
#include "segmentationbasedoncomponenttree.h"

#include "vector"
#include "include/Point.h"


class SegmentationAlphaParameterFinder{
public:
    SegmentationAlphaParameterFinder (SegmentationBasedOnComponentTree &algorithm, QImage &input);
    float FindAlpha (float alphaStep, bool useMeaningfulScales);
    float FindAlphaByBisection (unsigned char numberOfBisections, bool useMeaningfulScales);

protected:
    float weightedMaxOfContoursNeighbourhoodGradient (std::vector<std::vector<LibTIM::Point<LibTIM::U8> > > *contours, unsigned int maxContourLength, unsigned int &totalNumberOfContoursPixels);
    void calculate (unsigned char numberOfBisections, bool &useMeaningfulScales, float &alpha, float &criteriumValue, float alphaLeft, unsigned int pixelsFoundedNumberLeft, float criteriumValueLeft, float alphaRight, unsigned int pixelsFoundedNumberRight, float criteriumValueRight);

    SegmentationBasedOnComponentTree &segmentationAlgorithm;
    unsigned int indicatorId;
    ImageGradient gradient;
    unsigned short width;
    unsigned short height;
};

#endif // SEGMENTATIONALPHAPARAMETERFINDER_H

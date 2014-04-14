#ifndef MEANINGFULSCALESALPHAADJUST_H
#define MEANINGFULSCALESALPHAADJUST_H

#include "imagegradient.h"
#include "segmentationbasedoncomponenttree.h"

#include "vector"
#include "include/Point.h"


class MeaningfulScalesAlphaAdjust{
public:
    MeaningfulScalesAlphaAdjust (SegmentationBasedOnComponentTree &algorithm, unsigned int indicator, QImage &input);
    float FindAlpha (float alphaStep, int samplingSizeMax);

protected:
    float sumOfContoursWithGradientNeighbourhood (std::vector<LibTIM::Point<LibTIM::U8> > *contoursPoints);
    float maxOfContoursWithGradientNeighbourhood (std::vector<LibTIM::Point<LibTIM::U8> > *contoursPoints);
    float weightedMaxOfContoursWithGradientNeighbourhood (std::vector<std::vector<LibTIM::Point<LibTIM::U8> > > *contours, unsigned int maxContourLength);
    float weightedAverageOfContoursWithGradientNeighbourhood (std::vector<std::vector<LibTIM::Point<LibTIM::U8> > > *contours, unsigned int maxContourLength);

    float indicator (unsigned int &freemanChainSize, unsigned int &totalNoise, unsigned int &resultArea, float standardDeviation, unsigned int totalContourLength, float contoursNeighbourhoodSum, float contoursNeighbourhoodMax, float contoursNeighbourhoodWeightedMax, float contoursNeighbourhoodWeightedAverage, unsigned int numberOfMeaningfulPixels);

    SegmentationBasedOnComponentTree &segmentationAlgorithm;
    unsigned int indicatorId;
    ImageGradient gradient;
    unsigned short width;
    unsigned short height;
};

#endif // MEANINGFULSCALESALPHAADJUST_H

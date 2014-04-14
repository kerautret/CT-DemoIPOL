#include "meaningfulscalesalphaadjust.h"
#include "meaningfulscales.h"
#include "float.h"
#include "timer.h"

#include <QDebug>

#include "include/Image.h"

Timer timers[10];

#define NUMBER_OF_CRITERIA 33


MeaningfulScalesAlphaAdjust::MeaningfulScalesAlphaAdjust (SegmentationBasedOnComponentTree &algorithm, unsigned int indicator, QImage &input) :
    segmentationAlgorithm (algorithm),
    indicatorId (indicator),
    gradient (ImageGradient (input)),
    width (algorithm.GetInputWidth ()),
    height (algorithm.GetInputHeight ())
{
}


/**
  @todo remove from MeaningfulScales translation image to PGM (needed for Freeman Chain)
  */
float MeaningfulScalesAlphaAdjust::FindAlpha (float alphaStep, int samplingSizeMax = 7){
    //cerr.rdbuf(0);
    unsigned char indicatorIdCopy = indicatorId;

    float alpha = 0.0;
    segmentationAlgorithm.DoSegmentationWithPreviousComponentTree (alpha, timers);
    unsigned int previousIterationPixelsFoundedNumber;
    unsigned int pixelsFoundedNumber = segmentationAlgorithm.FoundedPixelNumber ();

    MeaningfulScales ms (*segmentationAlgorithm.GetResultImage ());
    ms.samplingSizeMax = samplingSizeMax;
    unsigned int freemanChainSize = ms.GetFreemanChainSize ();
    unsigned int sumOfNoise;
    float theBestIndicatorValue[NUMBER_OF_CRITERIA];
    float theBestAlpha[NUMBER_OF_CRITERIA];
    for (unsigned char i = 0; i < NUMBER_OF_CRITERIA; i++)
        theBestAlpha[i] = alpha;
    if (freemanChainSize > MeaningfulScales::MinimalFreemanChainSize){
        sumOfNoise = ms.GetSumOfNoise ();
        float standardDeviation = ms.GetNoiseStanardDeviation ();
        float contoursNeighbourhoodSum = 0;//sumOfContoursWithGradientNeighbourhood (&(ms.contourPixels));
        float contoursNeighbourhoodMax = 0;//maxOfContoursWithGradientNeighbourhood (&(ms.contourPixels));
        float contoursNeighbourhoodWeightedMax = weightedMaxOfContoursWithGradientNeighbourhood (&(ms.contours), freemanChainSize);
        float contoursNeighbourhoodWeightedAverage = weightedAverageOfContoursWithGradientNeighbourhood (&(ms.contours), freemanChainSize);
        unsigned int numberOfMeaningfulPixels = ms.GetNumberOfMeaningfulPixels ();
        unsigned int totalNumberOfContoursPixels = 0;
        vector<std::vector<LibTIM::Point<LibTIM::U8> > >::const_iterator contoursEnd = ms.contours.end ();
        for (vector<std::vector<LibTIM::Point<LibTIM::U8> > >::iterator c = ms.contours.begin (); c != contoursEnd; ++c)
            totalNumberOfContoursPixels += c -> size ();

        for (unsigned char i = 0; i < NUMBER_OF_CRITERIA; i++){
            indicatorId = i + 1;
            float indicatorValue = indicator (freemanChainSize, sumOfNoise, pixelsFoundedNumber, standardDeviation, totalNumberOfContoursPixels, contoursNeighbourhoodSum, contoursNeighbourhoodMax, contoursNeighbourhoodWeightedMax, contoursNeighbourhoodWeightedAverage, numberOfMeaningfulPixels);
            theBestIndicatorValue[i] = indicatorValue;
        }

        qDebug () << alpha << freemanChainSize << sumOfNoise << pixelsFoundedNumber << standardDeviation << totalNumberOfContoursPixels << contoursNeighbourhoodSum << contoursNeighbourhoodMax << contoursNeighbourhoodWeightedMax << contoursNeighbourhoodWeightedAverage << numberOfMeaningfulPixels;
    } else{
        sumOfNoise = ms.GetSumOfNoise ();
        for (unsigned char i = 0; i < NUMBER_OF_CRITERIA; i++)
            theBestIndicatorValue[i] = DBL_MAX;
    }

    /*
    //Comparing results of segmentation; as we agreed while Monday's meeting (7.05.2012), |R_{alpha_1}| < |R_{alpha_2}| <=> alpha_1 > alpha_2
    LibTIM::Image<U8> *firstSegmentationResult = segmentationAlgorithm.GetResultImage ();
    LibTIM::Image<U8> *secondSegmentationResult;
    bool booleanSwitch = true;
    */

    alpha += alphaStep;
    while (alpha <= 1.0){
        previousIterationPixelsFoundedNumber = pixelsFoundedNumber;
        segmentationAlgorithm.DoSegmentationWithPreviousComponentTree (alpha, timers);
        pixelsFoundedNumber = segmentationAlgorithm.FoundedPixelNumber ();

        /*
          //Comparing results of segmentation; as we agreed while Monday's meeting (7.05.2012), |R_{alpha_1}| < |R_{alpha_2}| <=> alpha_1 > alpha_2
        if (booleanSwitch)
            secondSegmentationResult = segmentationAlgorithm.GetResultImage ();
        else
            firstSegmentationResult = segmentationAlgorithm.GetResultImage ();
        booleanSwitch = !booleanSwitch;
        if ((LibTIM::Image<U8>)(*firstSegmentationResult) != (LibTIM::Image<U8>)(*secondSegmentationResult)){*/
        if (previousIterationPixelsFoundedNumber != pixelsFoundedNumber){
            MeaningfulScales ms2 (*segmentationAlgorithm.GetResultImage());
            freemanChainSize = ms2.GetFreemanChainSize ();

            if (freemanChainSize > MeaningfulScales::MinimalFreemanChainSize){
                sumOfNoise = ms2.GetSumOfNoise ();
                float standardDeviation = ms2.GetNoiseStanardDeviation ();
                float contoursNeighbourhoodSum = 0;//sumOfContoursWithGradientNeighbourhood (&(ms2.contourPixels));
                float contoursNeighbourhoodMax = 0;//maxOfContoursWithGradientNeighbourhood (&(ms2.contourPixels));
                float contoursNeighbourhoodWeightedMax = weightedMaxOfContoursWithGradientNeighbourhood (&(ms2.contours), freemanChainSize);
                float contoursNeighbourhoodWeightedAverage = weightedAverageOfContoursWithGradientNeighbourhood (&(ms2.contours), freemanChainSize);
                unsigned int numberOfMeaningfulPixels = ms2.GetNumberOfMeaningfulPixels ();
                unsigned int totalNumberOfContoursPixels = 0;
                vector<std::vector<LibTIM::Point<LibTIM::U8> > >::const_iterator contoursEnd = ms2.contours.end ();
                for (vector<std::vector<LibTIM::Point<LibTIM::U8> > >::iterator c = ms2.contours.begin (); c != contoursEnd; ++c)
                    totalNumberOfContoursPixels += c -> size ();

                //cout << alpha << "\t" << freemanChainSize << "\t" << sumOfNoise << "\t" << pixelsFoundedNumber << "\t" << indicatorValue << endl;
                for (unsigned char i = 0; i < NUMBER_OF_CRITERIA; i++){
                    indicatorId = i + 1;
                    float indicatorValue = indicator (freemanChainSize, sumOfNoise, pixelsFoundedNumber, standardDeviation, totalNumberOfContoursPixels, contoursNeighbourhoodSum, contoursNeighbourhoodMax, contoursNeighbourhoodWeightedMax, contoursNeighbourhoodWeightedAverage, numberOfMeaningfulPixels);
                    if (indicatorValue < theBestIndicatorValue[i]){
                        theBestIndicatorValue[i] = indicatorValue;
                        theBestAlpha[i] = alpha;
                    }
                }

                qDebug () << alpha << freemanChainSize << sumOfNoise << pixelsFoundedNumber << standardDeviation << totalNumberOfContoursPixels << contoursNeighbourhoodSum << contoursNeighbourhoodMax << contoursNeighbourhoodWeightedMax << contoursNeighbourhoodWeightedAverage << numberOfMeaningfulPixels;
            } else{
            //    qDebug () << alpha << "Too short Freeman chain." << freemanChainSize << "<" << MeaningfulScales::MinimalFreemanChainSize;
            }
        }

        alpha += alphaStep;
    }

    for (unsigned char i = 0; i < NUMBER_OF_CRITERIA; i++)
        qDebug () << i + 1 << theBestAlpha[i] << theBestIndicatorValue[i];

    indicatorId = indicatorIdCopy;
    return theBestAlpha[indicatorId - 1];
}


float MeaningfulScalesAlphaAdjust::sumOfContoursWithGradientNeighbourhood(vector<LibTIM::Point<LibTIM::U8> > *contoursPoints){
    float result = 0.0;
    vector<LibTIM::Point<LibTIM::U8> >::const_iterator end = contoursPoints -> end ();
    for (vector<LibTIM::Point<LibTIM::U8> >::iterator i = contoursPoints -> begin (); i != end; ++i)
        for (int x = -1; x <= 1; x++)
            for (int y = -1; y <= + 1; y++){
                x += (*i).x;
                y += (*i).y;
                if (x < 0)
                    continue;
                if (x > width - 1)
                    continue;
                if (y < 0)
                    continue;
                if (y > height - 1)
                    continue;
                result += gradient.GetValue (x, y);
            }
    return result;
}


float MeaningfulScalesAlphaAdjust::maxOfContoursWithGradientNeighbourhood(vector<LibTIM::Point<LibTIM::U8> > *contoursPoints){
    float result = 0.0;
    float max, gradientValue;
    vector<LibTIM::Point<LibTIM::U8> >::const_iterator end = contoursPoints -> end ();
    for (vector<LibTIM::Point<LibTIM::U8> >::iterator i = contoursPoints -> begin (); i != end; ++i){
        max = 0.0;
        for (int x = -1; x <= 1; x++)
            for (int y = -1; y <= + 1; y++){
                x += (*i).x;
                y += (*i).y;
                if (x < 0)
                    continue;
                if (x > width - 1)
                    continue;
                if (y < 0)
                    continue;
                if (y > height - 1)
                    continue;
                gradientValue = gradient.GetValue (x, y);
                if (max < gradientValue)
                    max = gradientValue;
            }
        result += max;
    }
    return result;
}


float MeaningfulScalesAlphaAdjust::weightedMaxOfContoursWithGradientNeighbourhood (vector<std::vector<LibTIM::Point<LibTIM::U8> > > *contours, unsigned int maxContourLength){
    float result = 0.0;
    float max, gradientValue, contourValue;
    vector<std::vector<LibTIM::Point<LibTIM::U8> > >::const_iterator contoursEnd = contours -> end ();
    for (vector<std::vector<LibTIM::Point<LibTIM::U8> > >::iterator c = contours -> begin (); c != contoursEnd; ++c){
        vector<LibTIM::Point<LibTIM::U8> >::const_iterator end = c -> end ();
        contourValue = 0.0;
        for (vector<LibTIM::Point<LibTIM::U8> >::iterator p = c -> begin (); p != end; ++p){
            max = 0.0;
            for (int x = -1; x <= 1; x++)
                for (int y = -1; y <= + 1; y++){
                    x += (*p).x;
                    y += (*p).y;
                    if (x < 0)
                        continue;
                    if (x > width - 1)
                        continue;
                    if (y < 0)
                        continue;
                    if (y > height - 1)
                        continue;
                    gradientValue = gradient.GetValue (x, y);
                    if (max < gradientValue)
                        max = gradientValue;
                }
            contourValue += max;
        }
        result += contourValue / (maxContourLength - c -> size () + 1);
    }
    return result;
}

float MeaningfulScalesAlphaAdjust::weightedAverageOfContoursWithGradientNeighbourhood (vector<std::vector<LibTIM::Point<LibTIM::U8> > > *contours, unsigned int maxContourLength){
    float result = 0.0;
    float contourValue;
    vector<std::vector<LibTIM::Point<LibTIM::U8> > >::const_iterator contoursEnd = contours -> end ();
    for (vector<std::vector<LibTIM::Point<LibTIM::U8> > >::iterator c = contours -> begin (); c != contoursEnd; ++c){
        vector<LibTIM::Point<LibTIM::U8> >::const_iterator end = c -> end ();
        contourValue = 0.0;
        for (vector<LibTIM::Point<LibTIM::U8> >::iterator p = c -> begin (); p != end; ++p){
            for (int x = -1; x <= 1; x++)
                for (int y = -1; y <= + 1; y++){
                    x += (*p).x;
                    y += (*p).y;
                    if (x < 0)
                        continue;
                    if (x > width - 1)
                        continue;
                    if (y < 0)
                        continue;
                    if (y > height - 1)
                        continue;
                    contourValue += gradient.GetValue (x, y);
                }
        }
        contourValue /= c -> size () / 9.0;
        result += contourValue / (maxContourLength - c -> size () + 1);
    }
    return result;
}


float MeaningfulScalesAlphaAdjust::indicator (
        unsigned int &freemanChainSize/*B*/,
        unsigned int &totalNoise/*C*/,
        unsigned int &resultArea/*D*/,
        float standardDeviation/*E*/,
        unsigned int totalContourLength/*F*/,
        float contoursNeighbourhoodSum/*G*/,
        float contoursNeighbourhoodMax/*H*/,
        float contoursNeighbourhoodWeightedMax,/*I*/
        float contoursNeighbourhoodWeightedAverage,/*J*/
        unsigned int numberOfMeaningfulPixels/*K*/
        ){
    switch (indicatorId){
        case 23:
            return ((float)totalContourLength * standardDeviation) / (contoursNeighbourhoodWeightedMax * (float)resultArea);
        case 24:
            return ((float)totalContourLength * standardDeviation) / contoursNeighbourhoodWeightedMax;
        case 25:
            return ((float)totalContourLength) / contoursNeighbourhoodWeightedMax;
        case 26:
            return ((float)totalContourLength) / (contoursNeighbourhoodWeightedMax * (float)resultArea);
        case 27:
            return ((float)totalContourLength * (float)totalNoise) / (standardDeviation * (float)freemanChainSize * contoursNeighbourhoodWeightedMax);
        case 28:
            return ((float)totalContourLength * (float)totalNoise) / ((float)freemanChainSize * contoursNeighbourhoodWeightedMax);
        case 29:
            return (float)totalContourLength / contoursNeighbourhoodWeightedAverage;
        case 30:
            return ((float)totalContourLength * (float)totalNoise) / ((float)freemanChainSize * contoursNeighbourhoodWeightedAverage);
        case 31:
            return ((float)totalContourLength * (float)freemanChainSize) / ((float)totalNoise * contoursNeighbourhoodWeightedMax);
        case 32:
            return ((float)totalContourLength) / (contoursNeighbourhoodWeightedMax * (float)numberOfMeaningfulPixels / (float)freemanChainSize);
        case 33:
            return ((float)totalContourLength * (float)numberOfMeaningfulPixels) / (contoursNeighbourhoodWeightedMax * (float)freemanChainSize);
        case 1:
            return (float)totalNoise / (float)freemanChainSize;
        case 2://apple//BlueBottle//CLetterBetterMarker
            return (float)totalNoise / (float)resultArea;
        case 3://ContourS
            return (float)totalNoise / (float)(freemanChainSize * freemanChainSize);
        case 4://BlueBottle//ContourS//DigitalGeometryBookBetterMarker
            return (float)totalNoise / (float)(freemanChainSize * resultArea);
        case 5://CLetterBetterMarker//ContourS//GreenBottle
            return (float)totalNoise / (float)(freemanChainSize * freemanChainSize * resultArea);
        case 6://apple//BlueBottle//DigitalGeometryBookBetterMarker
            return (float)totalNoise / (float)(resultArea * resultArea);
        case 7:
            return (float)totalNoise / (float)(freemanChainSize * resultArea * resultArea);
        case 8:
            return (float)(totalNoise - freemanChainSize) / (float)(freemanChainSize);
        case 9://apple//BlueBottle//CLetterBetterMarker
            return (float)(totalNoise - freemanChainSize) / (float)(resultArea);
        case 10://ContourS
            return (float)(totalNoise - freemanChainSize) / (float)(freemanChainSize * freemanChainSize);
        case 11://BlueBottle//ContourS
            return (float)(totalNoise - freemanChainSize) / (float)(freemanChainSize * resultArea);
        case 12://CLetterBetterMarker//ContourS//GreenBottle
            return (float)(totalNoise - freemanChainSize) / (float)(freemanChainSize * freemanChainSize * resultArea);
        case 13://apple//BlueBottle
            return (float)(totalNoise - freemanChainSize) / (float)(resultArea * resultArea);
        case 14:
            return (float)(totalNoise - freemanChainSize) / (float)(freemanChainSize * resultArea * resultArea);
        case 15:
            return standardDeviation;
        case 16:
            return (float)(totalNoise * totalNoise) / (float)resultArea;
        case 17:
            return (float)((totalNoise - freemanChainSize) * (totalNoise - freemanChainSize)) / (float)resultArea;
        case 18:
            return -contoursNeighbourhoodSum / (float)totalContourLength;
        case 19:
            return -contoursNeighbourhoodSum / (float)totalContourLength / (float)totalNoise;
        case 20:
            return -contoursNeighbourhoodSum / (float)totalContourLength / (float)(totalNoise - freemanChainSize);
        case 21:
            return -contoursNeighbourhoodMax / (float)totalContourLength;
        case 22:
            return -contoursNeighbourhoodWeightedMax / (float)totalContourLength;
    }//remember of changing NUMBER_OF_CRITERIA!!!
    return (float)totalNoise/(float)freemanChainSize;
}

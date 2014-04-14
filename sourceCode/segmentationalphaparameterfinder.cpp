#include "segmentationalphaparameterfinder.h"
#include "meaningfulscales.h"
#include "float.h"
#include "timer.h"

#include <QDebug>

#include "include/Image.h"


SegmentationAlphaParameterFinder::SegmentationAlphaParameterFinder (SegmentationBasedOnComponentTree &algorithm, QImage &input) :
    segmentationAlgorithm (algorithm),
    gradient (ImageGradient (input)),
    width (algorithm.GetInputWidth ()),
    height (algorithm.GetInputHeight ())
{
}


float SegmentationAlphaParameterFinder::FindAlpha (float alphaStep, bool useMeaningfulScales = true){
    //cerr.rdbuf (0);

    float alpha = 0.0;
    segmentationAlgorithm.DoSegmentationWithPreviousComponentTree (alpha);
    unsigned int previousIterationPixelsFoundedNumber;
    unsigned int pixelsFoundedNumber = segmentationAlgorithm.FoundedPixelNumber ();

    float theBestCriteriumValue, criteriumValue;
    float theBestAlpha = 0.0;

    unsigned int theLongestContourLength;
    float contoursNeighbourhoodGradient;
    unsigned int totalNumberOfContoursPixels;

    float meaningfulScalesCriterium = 1.0;

    {
        MeaningfulScales ms (*segmentationAlgorithm.GetResultImage ());
        theLongestContourLength = ms.GetFreemanChainSize ();
        if (theLongestContourLength > MeaningfulScales::MinimalFreemanChainSize){
            contoursNeighbourhoodGradient = weightedMaxOfContoursNeighbourhoodGradient (&(ms.contours), theLongestContourLength, totalNumberOfContoursPixels);

            if (useMeaningfulScales){
                unsigned int numberOfMeaningfulPixelsOfTheLongestContour = ms.GetNumberOfMeaningfulPixels ();
                meaningfulScalesCriterium = (float)theLongestContourLength / (float)numberOfMeaningfulPixelsOfTheLongestContour;
            }

            theBestCriteriumValue = criteriumValue = (float)totalNumberOfContoursPixels / (contoursNeighbourhoodGradient * meaningfulScalesCriterium);
        } else
            theBestCriteriumValue = DBL_MAX;
    }

    alpha += alphaStep;
    while (alpha <= 1.0){
        previousIterationPixelsFoundedNumber = pixelsFoundedNumber;
        segmentationAlgorithm.DoSegmentationWithPreviousComponentTree (alpha);
        pixelsFoundedNumber = segmentationAlgorithm.FoundedPixelNumber ();

        if (previousIterationPixelsFoundedNumber != pixelsFoundedNumber){
            MeaningfulScales ms (*segmentationAlgorithm.GetResultImage ());
            theLongestContourLength = ms.GetFreemanChainSize ();
            if (theLongestContourLength > MeaningfulScales::MinimalFreemanChainSize){
                contoursNeighbourhoodGradient = weightedMaxOfContoursNeighbourhoodGradient (&(ms.contours), theLongestContourLength, totalNumberOfContoursPixels);

                if (useMeaningfulScales){
                    unsigned int numberOfMeaningfulPixelsOfTheLongestContour = ms.GetNumberOfMeaningfulPixels ();
                    meaningfulScalesCriterium = (float)theLongestContourLength / (float)numberOfMeaningfulPixelsOfTheLongestContour;
                }

                criteriumValue = (float)totalNumberOfContoursPixels / (contoursNeighbourhoodGradient * meaningfulScalesCriterium);

                if (theBestCriteriumValue > criteriumValue){
                    theBestCriteriumValue = criteriumValue;
                    theBestAlpha = alpha;
                }
            }
        }

        alpha += alphaStep;
    }

    return theBestAlpha;
}


float SegmentationAlphaParameterFinder::FindAlphaByBisection (unsigned char numberOfBisections, bool useMeaningfulScales){
    cerr.rdbuf (0);

    qDebug () << numberOfBisections;

    float alphaLeft = 0.0;
    segmentationAlgorithm.DoSegmentationWithPreviousComponentTree (alphaLeft);
    unsigned int pixelsFoundedNumberLeft = segmentationAlgorithm.FoundedPixelNumber ();
    float criteriumValueLeft = DBL_MAX;
    {
        MeaningfulScales ms (*segmentationAlgorithm.GetResultImage ());
        unsigned int theLongestContourLength = ms.GetFreemanChainSize ();
        if (theLongestContourLength > MeaningfulScales::MinimalFreemanChainSize){
            unsigned int totalNumberOfContoursPixels;
            float contoursNeighbourhoodGradient = weightedMaxOfContoursNeighbourhoodGradient (&(ms.contours), theLongestContourLength, totalNumberOfContoursPixels);

            float meaningfulScalesCriterium = 1.0;
            if (useMeaningfulScales){
                unsigned int numberOfMeaningfulPixelsOfTheLongestContour = ms.GetNumberOfMeaningfulPixels ();
                meaningfulScalesCriterium = (float)theLongestContourLength / (float)numberOfMeaningfulPixelsOfTheLongestContour;
            }

            criteriumValueLeft = (float)totalNumberOfContoursPixels / (contoursNeighbourhoodGradient * meaningfulScalesCriterium);
        }
    }

    float alphaRight = 1.0;
    segmentationAlgorithm.DoSegmentationWithPreviousComponentTree (alphaRight);
    unsigned int pixelsFoundedNumberRight = segmentationAlgorithm.FoundedPixelNumber ();
    float criteriumValueRight = DBL_MAX;
    if (pixelsFoundedNumberLeft != pixelsFoundedNumberRight){
        MeaningfulScales ms (*segmentationAlgorithm.GetResultImage ());
        unsigned int theLongestContourLength = ms.GetFreemanChainSize ();
        if (theLongestContourLength > MeaningfulScales::MinimalFreemanChainSize){
            unsigned int totalNumberOfContoursPixels;
            float contoursNeighbourhoodGradient = weightedMaxOfContoursNeighbourhoodGradient (&(ms.contours), theLongestContourLength, totalNumberOfContoursPixels);

            float meaningfulScalesCriterium = 1.0;
            if (useMeaningfulScales){
                unsigned int numberOfMeaningfulPixelsOfTheLongestContour = ms.GetNumberOfMeaningfulPixels ();
                meaningfulScalesCriterium = (float)theLongestContourLength / (float)numberOfMeaningfulPixelsOfTheLongestContour;
            }

            criteriumValueRight = (float)totalNumberOfContoursPixels / (contoursNeighbourhoodGradient * meaningfulScalesCriterium);
        }
    } else
        return 0.0;

    if (numberOfBisections > 0){
        float alpha;
        float criteriumValue;
        if (criteriumValueLeft < criteriumValueRight){
            alpha = alphaLeft;
            criteriumValue = criteriumValueLeft;
        } else{
            alpha = alphaRight;
            criteriumValue = criteriumValueRight;
        }
        calculate (numberOfBisections, useMeaningfulScales, alpha, criteriumValue, alphaLeft, pixelsFoundedNumberLeft, criteriumValueLeft, alphaRight, pixelsFoundedNumberRight, criteriumValueRight);
        return alpha;
    }
    if (criteriumValueLeft < criteriumValueRight)
        return alphaLeft;
    return alphaRight;
}


float SegmentationAlphaParameterFinder::weightedMaxOfContoursNeighbourhoodGradient (std::vector<std::vector<LibTIM::Point<LibTIM::U8> > > *contours, unsigned int maxContourLength, unsigned int &totalNumberOfContoursPixels){
    totalNumberOfContoursPixels = 0;

    float result = 0.0;
    float max, gradientValue, contourValue;
    short x, y;
    unsigned short widthMinus1 = width - 1;
    unsigned short heightMinus1 = height - 1;

    unsigned int contourLength;

    bool x0, y0, xWidth, yHeight;
    vector<std::vector<LibTIM::Point<LibTIM::U8> > >::const_iterator contoursEnd = contours -> end ();
    for (vector<std::vector<LibTIM::Point<LibTIM::U8> > >::iterator c = contours -> begin (); c != contoursEnd; ++c){
        vector<LibTIM::Point<LibTIM::U8> >::const_iterator end = c -> end ();
        contourValue = 0.0;
        for (vector<LibTIM::Point<LibTIM::U8> >::iterator p = c -> begin (); p != end; ++p){
            x = p -> x;
            y = p -> y;

            x0 = (x > 0);
            y0 = (y > 0);
            xWidth = (x < widthMinus1);
            yHeight = (y < heightMinus1);

            max = gradient.GetValue (x, y);

            if (x0){
                gradientValue = gradient.GetValue (x - 1, y);
                if (max < gradientValue)
                    max = gradientValue;

                if (y0){
                    gradientValue = gradient.GetValue (x - 1, y - 1);
                    if (max < gradientValue)
                        max = gradientValue;
                }

                if (yHeight){
                    gradientValue = gradient.GetValue (x - 1, y + 1);
                    if (max < gradientValue)
                        max = gradientValue;
                }
            }

            if (xWidth){
                gradientValue = gradient.GetValue (x + 1, y);
                if (max < gradientValue)
                    max = gradientValue;

                if (y0){
                    gradientValue = gradient.GetValue (x + 1, y - 1);
                    if (max < gradientValue)
                        max = gradientValue;
                }

                if (yHeight){
                    gradientValue = gradient.GetValue (x + 1, y + 1);
                    if (max < gradientValue)
                        max = gradientValue;
                }
            }

            if (y0){
                gradientValue = gradient.GetValue (x, y - 1);
                if (max < gradientValue)
                    max = gradientValue;
            }

            if (yHeight){
                gradientValue = gradient.GetValue (x, y + 1);
                if (max < gradientValue)
                    max = gradientValue;
            }

            contourValue += max;
        }
        contourLength = c -> size ();
        result += contourValue / (maxContourLength - contourLength + 1);
        totalNumberOfContoursPixels += contourLength;
    }
    return result;
}

int mk = 0;
void SegmentationAlphaParameterFinder::calculate (
        unsigned char numberOfBisections,
        bool &useMeaningfulScales,
        float &alpha,
        float &criteriumValue,
        float alphaLeft,
        unsigned int pixelsFoundedNumberLeft,
        float criteriumValueLeft,
        float alphaRight,
        unsigned int pixelsFoundedNumberRight,
        float criteriumValueRight)
{
    float alphaBisection = alphaLeft + (alphaRight - alphaLeft) / 2.0;
    segmentationAlgorithm.DoSegmentationWithPreviousComponentTree (alphaBisection);
    unsigned int pixelsFoundedNumber = segmentationAlgorithm.FoundedPixelNumber ();

    if (pixelsFoundedNumber == pixelsFoundedNumberRight){
        if (pixelsFoundedNumber == pixelsFoundedNumberLeft)
            return;
        if (numberOfBisections > 0)
            calculate (numberOfBisections - 1, useMeaningfulScales, alpha, criteriumValue, alphaLeft, pixelsFoundedNumberLeft, criteriumValueLeft, alphaBisection, pixelsFoundedNumberRight, criteriumValueRight);
    } else if (pixelsFoundedNumber == pixelsFoundedNumberLeft){
        if (numberOfBisections > 0)
            calculate (numberOfBisections - 1, useMeaningfulScales, alpha, criteriumValue, alphaBisection, pixelsFoundedNumberLeft, criteriumValueLeft, alphaRight, pixelsFoundedNumberRight, criteriumValueRight);
    } else{
        float criteriumValueBisection;
        {
            MeaningfulScales ms (*segmentationAlgorithm.GetResultImage ());
            unsigned int theLongestContourLength = ms.GetFreemanChainSize ();
            if (theLongestContourLength > MeaningfulScales::MinimalFreemanChainSize){
                unsigned int totalNumberOfContoursPixels;
                float contoursNeighbourhoodGradient = weightedMaxOfContoursNeighbourhoodGradient (&(ms.contours), theLongestContourLength, totalNumberOfContoursPixels);

                float meaningfulScalesCriterium = 1.0;
                if (useMeaningfulScales){
                    unsigned int numberOfMeaningfulPixelsOfTheLongestContour = ms.GetNumberOfMeaningfulPixels ();
                    meaningfulScalesCriterium = (float)theLongestContourLength / (float)numberOfMeaningfulPixelsOfTheLongestContour;
                }

                criteriumValueBisection = (float)totalNumberOfContoursPixels / (contoursNeighbourhoodGradient * meaningfulScalesCriterium);
                if (criteriumValueBisection < criteriumValue){
                    alpha = alphaBisection;
                    criteriumValue = criteriumValueBisection;
                }
            }
        }

        mk++;
        qDebug () << mk << alphaBisection << alphaLeft << alphaRight << pixelsFoundedNumber << pixelsFoundedNumberLeft << pixelsFoundedNumberRight;

        if (numberOfBisections > 0){
            calculate (numberOfBisections - 1, useMeaningfulScales, alpha, criteriumValue, alphaLeft, pixelsFoundedNumberLeft, criteriumValueLeft, alphaBisection, pixelsFoundedNumber, criteriumValueBisection);
            calculate (numberOfBisections - 1, useMeaningfulScales, alpha, criteriumValue, alphaBisection, pixelsFoundedNumber, criteriumValueBisection, alphaRight, pixelsFoundedNumberRight, criteriumValueRight);
        }
    }
}

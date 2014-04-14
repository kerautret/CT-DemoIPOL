#include <QImage>
#include <QDebug>
#include <QPainter>

#include "ImaGene/base/Arguments.h"
#include "ImaGene/base/StandardArguments.h"

using namespace std;
using namespace ImaGene;

static Arguments args;


#include "componenttreeinputgenerator.h"
#include "meaningfulscales.h"
#include "meaningfulscalesalphaadjust.h"
#include "segmentationalphaparameterfinder.h"
#include "segmentationbasedoncomponenttree.h"
#include "subimage.h"
#include "timer.h"


int main (int argc, char *argv[]){
    args.addOption ("-alpha", "-alpha <n> ", "0.01");

    args.addOption ("-imageSource", "-imageSource <imageName> ", " ");
    args.addOption ("-source", "-source <imageName> ", " ");

    args.addOption ("-imageMarqueur", "-imageMarqueur <imageName> ", " ");
    args.addOption ("-marker", "-marker <imageName> ", " ");

    args.addOption ("-imageResu", "-imageResu <imageName> ", "resu.png");
    args.addOption ("-result", "-result <imageName> ", "result.png");

    args.addBooleanOption ("-autonegated", "-autonegated ");
    args.addBooleanOption ("-meaningfulScales", "-meaningfulScales ");
    //args.addBooleanOption ("-saveGradientImage", "-saveGradientImage ");
    args.addBooleanOption ("-saveInputImage", "-saveInputImage ");
    args.addBooleanOption ("-noMarker", "-noMarker ");

    args.addBooleanOption ("-adjustedInput", "-adjustedInput ");
    args.addBooleanOption ("-adjustInput", "-adjustInput ");

    args.addOption ("-findBestAlphaWithMS", "-findBestAlphaWithMS <n> ", "1");
    args.addBooleanOption ("-adjustAlpha", "-adjustAlpha ");
    args.addBooleanOption ("-adjustAlphaWithoutMS", "-adjustAlphaWithoutMS ");

    args.addOption ("-bisection", "-bisection <n> ", "6");

    bool readArgsOK = args.readArguments (argc, argv);

    cerr << "tototo"<<endl;
    if (!readArgsOK){
        cerr << "Error occured while arguments reading!" << endl;
        return 1;
    }

    string sourceImage;
    if (args.check ("-source"))
        sourceImage = args.getOption ("-source") -> getValue (0);
    else if (args.check ("-imageSource"))
        sourceImage = args.getOption ("-imageSource") -> getValue (0);
    else{
        cerr << "No source image!" << endl;
        return 1;
    }

    string markerImage;
    if (args.check ("-marker"))
        markerImage = args.getOption ("-marker") -> getValue (0);
    else if (args.check ("-imageMarqueur"))
        markerImage = args.getOption ("-imageMarqueur") -> getValue (0);
    else{
        cerr << "No marker image!" << endl;
        return 1;
    }

    string resultImage;
    if (args.check ("-result"))
        resultImage = args.getOption ("-result") -> getValue (0);
    else if (args.check ("-imageResu"))
        resultImage = args.getOption ("-imageResu") -> getValue (0);
    else{
        cerr << "No result image!" << endl;
        return 1;
    }

    QImage source (0, 0, QImage::Format_ARGB32);
    source.load (sourceImage.c_str ());
    source = source.convertToFormat (QImage::Format_ARGB32);

    QImage marker (0, 0, QImage::Format_ARGB32);
    marker.load (markerImage.c_str ());
    marker = marker.convertToFormat (QImage::Format_ARGB32);

    unsigned int size = source.width () * source.height ();

    Subimage markedPixels (size);
    markedPixels.Update (marker);

    QImage input (source.width (), source.height (), QImage::Format_ARGB32);
    QImage segmentationResult (source.width (), source.height (), QImage::Format_ARGB32);

    //create input
    InputGeneratingParameters parameters;
    if (args.check ("-adjustedInput") || args.check ("-adjustInput")){
        parameters.redCanalIncluded = true;
        parameters.greenCanalIncluded = true;
        parameters.blueCanalIncluded = true;
        parameters.hueCanalIncluded = true;
        parameters.saturationCanalIncluded = true;
        parameters.valueCanalIncluded = true;
        parameters.lightnessCanalIncluded = true;
        parameters.cyanCanalIncluded = false;
        parameters.magentaCanalIncluded = false;
        parameters.yellowCanalIncluded = false;
        parameters.keyCanalIncluded = false;

        //parameters.negationMode = NO_NEGATION;
        //parameters.negationMode = NEGATION;
        parameters.negationMode = AUTONEGATION;

        //parameters.outOfRangeSolution = 1;
        //parameters.outOfRangeSolution = 2;
        parameters.outOfRangeSolution = 6;

        //parameters.canalsMixingMode = AVERAGE_MEAN;
        parameters.canalsMixingMode = WEIGHTED_MEAN_BY_COLOURS_DIFFERENCES;
        //parameters.canalsMixingMode = WEIGHTED_MEAN_BY_BINARY_HISTOGRAM_COMPARISON;
        //parameters.canalsMixingMode = WEIGHTED_MEAN_BY_PROPORTIONAL_HISTOGRAM_COMPARISON;

        //parameters.imageTransformationMode = NO_TRANSFORMATION;
        //parameters.imageTransformationMode = MARKED_PIXELS_HISTOGRAM_BUBBLE_BRIGHTENING;
        //parameters.imageTransformationMode = MARKED_PIXELS_MAXIMAL_VALUE_BRIGHTENING;
        parameters.imageTransformationMode = MARKED_PIXELS_AVERAGE_PIXEL_VALUE_BRIGHTNENING;
        //parameters.imageTransformationMode = MARKED_PIXELS_MAXIMAL_AVERAGE_PIXEL_VALUE_BRIGHTENING;
    } else if (args.check ("-autonegated")){
        parameters.redCanalIncluded = true;
        parameters.greenCanalIncluded = true;
        parameters.blueCanalIncluded = true;
        parameters.hueCanalIncluded = true;
        parameters.saturationCanalIncluded = true;
        parameters.valueCanalIncluded = true;
        parameters.lightnessCanalIncluded = true;
        parameters.cyanCanalIncluded = false;
        parameters.magentaCanalIncluded = false;
        parameters.yellowCanalIncluded = false;
        parameters.keyCanalIncluded = false;

        parameters.negationMode = AUTONEGATION;

        parameters.outOfRangeSolution = 6;

        parameters.canalsMixingMode = WEIGHTED_MEAN_BY_COLOURS_DIFFERENCES;

        parameters.imageTransformationMode = NO_TRANSFORMATION;
    } else{
        parameters.redCanalIncluded = true;
        parameters.greenCanalIncluded = false;
        parameters.blueCanalIncluded = false;
        parameters.hueCanalIncluded = false;
        parameters.saturationCanalIncluded = false;
        parameters.valueCanalIncluded = false;
        parameters.lightnessCanalIncluded = false;
        parameters.cyanCanalIncluded = false;
        parameters.magentaCanalIncluded = false;
        parameters.yellowCanalIncluded = false;
        parameters.keyCanalIncluded = false;

        parameters.negationMode = NO_NEGATION;

        parameters.outOfRangeSolution = 1;

        parameters.canalsMixingMode = AVERAGE_MEAN;

        parameters.imageTransformationMode = NO_TRANSFORMATION;
    }

    ComponentTreeInputGenerator inputGenerator (size);
    inputGenerator.GenerateInput (source, input, parameters, markedPixels);
    if (args.check ("-saveInputImage"))
        input.save ("input.png");

    SegmentationBasedOnComponentTree segmentationAlgorithm (segmentationResult.width (), segmentationResult.height ());
    segmentationAlgorithm.ComputeComponentTreeOnly (input, marker);

    if (args.check ("-findBestAlphaWithMS")){
        /*if (args.check ("-saveGradientImage")){
            QImage gradientImage (input.width (), input.height (), QImage::Format_ARGB32);
            unsigned int *bits = (unsigned int *)gradientImage.bits();
            for (unsigned int i = 0; i < size; i++){
                unsigned char canalColour = gradient[i] * 255;
                bits[i] = qRgba (canalColour, canalColour, canalColour, 255);
            }
            gradientImage.save ("gradient.png");
        }*/

        Timer timer;
        timer.StartCycle ();
        MeaningfulScalesAlphaAdjust msaa (segmentationAlgorithm, args.getOption ("-findBestAlphaWithMS") -> getIntValue (0), input);
        float alpha = msaa.FindAlpha (0.01, 10);
        timer.StopCycle ();
        segmentationAlgorithm.DoSegmentationWithPreviousComponentTree (alpha, segmentationResult);
        qDebug () << "The best alpha:" << alpha << "Time: " << (float)timer.TotalTime () / (float)CLOCKS_PER_SEC;
    } else if (args.check ("-adjustAlpha")){
        Timer timer;
        timer.StartCycle ();
        SegmentationAlphaParameterFinder alphaFinder (segmentationAlgorithm, input);
        float alpha = args.check ("-bisection") ? alphaFinder.FindAlphaByBisection (args.getOption ("-bisection") -> getIntValue (0), true) : alphaFinder.FindAlpha (0.01, true);
        timer.StopCycle ();
        segmentationAlgorithm.DoSegmentationWithPreviousComponentTree (alpha, segmentationResult);
        qDebug () << "The best alpha:" << alpha << "Time: " << (float)timer.TotalTime () / (float)CLOCKS_PER_SEC;
    } else if (args.check ("-adjustAlphaWithoutMS")){
        Timer timer;
        timer.StartCycle ();
        SegmentationAlphaParameterFinder alphaFinder (segmentationAlgorithm, input);
        float alpha = args.check ("-bisection") ? alphaFinder.FindAlphaByBisection (args.getOption ("-bisection") -> getIntValue (0), false) : alphaFinder.FindAlpha (0.01, false);
        timer.StopCycle ();
        segmentationAlgorithm.DoSegmentationWithPreviousComponentTree (alpha, segmentationResult);
        qDebug () << "The best alpha:" << alpha << "Time: " << (float)timer.TotalTime () / (float)CLOCKS_PER_SEC;
    } else if (args.check ("-alpha")){
        float alpha = args.getOption ("-alpha") -> getFloatValue (0);
        segmentationAlgorithm.DoSegmentationWithPreviousComponentTree (alpha, segmentationResult);
    } else{
        cerr << "No -alpha <n>, -adjustAlpha or -adjustAlphaWithoutMS parameter. Choose one!";
        return 1;
    }

    QPainter painter (&source);

    unsigned int *bits, *bitsEnd;
    bits = (unsigned int*)segmentationResult.bits ();
    bitsEnd = bits + size;
    while (bits < bitsEnd){
        if (*bits > 0)
            *bits = QColor (0, 0, 255, 150).rgba();
        bits++;
    }
    painter.drawImage (0, 0, segmentationResult);

    if (!args.check ("-noMarker")){
        bits = (unsigned int*)marker.bits ();
        bitsEnd = bits + size;
        while (bits < bitsEnd){
            if (*bits > 0)
                *bits = QColor (255, 0, 0, 150).rgba();
            bits++;
        }
        painter.drawImage (0, 0, marker);
    }

    if (args.check ("-meaningfulScales")){
        //timers[8].StartCycle ();
        MeaningfulScales ms (*segmentationAlgorithm.GetResultImage());
        //timers[8].StopCycle ();
        painter.drawImage (0, 0, ms.GetResult ());
    }

    source.save (resultImage.c_str ());
    
    return 0;
}

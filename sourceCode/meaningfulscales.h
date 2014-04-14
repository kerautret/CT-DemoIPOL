#ifndef MEANINGFULSCALES_H
#define MEANINGFULSCALES_H

#include <QImage>
#include "ImaGene/dgeometry2d/FreemanChain.h"
#include "ImaGene/helper/MultiscaleProfile.h"

#include "ImaGene/base/Proxy.h"
#include "ImaGene/base/Vector.h"
#include "ImaGene/base/Shapes.h"
#include "ImaGene/digitalnD/GridEmbedder.h"
#include "ImaGene/digitalnD/KnSpace.h"
#include "ImaGene/digitalnD/KnCharSet.h"
#include "ImaGene/digitalnD/KnRCellSet.h"
#include "ImaGene/digitalnD/KnShapes.h"
#include "ImaGene/digitalnD/C4CIteratorOnBdry.h"
#include "ImaGene/digitalnD/BelAdjacency.h"
#include "ImaGene/digitalnD/ObjectBoundary.h"
#include "ImaGene/digitalnD/Frame2D.h"
#include "ImaGene/helper/ContourHelper.h"
#include "ImaGene/helper/MultiscaleProfile.h"
#include "ImaGene/helper/ShapeHelper.h"

#include <algorithm>
#include "ImaGene/dgeometry2d/C4CGeometry.h"
#include "ImaGene/dgeometry2d/C4CSegmentPencil.h"
#include "ImaGene/base/Vector2i.h"
#include "ImaGene/mathutils/Mathutils.h"
#include "ImaGene/mathutils/Statistics.h"
#include "ImaGene/mathutils/SimpleLinearRegression.h"
#include "ImaGene/dgeometry2d/C4CIteratorOnFreemanChain.h"
#include "ImaGene/dgeometry2d/C4CTangentialCover.h"
#include "ImaGene/dgeometry2d/FreemanChain.h"
#include "ImaGene/dgeometry2d/FreemanChainTransform.h"
#include "ImaGene/digitalnD/C4CIteratorOnFreemanChainSurface.h"
#include "ImaGene/digitalnD/C4CIteratorOnSurface.h"
#include "ImaGene/digitalnD/KnSpace.h"
#include "ImaGene/helper/C4CTangentialCoverGeometry.h"
#include "ImaGene/helper/CurveVariationsHelper.h"
#include "ImaGene/helper/DrawingXFIG.h"

#include <sstream>

#include "include/Image.h"


class MeaningfulScales{
public:
    MeaningfulScales (LibTIM::Image<LibTIM::U8> &image);
    ~MeaningfulScales ();
    QImage GetResult ();
    unsigned int GetFreemanChainSize ();
    unsigned int GetNumberOfMeaningfulPixels ();
    unsigned int GetSumOfNoise ();
    float GetNoiseStanardDeviation ();

    static const unsigned int MinimalFreemanChainSize = 300;

    int samplingSizeMax;

    std::vector<std::vector<LibTIM::Point<LibTIM::U8> > > contours;

protected:
    void doMeaningfulScales ();
    void getFreemanChain ();
    void getPGMStream ();

    static const uint mscales_min_size = 1;
    static const double mscales_max_slope = 0;

    LibTIM::Image<LibTIM::U8> &inputImage;
    QImage boxes;
    unsigned short inputWidth;
    unsigned short inputHeight;
    std::stringstream pgmFile;
    std::stringstream freemanChainStream;
    ImaGene::FreemanChain freemanChain;
    ImaGene::MultiscaleProfile *MP;
};

#endif // MEANINGFULSCALES_H

#ifndef PIXELOPERATIONS_H
#define PIXELOPERATIONS_H


#include <qimage.h>
#include <cmath>

namespace cg2{
    void calcImageCharacteristics(QImage * image, double*& histogram_ref, int& variance_ref, int& average_ref, const bool linear_scaling);
    inline double* histogramm = new double[256];
    inline int variance = 0;
    inline int average = 0;
    QImage* changeImageDynamic(QImage *  image, int newDynamicValue);
    QImage* adjustBrightness(QImage *  image, int brightness_adjust_factor);
    QImage* adjustContrast(QImage *  image, double contrast_adjust_factor);
    QImage* doRobustAutomaticContrastAdjustment(QImage *  image, double plow, double phigh);
    float clamping(float x);
    typedef struct {
        float y;
        float cb;
        float cr;
    }    QYcbcr;
    QYcbcr convertToYcbcr(QRgb input);
    QRgb convertToRgb(QYcbcr input);

}




#endif // PIXELOPERATIONS_H

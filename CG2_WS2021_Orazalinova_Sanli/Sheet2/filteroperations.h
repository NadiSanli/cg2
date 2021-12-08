#ifndef FILTEROPERATIONS_H
#define FILTEROPERATIONS_H

#include <qimage.h>



namespace cg2 {
    QImage* filterImage(QImage * image, int**& filter, int filter_width, int filter_height, int border_treatment);
    QImage* filterGauss2D(QImage * image, double gauss_sigma, int border_treatment);
    extern int intClamping(int x);

}
#endif // FILTEROPERATIONS_H

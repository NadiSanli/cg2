#include "filteroperations.h"
#include "imageviewer-qt5.h"


namespace cg2 {

    int intClamping(int x);

    /**
     * @brief filterImage
     *      calculate the 2D filter over the image
     *      handle border treatment as desired
     * @param image
     *      input image
     * @param filter
     *      filter matrix with filter coefficients
     *      can reach up to 15x15
     * @param filter_width
     *      filter matrix width in range [0,15]
     * @param filter_height
     *      filter matrix height in range [0,15]
     *
     * @param border_treatment
     *      0: Zentralbereich
     *      1: Zero Padding
     *      2: Konstante Randbedingung
     *      3: Gespiegelte Randbedingung
     * @return new Image to show in GUI
     */
    QImage* filterImage(QImage * image, int**& filter, int filter_width, int filter_height, int border_treatment) {
        /**Berechnen die Summe von Filter-Koeffizienten und daraus den Normierungsfaktor*/
        int sum_filter=0;
        for(int i=0; i<filter_height; i++) {
            for(int j=0; j<filter_width; j++) {
                sum_filter+=filter[i][j];
            }
        }
        double norm_factor=1.0/sum_filter;

        /**Berechnen K - Mitte von Filterhöhe und L - Mitte von Filterbreite*/
        int K=filter_height/2;
        int L=filter_width/2;

        /**Erstellen eine Kopie vom Bild*/
        QImage image_copy=image->copy();


        int pixel_value=0;
        int filter_coeff=0;
        int new_pixel_value=0;
        int pos_x=0;
        int pos_y=0;
        int sum=0;
        float y;
        QYcbcr ycbcr;
        QColor* color {new QColor(127, 127, 127)};

        logFile << "Filter read:" << std::endl;
        for(int i = 0; i < filter_height; i++ ){
            for(int j = 0; j < filter_width; j++ ){
                logFile << filter[i][j];
                if(j < (filter_width-1)){
                    logFile << " | ";
                }
            }
            logFile << std::endl;
        }

        logFile << "filter applied:" << std::endl << "---border treatment: ";

        switch (border_treatment) {
            //Zentralbereich
            case 0:
            /**Iterieren durch die Filtermatrix(2D-Array) und iterieren durch alle Pixel des Bildes und
             * wenden die Koeffizienten der Matrix auf die Helligkeitswerte an*/
                for(int v=L; v<=(image->width()-L-1); v++) {
                    for(int u=K; u<=(image->height()-K-1); u++) {
                        int sum=0;
                        for(int j=-L; j<=L; j++) {
                            for(int i=-K; i<=K; i++) {
                                //RGB zu YCbCr konvertieren
                                ycbcr = convertToYcbcr(image_copy.pixel(u+i, v+j));
                                y = ycbcr.y; //Auf Helligkeit zugreifen
                                filter_coeff=filter[j+L][i+K];
                                sum=sum+filter_coeff*y;
                            }
                        }
                        y=(int) (sum*norm_factor + 0.5); //um richtig zu runden, addieren wir 0.5
                        ycbcr.y=intClamping(y);
                        image->setPixel(u,v,convertToRgb(ycbcr));
                    }
                }

            logFile << "Summe der Filterkoeffizienten ist: " << sum_filter << std::endl;
            logFile << "Der Normierungsfaktor ist: " << norm_factor << std::endl;
                logFile << "Zentralbereich" << std::endl;
                break;
            //Zero Padding
            case 1:
                for(int v=0; v<image->width(); v++) {
                    for(int u=0; u<image->height(); u++) {
                        int sum=0;
                        for(int j=-L; j<=L; j++) {
                            for(int i=-K; i<=K; i++) {

                                if((u+i)>=image->height() || (u+i)<0 || (v+j)>=image->width() || (v+j)<0) {
                                    ycbcr = convertToYcbcr(color->rgb());
                                    //logFile << "Pixelindex Y: " << u+i<< std::endl;
                                    //logFile << "Pixelindex X: " << v+j << std::endl;
                                } else {
                                    ycbcr = convertToYcbcr(image_copy.pixel(v+j, u+i));
                                }
                                y = ycbcr.y; //Auf Helligkeit zugreifen
                                filter_coeff=filter[j+L][i+K];
                                sum=sum+filter_coeff*y;
                            }
                        }
                        y=(int) (sum*norm_factor + 0.5); //um richtig zu runden, addieren wir 0.5
                        ycbcr.y=intClamping(y);
                        image->setPixel(v, u, convertToRgb(ycbcr));
                    }
                }

                logFile << "Zero Padding" << std::endl;
                break;
        //Konstante Randbedingung
        case 2:

            for(int v=0; v<image->width(); v++) {
                for(int u=0; u<image->height(); u++) {
                    int sum=0;
                    for(int j=-L; j<=L; j++) {
                        for(int i=-K; i<=K; i++) {
                            pos_x=v+j;
                            pos_y=u+i;
                            if(v+j < 0) pos_x=0;
                            if(v+j >=image->width()) pos_x=image->width()-1;
                            if(u+i < 0) pos_y=0;
                            if(u+i >= image->height()) pos_y=image->height();

                            ycbcr = convertToYcbcr(image_copy.pixel(pos_x, pos_y));

                            y = ycbcr.y; //Auf Helligkeit zugreifen
                            filter_coeff=filter[j+L][i+K];
                            sum=sum+filter_coeff*y;
                        }
                    }
                    y=(int) (sum*norm_factor + 0.5); //um richtig zu runden, addieren wir 0.5
                    ycbcr.y=intClamping(y);
                    image->setPixel(v,u, convertToRgb(ycbcr));
                }
            }
                logFile << "Konstante Randbedingung" << std::endl;
                break;

            //Gespiegelte Randbedingung
            case 3:
                for(int v=0; v<=(image->width()); v++) {
                    for(int u=0; u<=(image->height()); u++) {
                        int sum=0;
                        for(int j=-L; j<=L; j++) {
                            for(int i=-K; i<=K; i++) {
                                //Der Pixel ist außerhalb des Bildes oben oder unten
                                if((u+i)>image->height() || (u+i)<0) {
                                    ycbcr = convertToYcbcr(image_copy.pixel(v, (u-i)));
                                //Der Pixel ist außerhalb des Bildes rechts oder links
                                } else if((v+j)>image->width() || (v+j)<0){
                                    ycbcr = convertToYcbcr(image_copy.pixel((v-j), u));
                                } else {
                                    ycbcr = convertToYcbcr(image_copy.pixel((v+j), (u+i)));
                                }
                                y = ycbcr.y; //Auf Helligkeit zugreifen
                                filter_coeff=filter[j+L][i+K];
                                sum=sum+filter_coeff*y;
                            }
                        }
                        y=(int) (sum*norm_factor + 0.5); //um richtig zu runden, addieren wir 0.5
                        ycbcr.y=intClamping(y);
                        image->setPixel(v,u, convertToRgb(ycbcr));
                    }
                }
                    logFile << "Gespiegelte Randbedingung" << std::endl;
                    break;
          }

        logFile << "---filter width: " << filter_width << std::endl;
        logFile << "---filter height: " << filter_height << std::endl;
        return image;
    }

    //Methode um alle Werte außerhalb des Bereiches 255-0 werden durch 0 oder 255 ersetzt.
    int intClamping(int x){
        int min = 0.0;
        int max = 255.0;
        if (x < min) {
            return min;
        } else if (x > max){
            return max;
        } else {
            return x;
        }
    }

    /**
     * @brief filterGauss2D
     *      calculate the 2D Gauss filter algorithm via two separate 1D operations,
     *      handle border treatment as desired
     * @param image
     *      input image
     * @param gauss_sigma
     *      sigma for gauss kernel
     * @param border_treatment
     *      0: Zentralbereich
     *      1: Zero Padding
     *      2: Konstante Randbedingung
     *      3: Gespiegelte Randbedingung
     * @return new Image to show in GUI
     */
    QImage* filterGauss2D(QImage * image, double gauss_sigma, int border_treatment){

        const int center=(int)(3.0*gauss_sigma);

        const int gauss_2d_height=2*center+1;
        const int gauss_2d_width=2*center+1;

        float* gauss_1d {new float[gauss_2d_width]};
        double gauss_sigma2=gauss_sigma*gauss_sigma;

        int gauss_1d_length=sizeof(*gauss_1d)/sizeof(float);
        for(int i=0; i< gauss_1d_length; i++) {
            double r=center-i;
            gauss_1d[i]=(float) exp(-0.5*(r*r)/gauss_sigma2);
        }


        //Allokieren Speicherplatz auf Heap für Hauptarray
        float** gauss_2d {new float*[gauss_2d_height]};

        //Allokieren Speicherplatz auf Heap für Subarrays
        for(int i = 0; i < gauss_2d_width; i++) {
            gauss_2d[i]=new float[gauss_2d_height];
        }

        //Füllen Gauss-Array mit den Koeffizienten
        for(int i=0; i<gauss_1d_length; i++) {
            for(int j=0; j<gauss_1d_length; j++) {
                gauss_2d[i][j]=gauss_1d[i]*gauss_1d[j];
            }
        }

        //Berechnen die Summe von Filter-Koeffizienten und daraus den Normierungsfaktor
        int sum_filter=0;
        for(int i=0; i<gauss_2d_height; i++) {
            for(int j=0; j<gauss_2d_width; j++) {
                sum_filter+=gauss_2d[i][j];
            }
        }
        double norm_factor=1.0/sum_filter;

        //Berechnen K - Mitte von Filterhöhe und L - Mitte von Filterbreite
        int K=gauss_2d_height/2;
        int L=gauss_2d_width/2;

        //Erstellen eine Kopie vom Bild
        QImage image_copy=image->copy();

        int filter_coeff;
        float y;
        QYcbcr ycbcr;


        logFile << "2D Gauss-Filter angewendet mit σ: " << gauss_sigma;
        logFile <<  " ---border treatment: ";
        switch (border_treatment) {
            case 0:
                for(int v=L; v<=(image->width()-L-1); v++) {
                    for(int u=K; u<=(image->height()-K-1); u++) {
                        int sum=0;
                        for(int j=-L; j<=L; j++) {
                            for(int i=-K; i<=K; i++) {
                                ycbcr = convertToYcbcr(image_copy.pixel(v+j, u+i));
                                y = ycbcr.y; //Auf Helligkeit zugreifen
                                filter_coeff=gauss_2d[L+j][K+i];
                                sum=sum+filter_coeff*y;
                            }
                        }
                        y=(int) (sum*norm_factor + 0.5); //um richtig zu runden, addieren wir 0.5
                        ycbcr.y=intClamping(y);
                        image->setPixel(v,u,convertToRgb(ycbcr));
                    }
                }
                logFile << "Zentralb ereich" << std::endl;
                break;
            case 1:
                logFile << "Zero Padding" << std::endl;
                break;
            case 2:
                logFile << "Konstante Randbedingung" << std::endl;
                break;
            case 3:
                logFile << "Gespiegelte Randbedingung" << std::endl;
                break;
        }

        //Speicherplatz des Arrays wiederfreigeben, zuerst die Arrays der zweiten Dimension, dann das Hauptarray
        for(int i = 0; i < gauss_2d_width; i++) {
            delete[] gauss_2d[i];
        }
        delete[] gauss_2d;


        return image;
    }

}

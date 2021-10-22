#include "pixeloperations.h"
#include "imageviewer-qt5.h"


namespace cg2 {
    
    /**
     * @brief calcImageCharacteristics
     *      calculation of the histogram, average value and variance of the image
     *      no return values, just set the references to the correct values
     * @param image
     *      working image
     * @param histogram_ref
     *      Reference to "inline double* histogramm = new double[256]"
     *      doubles allowed from [0-100]
     *      calculate the histogram and fill the histogramm:
     *      the histogramm[0] is corresponding to the luminance value 0 and so on
     *      NOTE!: the histogram must be scaled (linear or logarithmic) so that the
     *          highest value is 100!
     *      (reason: histogram image is 256x100 pixel)
     * @param variance_ref
     *      calculate and set the variance of the image to the variance_ref
     * @param average_ref
     *      calculate and set the average of the image to the average_ref
     * @param linear_scaling
     *      boolean, if true -> scale the histogram linear
     *               if false -> scale the histogram logarithmic
     */
    void calcImageCharacteristics(QImage * image, double*& histogram_ref, int& variance_ref, int& average_ref, const bool linear_scaling){
       double y;
        double sum=0;
        int anz_pix=0;

        for (int i = 0; i < image->width(); i++) {
            for (int j = 0; j < image->height(); j++) {
                //Jeden Pixel in pixel speichern mit column/row
                QRgb pixel = image->pixel(i, j);
                //Pro Pixel die einzelnen RGB-Werte rausfiltern
                int rot = qRed(pixel);
                int blau = qBlue(pixel);
                int gruen = qGreen(pixel);

                //in y speichern wir das Luminanz-Signal/Helligkeitswert
                y = 0.299 * rot + 0.587 * gruen + 0.114 * blau;

                //aufsummieren der einzelnen Helligkeitswerte
                sum = sum + y;
            }
        }
        //summieren der pixel
        anz_pix = image->height() * image->width();
        average_ref = (int) ((sum / anz_pix) + 0.5);

        double sum_varianz = 0;
        int y_round = 0;
        for (int i = 0; i < image->width(); i++) {
            for (int j = 0; j < image->height(); j++) {
                //Jeden Pixel in pixel speichern mit column/row
                QRgb pixel = image->pixel(i, j);

                //Pro Pixel die einzelnen RGB-Werte rausfiltern
                int rot = qRed(pixel);
                int blau = qBlue(pixel);
                int gruen = qGreen(pixel);

                //in y speichern wir das Luminanz-Signal/Helligkeitswert
                // Luminanz ignoriert die Farbanteile und holt die Helligkeitswerte aus dem Bild
                y = 0.299 * rot + 0.587 * gruen + 0.114 * blau;
                y_round = (int)((y) + 0.5);

                histogram_ref[y_round] ++;

                //Varianz berechnen
                sum_varianz = sum_varianz + ((y-average_ref)*(y-average_ref));
            }
        }

        //Varianz / Anzahl_pixel
        variance_ref = int (sum_varianz / anz_pix);

        /*Werte im Histogramm linear skalieren*/
       /* int max=0;
        for (int i = 0; i < 256; i++) {
            //max= histogram_ref[i] > max ? histogram_ref[i] : max;
            histogram_ref[i]=histogram_ref[i]/2745.0*100; //2745 ist der maximale Wert
        }*/

        /*Werte im Histogramm logarithmisch skalieren*/
        int max_log=0;
        for (int i = 0; i < 256; i++) {
            if(histogram_ref[i]==0) {

            } else {
                histogram_ref[i]=log(histogram_ref[i]);

                max_log= histogram_ref[i] > max_log ? histogram_ref[i] : max_log;
            }
        }
  /*      for (int i = 0; i < 256; i++) {
            histogram_ref[i]=histogram_ref[i]/(double)max_log*100;
        }
*/
        logFile << "Max: " << max_log << std::endl;

        //Ausgabe von Werten im Histogramm
        for (int i = 0; i < 256; i++) {
            logFile << histogram_ref[i]<<std::endl;
        }

        logFile << "Image characteristics calculated:" << std::endl << "--- Average: " << average_ref << " ; Variance: " << variance_ref << std::endl << "--- Histogram calculated: " << "linear scaling = " << linear_scaling << std::endl;
    }

    /**
     * @brief changeImageDynamic
     *      calculate an image with the desired resolution (given bit depth -> dynamic value)
     *      and return the image pointer
     * @param image
     *      input image
     * @param newDynamicValue
     *      bit depth value for resolution values from 8 to 1
     * @return new Image to show in GUI
     */
    QImage* changeImageDynamic(QImage * image, int newDynamicValue) {
        //Anzahl der Grenzwerte berechnen
        int grenze=(pow(2,newDynamicValue)-1);
        int anz_farbe=(pow(2,newDynamicValue));
        //Array mit Grenzwerten(Helligkeitswerte)
        int grenzewerte[7];
        int intervall=255/anz_farbe;
        //Array mit den Grenzwerten füllen
        for(int i=0; i<anz_farbe; i++) {
            grenzewerte[i]+=intervall;
            logFile << grenzewerte[i];
        }
        //auf Farbwert die hälfte des Intervalls draufrechnen und runden
        //Farbwert + Intervall/2 / int(Intervall) = Arrayposition der jeweiligen Helligkeit ´
        for (int i = 0; i < image->width(); i++) {
            for (int j = 0; j < image->height(); j++) {

            }
        }
        logFile << "Dynamik des Bildes geändert auf: " + std::to_string(newDynamicValue) + " Bit" << std::endl;
        return image;

    }
    constexpr int new_clippedRGB_value(int pixel_value, int brightness_adjust_factor){
        int color = pixel_value + brightness_adjust_factor;
        if(color > 255){
            color = 255;
        }
        else if(color < 0){
            color = 0;
        }
        return color;
    }

    /**
     * @brief adjustBrightness
     *      Add brightness adjust on each pixel in the Image
     * @param image
     *      Input Image to work with
     * @param brightness_adjust_factor
     *      [-255,255]
     *      the brightness adjust shift, will be added on each pixel
     * @return result image, will be shown in the GUI
     */
    QImage* adjustBrightness(QImage * image, int brightness_adjust_factor){
        //Aufgabe d, Teil Helligkeit. TODO Kontrast
        image = new QImage(*backupImage);
        int image_width = image->width();
        int image_height = image->height();

        //Mit doppelter for-schleife durch das ganze Bild um die Pixel zu speichern
        for(int column = 0 ; column < image_width; column++){
            for(int row = 0  ; row < image_height; row++){
                // Pixel object and pixel getter from image
                //Aktuelles x und y übergeben. Damit bekommt man die aktuellen Pixel
                QRgb pixel = image->pixel(column, row);

                int rot = new_clippedRGB_value(qRed(pixel), brightness_adjust_factor);
                int gruen = new_clippedRGB_value(qGreen(pixel), brightness_adjust_factor);
                int blau = new_clippedRGB_value(qBlue(pixel), brightness_adjust_factor);

                // pixel setter in image with qRgb
                // note that qRgb values must be in [0,255]
                //an einer Stelle kann man einen neuen Pixelwert zuweisen
                //image->setPixel(column, row, qRgb(rot,gruen,blau));
                image->setPixel(row, column, qRgb(rot,gruen,blau));
            }
        }

        logFile << "Brightness adjust applied with factor = " <<brightness_adjust_factor << std::endl;
        return image;

    }

    /**
     * @brief adjustContrast
     *      calculate an contrast adjustment on each pixel in the Image
     * @param image
     *      Input Image to work with
     * @param contrast_adjust_factor
     *      [0,3]
     *      the contrast adjust factor
     * @return result image, will be shown in the GUI
     */
    QImage* adjustContrast(QImage * image, double contrast_adjust_factor){

        for(int i=0;i<image->width();i++)
        {
            for(int j=0;j<image->height();j++)
            {
            }
        }

        logFile << "Contrast calculation done with contrast factor: " << contrast_adjust_factor << std::endl;
        return image;
    }


    /**
     * @brief doRobustAutomaticContrastAdjustment
     *      calculate the robust automatic contrast adjustment algorithm with the image as input
     * @param image
     *      Input Image to work with
     * @param plow
     *      [0%,5%]
     *      brightness saturation
     * @param phigh
     *      [0%,5%]
     *      dark saturation
     * @return result image, will be shown in the GUI
     */
    QImage* doRobustAutomaticContrastAdjustment(QImage * image, double plow, double phigh){

        for(int i=0;i<image->width();i++)
        {
            for(int j=0;j<image->height();j++)
            {
            }
        }

        logFile << "Robust automatic contrast adjustment applied with:"<< std::endl << "---plow = " << (plow*100) <<"%" << std::endl << "---phigh = " << (phigh*100)<<"%" << std::endl;

        return image;
    }

}



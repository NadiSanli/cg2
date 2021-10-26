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
    //Aufgabe 1a) Helligkeit
    void calcImageCharacteristics(QImage * image, double*& histogram_ref, int& variance_ref, int& average_ref, const bool linear_scaling){
       double y;
        double sum=0;
        int anz_pix=0;
        int haeufigkeit_helligkeit[256];

        //Häufigkeiten auf 0 setzen falls Helligkeitswert nicht vergeben ist (Multiplikation)
        for(int i=0; i<256; i++){
            haeufigkeit_helligkeit[i] =0;
        }

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

                haeufigkeit_helligkeit[(int)y]++;
            }
        }
        //Summieren der Pixel
        anz_pix = image->height() * image->width();
        average_ref = (int) ((sum / anz_pix) + 0.5);

        //Aufgabe 1a) Varianz
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

                // in y speichern wir das Luminanz-Signal/Helligkeitswert
                // Luminanz ignoriert die Farbanteile und holt die Helligkeitswerte aus dem Bild
                y = 0.299 * rot + 0.587 * gruen + 0.114 * blau;
                y_round = (int)((y) + 0.5);

                histogram_ref[y_round] ++;

                //Varianz berechnen
                sum_varianz = sum_varianz + ((y-average_ref)*(y-average_ref));
            }
        }

        // Varianz / Anzahl_pixel
        variance_ref = int (sum_varianz / anz_pix);

       // Aufgabe 1b) Lineares und logarithmisches Histogramm
       /* Werte im Histogramm linear skalieren */
       /* int max=0;
        for (int i = 0; i < 256; i++) {
            //max= histogram_ref[i] > max ? histogram_ref[i] : max;
            histogram_ref[i]=histogram_ref[i]/2745.0*100; //2745 ist der maximale Wert
        }

        //Werte im Histogramm logarithmisch skalieren
        int max_log=0;
        for (int i = 0; i < 256; i++) {
            if(histogram_ref[i]==0) {

            } else {
                histogram_ref[i]=log(histogram_ref[i]);

                max_log= histogram_ref[i] > max_log ? histogram_ref[i] : max_log;
            }
        }
        for (int i = 0; i < 256; i++) {
            histogram_ref[i]=histogram_ref[i]/(double)max_log*100;
        }*/

        int max_log = 0;

        for(int i=0; i<256; i++){
            //Höchsten Helligkeitswert zum skalieren des Histogramms
            if (haeufigkeit_helligkeit[i] > max_log){
                max_log = haeufigkeit_helligkeit[i];
            }
        }

        for(int i=0; i<256; i++){
            histogram_ref[i] = haeufigkeit_helligkeit[i]/(max_log/100);
        }

        logFile << "Image characteristics calculated:" << std::endl << "--- Average: " << average_ref << " ; Variance: " << variance_ref << std::endl << "--- Histogram calculated: " << "linear scaling = " << linear_scaling << std::endl;
    }

    //Methode um alle Werte außerhalb des Bereiches 255-0 werden durch 0 oder 255 ersetzt.
    float clamping(float x){
        float min = 0.0;
        float max = 255.0;
        if (x < min) {
            return min;
        } else if (x > max){
            return max;
        } else {
            return x;
        }
    }

    //Konvertiert inputRGB zu YCBCR
    QYcbcr convertToYcbcr(QRgb input){
        float rot = qRed(input);
        float gruen = qGreen(input);
        float blau = qBlue(input);

        float y = clamping(0.299*rot + 0.587*gruen + 0.114*blau);
        float cb = clamping(-0.169 * rot - 0.331*gruen + 0.5 * blau +128.0);
        float cr = clamping(0.5 * rot - 0.419 * gruen - 0.081* blau + 128.0);

        return QYcbcr{y,cb,cr};
    }

    QRgb convertToRgb(QYcbcr input){
        float y = input.y;
        float cb = input.cb;
        float cr = input.cr;

        float rot = clamping((cb - 128.0) + 1.4 * (cr -128.0));
        float gruen = clamping(y - 0.35 * (cb - 128.0) - 0.71 * (cr - 128.0));
        float blau = clamping(y + 1.78 * (cb - 128.0));
        return qRgb(rot,gruen,blau);

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
     //Aufgabe 1c)
/*    QImage* changeImageDynamic(QImage * image, int newDynamicValue) {
        image = new QImage(*backupImage);
        //Anzahl der Grenzwerte berechnen
        int grenze=(pow(2,newDynamicValue)-1);
        int anz_farbe=(pow(2,newDynamicValue));
        //Array mit Grenzwerten(Helligkeitswerte)

        int* grenzwerte =new int[grenze];
        grenzwerte[0]=0;
        grenzwerte[anz_farbe]=255;
        int* farbwerte=new int[anz_farbe];

        int intervall=255/anz_farbe;
        //Array mit den Grenzwerten füllen
        for(int i=1; i<anz_farbe-1; i++) {
            grenzwerte[i]+=intervall;
            logFile << grenzwerte[i];
        }

        //Array mit den Farbwerten füllen
        farbwerte[0]=0;
        for(int i=0; i<anz_farbe; i++) {
            farbwerte[i]+=intervall;
            logFile << farbwerte[i];
        }

        int y=0;
        int y_round=0;
        int image_width = image->width();
        int image_height = image->height();
        //auf Farbwert die hälfte des Intervalls draufrechnen und runden
        //Farbwert + Intervall/2 / int(Intervall) = Arrayposition der jeweiligen Helligkeit ´
        for (int i = 0; i < image->width(); i++) {
            for (int j = 0; j < image->height(); j++) {
                QRgb pixel = image->pixel(i, j);

                //Pro Pixel die einzelnen RGB-Werte rausfiltern
                int rot = qRed(pixel);
                int blau = qBlue(pixel);
                int gruen = qGreen(pixel);

                //in y speichern wir das Luminanz-Signal/Helligkeitswert
                // Luminanz ignoriert die Farbanteile und holt die Helligkeitswerte aus dem Bild
                y = 0.299 * rot + 0.587 * gruen + 0.114 * blau;

                //wenn wir zum int casten, wird alles nach der Komma abgeschnitten,
                //d.h. abgerundet, deswegen addieren wir 0,5, um mathematisch korrekt zu runden
                y_round = (int)((y) + 0.5);

                //damit bekommen wir die Position im Array mit
                int pos_array=(y_round+intervall/2)/(int)intervall;
                logFile << pos_array;

                //weisen den neuen Helligkeitswert jedem Pixel zu
                y_round=farbwerte[pos_array];

                //Luminanz zurück nach RGB berechnen
                for(int column = 0 ; column < image_width; column++){
                    for(int row = 0  ; row < image_height; row++){
                image->setPixel(row, column, qRgb(rot,gruen,blau));

                    }
                }
            }
        }
        logFile << "Dynamik des Bildes geändert auf: " + std::to_string(newDynamicValue) + " Bit" << std::endl;
        return image;

    }*/

    QImage* changeImageDynamic(QImage * image, int newDynamicValue) {
        double anz_farbe=(pow(2,newDynamicValue));
        for (int i = 0; i < image->width(); i++) {
            for (int j = 0; j < image->height(); j++) {
                QYcbcr ycbcr = convertToYcbcr(image -> pixel(i,j));
                //Auf Helligkeit zugreifen
                double ergebnis = ycbcr.y;
                ergebnis *= (anz_farbe / 256.0);
                ergebnis = round(anz_farbe);
                ergebnis /= (anz_farbe / 256.0);
                ycbcr.y = ergebnis;
                image->setPixel(i,j, convertToRgb(ycbcr));
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
    //Aufgabe 1d) Helligkeit
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
    //Aufgabe 1d) Kontrast
    QImage* adjustContrast(QImage * image, double contrast_adjust_factor){

        float obere_grenze = 255.0;
        float untere_grenze = 0.0;
        double y = 0.0;
        int y_round=0;
        int ytoR = 0.0;
        int ytorRound = 0.0;
        image = new QImage(*backupImage);
        int image_width = image->width();
        int image_height = image->height();

        //Mit doppelter for-schleife durch das ganze Bild um die Pixel zu speichern
        for(int column = 0 ; column < image_width; column++){
            for(int row = 0  ; row < image_height; row++){
                QRgb pixel = image->pixel(column, row);

                int rot = qRed(pixel);
                int blau = qBlue(pixel);
                int gruen = qGreen(pixel);

                //in y speichern wir das Luminanz-Signal/Helligkeitswert
                // Luminanz ignoriert die Farbanteile und holt die Helligkeitswerte aus dem Bild
                y = 0.299 * rot + 0.587 * gruen + 0.114 * blau;

                //wenn wir zum int casten, wird alles nach der Komma abgeschnitten,
                //d.h. abgerundet, deswegen addieren wir 0,5, um mathematisch korrekt zu runden
                y_round = (int)((y) + 0.5);

                if(y_round > obere_grenze) {
                    obere_grenze = y_round;
                }
                if(y_round < untere_grenze){
                    untere_grenze = y_round;
                }
            }
        }


        float min = 0.0;
        float max = 255.0;
            for(int column = 0 ; column < image_width; column++){
                for(int row = 0  ; row < image_height; row++){
                    QRgb pixel = image->pixel(column, row);
                    //Pro Pixel die einzelnen RGB-Werte rausfiltern
                    int rot = qRed(pixel);
                    int blau = qBlue(pixel);
                    int gruen = qGreen(pixel);

                    //ytoR = Hier muss Ycbcr in RGB umgerechnet werden;

                    //image-> setPixel(x,y, ....)
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



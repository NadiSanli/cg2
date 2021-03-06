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
        average_ref =0;
        variance_ref =0;
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

        /* --------------------------------------------- */
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

    //Konvertiert inputRGB zu YCBCR (Methode zur Vereinfachung, in 1a) nicht genutzt)
    QYcbcr convertToYcbcr(QRgb input){
        int rot = qRed(input);
        int gruen = qGreen(input);
        int blau = qBlue(input);

        float y = clamping(0.299f*rot + 0.587f*gruen + 0.114f*blau);
        float cb = clamping(-0.169f * rot - 0.331f* gruen + 0.5f * blau +128.0f);
        float cr = clamping(0.5f * rot - 0.419f * gruen - 0.081f* blau + 128.0f);

        return QYcbcr{y,cb,cr};
    }

    //Konvertiert inputYCBR zu RGB (Methode zur Vereinfachung)
    QRgb convertToRgb(QYcbcr input){
        float y = input.y;
        float cb = input.cb;
        float cr = input.cr;

        float r   = clamping(y - 0 * (cb - 128.0f) + 1.4f * (cr - 128.0f)); //rot
        float g = clamping(y - 0.34f * (cb - 128.0f) - 0.71f * (cr - 128.0f)); //grün
        float b  = clamping(y + 1.77f * (cb - 128.0f) + 0 * (cr - 128.0f)); //blau
        return qRgb(r,g,b);

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
            image = new QImage(*backupImage); //Verursacht ein coordinate out of range
            float anz_farbe=pow(2, newDynamicValue);
            float maxAmount = 256;
            for (int i = 0; i < image->width(); i++) {
                for (int j = 0; j < image->height(); j++) {
                    QYcbcr ycbcr = convertToYcbcr(image->pixel(i,j));
                    float ergebnis = ycbcr.y; //Auf Helligkeit zugreifen
                    ergebnis = ergebnis * (anz_farbe / maxAmount);
                    ergebnis = round(ergebnis);
                    ergebnis = ergebnis / (anz_farbe / maxAmount);
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
                image->setPixel(column, row, qRgb(rot,gruen,blau));
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

        image = new QImage(*backupImage);
        int image_width = image->width();
        int image_height = image->height();
        int max_pixel=0;
        int min_pixel=255;
        int mitte=0;
        int ergebnis=0.0;
        QYcbcr ycbcr;
        //wir finden den dunkelsten min_pixel und den hellsten max_pixel vorkommenden Helligkeitswert im Bild
        for (int i = 0; i < image->width(); i++) {
            for (int j = 0; j < image->height(); j++) {
                ycbcr = convertToYcbcr(image->pixel(i,j));
                ergebnis=(int) (ycbcr.y+0.5);
                max_pixel=std::max(max_pixel, ergebnis);
                min_pixel=std::min(min_pixel, ergebnis);
            }
        }
        /*Mitte der Helligkeitswerte berechnen*/
        mitte=round((max_pixel-min_pixel)/2.0);
        logFile << "Maximaler Helligkeitswert: " << max_pixel << " Minimaler Helligkeitswert: " << min_pixel << std::endl;

        //Mit doppelter for-schleife durch das ganze Bild iterieren, um die Helligkeitswerte der Pixel zu ändern
        for(int i = 0 ; i < image_width; i++){
            for(int j = 0  ; j < image_height; j++){

                ycbcr = convertToYcbcr(image->pixel(i,j));
                ergebnis = (int) (ycbcr.y + 0.5); //Auf Helligkeitswert zugreifen
                //Mitte in den Null verschieben, d.h. alle Werte verschieben
                ergebnis= ergebnis-mitte;
                //Der Helligkeitswert wird skaliert
                ergebnis = (int) (ergebnis * contrast_adjust_factor + 0.5); //+0,5 damit mathematisch richtig gerundet wird, wenn wir casten
                /*Mitte zurück verschieben*/
                ergebnis = ergebnis+mitte;
                //Clamping: Ausreißer auf 0 oder 255 setzen
                ergebnis=intClamping(ergebnis);
                ycbcr.y = ergebnis;
                image->setPixel(i,j, convertToRgb(ycbcr));
                //image->setPixel(i,j, qRgb(ycbcr.y, ycbcr.y, ycbcr.y));
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
        //image=new QImage(*backupImage);
        int image_width=image->width();
        int image_height=image->height();
        int sum_pixel=image_width*image_height;
        int cut_pixel_low=(int)(sum_pixel*plow +0.5); //Anzahl der Pixel, die wir am unteren Ende abschneiden
        int cut_pixel_high=(int)(sum_pixel*phigh +0.5);//Anzahl der Pixel, die wir am oberen Ende abschneiden
        int a_min=0;
        int a_max=255;
        int a_low_new=0; //neue untere Grenze
        int a_high_new=0; //neue obere Grenze

        int haeufigkeit_helligkeit[256];
        int y=0;

        for(int i=0; i<256; i++){
            haeufigkeit_helligkeit[i] =0;
        }
        QRgb pixel;
        //erstellen das kumulative Histogramm fürs Bild
        for (int i = 0; i < image->width(); i++) {
            for (int j = 0; j < image->height(); j++) {

                //Jeden Pixel in pixel speichern mit column/row
                pixel = image->pixel(i, j);

                //Pro Pixel die einzelnen RGB-Werte rausfiltern
                int rot = qRed(pixel);
                int blau = qBlue(pixel);
                int gruen = qGreen(pixel);

                //in y speichern wir das Luminanz-Signal/Helligkeitswert
                y = (int)(clamping(0.299 * rot + 0.587 * gruen + 0.114 * blau)+0.5);

                haeufigkeit_helligkeit[y]++;
            }
        }
        //Histogramm ausgeben
        for (int i = 0; i < 256; i++) {
            logFile <<  haeufigkeit_helligkeit[i] << std::endl;
        }
        logFile << "Pixel sum: "<< image->width()*image->height() << std::endl;
        logFile << "Cut_pixel_high: " << cut_pixel_high << std::endl;
        logFile << "Cut_pixel_low: "<< cut_pixel_low << std::endl;

        //Neue Skalierierungsgrenzen ermitteln, dafür zählen wir Pixel ab, die wir rauswerfen können
        int pixel_count_low=0;
        int pixel_count_high=0;
        int i=0;
        int j=255;

        while(pixel_count_low<=cut_pixel_low){
            pixel_count_low=pixel_count_low+haeufigkeit_helligkeit[i];
            i++;
        }
        a_low_new=i;

        while(pixel_count_high<=cut_pixel_high) {
            pixel_count_high=pixel_count_high+haeufigkeit_helligkeit[j];
            j--;
        }
        a_high_new=j;

        float ergebnis;
        QYcbcr ycbcr;
        //Robuste automatische Kontrastanpassung
        for (int i = 0; i < image->width(); i++) {
            for (int j = 0; j < image->height(); j++) {

                //Jeden Pixel in pixel speichern mit column/row
                ycbcr = convertToYcbcr(image->pixel(i,j));
                ergebnis =ycbcr.y;

                if(ergebnis<=a_low_new) {
                    ergebnis=a_min;
                } else if(ergebnis>a_low_new && ergebnis<a_high_new) {
                     ergebnis=(a_min+(ergebnis-a_low_new)*(float)(a_max-a_min)/(float)(a_high_new-a_low_new)+0.5);
                } else {
                    ergebnis=a_max;
                }

                ycbcr.y = clamping(ergebnis);
                image->setPixel(i,j, convertToRgb(ycbcr));
             }
        }
        logFile << "a_high_new: : " << a_high_new << std::endl;

        logFile << "a_low_new: : "<< a_low_new << std::endl;

        logFile << "Robust automatic contrast adjustment applied with:"<< std::endl << "---plow = " << (plow*100) <<"%" << std::endl << "---phigh = " << (phigh*100)<<"%" << std::endl;

        return image;
    }
}

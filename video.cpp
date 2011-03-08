/*
 * video.c
 *
 * Implementation of video.h functions
 *
 */

#include "video.h"
#include "math.h"

void filter(unsigned char *pixels, unsigned char *processedPixels, int numPixels) {

    int i;

    for(i = 0; i < numPixels; i+=3) {

        int r = pixels[i];
        int g = pixels[i + 1];
        int b = pixels[i + 2];

        int db = (b - 255) * (b - 255);
        int dg = (g * g);
        int dr = (r * r);

        double deltaSum = dr + dg + db;

        double distance = sqrt(deltaSum);

        if(distance > 150) {
            r = 0;
            g = 0;
            b = 0;
        }
        processedPixels[i] = (unsigned char)r;
        processedPixels[i+1] = (unsigned char)g;
        processedPixels[i+2] = (unsigned char)b;

    }
}



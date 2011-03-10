/*
 * video.c
 *
 * Implementation of video.h functions
 *
 */

#include "video.h"
#include "math.h"

#include <stdio.h>

void filter(unsigned char *pixels, unsigned char **processedPixels, unsigned int width, unsigned int height){

    int i;

    unsigned int totalPixels = sizeof(unsigned char) * width * height * 3;
    for(i = 0; i < totalPixels; i+=3) {

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
        int shift = i / 3;
        int row = shift / width;
        int col =  shift % width;

        processedPixels[row][col] = (unsigned char)b;
    }   
}



/*
 * video.c
 *
 * Implementation of video.h functions
 *
 */

#include "video.h"
#include "math.h"

#include <stdio.h>
#include <stdlib.h>

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
/*
 * blobs : an array to store the blobs in, 
 * numBlobs : the number of blobs to extract
 * blob labels : 2D array containing the blobs labels
 *             : each blob has unique label
 * width : width for the blob labels array
 * height : height for the blob labels array
 */

void extract_blobs(blob *blobs, int numBlobs, int **blobLabels,
        int width, int height) {

    
    int i, j;


    // create some room to store the blob points
    for(i = 0; i < numBlobs; i++) {
        // just create some room
        int defaultSize = 10;
        blobs[i].points = (coord *)malloc(sizeof(coord) * defaultSize);
        blobs[i].size = defaultSize;
        blobs[i].numPoints = 0;         
    }

    for(i = 0; i  < height; i++) {
      for(j = 0; j < width; j++) {
        int label = blobLabels[i][j];
        if(label < numBlobs && label >= 0) {
            int size = blobs[label].size;
            if((blobs[label].numPoints + 1) == size) {
                int newSize = blobs[label].size * 2;
                blobs[label].points = (coord*)realloc(blobs[label].points, sizeof(coord)* newSize);
                blobs[label].size = newSize;
            }
            coord *points = blobs[label].points;
            int numPoints = blobs[label].numPoints;
            points[numPoints].x = j;
            points[numPoints].y = i;

            blobs[label].numPoints++;
        }
      }   
    }
}
void draw_box(unsigned char *frame, int x, int y, int w, int h) {
	int i;
	int loc, loc1;
	int X1, X2, Y1, Y2;
	for(i = 0; i < w; i++) {
		X1 = max((x- w/2), 0);
		Y1 = max((y - h/2), 0);
		Y2 = min((y + h/2), 479);
		
		loc = ((640 * Y1) + (i + X1)) * 3;
		loc1 = ((640 * Y2) + (i + X1)) * 3;
		frame[loc] = (unsigned char) 255;
		frame[loc + 1] = (unsigned char) 255;
		frame[loc + 2] = (unsigned char) 255;
		
		frame[loc1] = (unsigned char) 255;
		frame[loc1 + 1] = (unsigned char) 255;
		frame[loc1 + 2] = (unsigned char) 255;
		
	}
	
	for(i = 0; i < h; i++) {
		
		Y1 = max((y- h/2), 0);
		X1 = max((x - w/2), 0);
		X2 = min((x + w/2), 639);
		
		
		loc = ((640 * (i + Y1)) + X1) * 3;
		loc1 = ((640 * (i + Y1)) + X2) * 3;
		
		frame[loc] = (unsigned char) 255;
		frame[loc + 1] =  (unsigned char) 255;
		frame[loc + 2] =  (unsigned char) 255;
		
		frame[loc1] = (unsigned char) 255;
		frame[loc1 + 1] =  (unsigned char) 255;
		frame[loc1 + 2] =  (unsigned char) 255;
	}
}



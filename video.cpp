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

/*---------------------------------------------  PRIVATE FUNCTIONS -----------------------------------------*/
double gradient(coord p1, coord p2);
double distance(coord p1, coord p2);
//bool findLongSide(coord refPoint, coord *point1, coord *point2, vector<coord> points, double errorRate);
/*----------------------------------------------------------------------------------------------------------*/

void filter(unsigned char *pixels, unsigned char **processedPixels, unsigned int width, unsigned int height){

    int i;

    unsigned int totalPixels = sizeof(unsigned char) * width * height * 3;
    for(i = 0; i < totalPixels; i+=3) {

        int r = pixels[i];
        int g = pixels[i + 1];
        int b = pixels[i + 2];

        int db = (b - 255) * (b - 255);
        int dg = (g - 255) * (g - 255);
        int dr = (r - 255) * (r - 255);

        double deltaSum = dr + dg + db;

        double distance = sqrt(deltaSum);
        
       // printf("%d %d %d %f\n", r, g, b, distance);
        if(distance > 10) {
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

int get_shape(blob *blobs, int numBlobs, shape shp) {

	int i;
	
	coord *centerCoords = (coord*)malloc(sizeof(coord) * numBlobs);	
	for(i = 0; i < numBlobs; i++) {
		//centerCoords[i] = get_blob_center(blobs[i]);	
		//printf("(%d,%d) ", centerCoords[i].x, centerCoords[i].y);	
		printf("%d ", blobs[i].numPoints);
	}
	printf("\n");
	
	// free memory
	free(centerCoords); 
		
	return 0;
}

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
int apply_blob_size_heuristic(blob *blobs, int numBlobs) {
	int i;
	for(i = 0; i < numBlobs; i++) {
		// numbers choosen arbitrarily from analysis
		if(blobs[i].numPoints < 20 || blobs[i].numPoints > 450) {
			// remove this small/big blob
			free(blobs[i].points);
			
			blobs[i].points = blobs[numBlobs - 1].points;
			blobs[i].numPoints = blobs[numBlobs - 1].numPoints;
			blobs[i].size = blobs[numBlobs - 1].size;
			
			numBlobs--;   			
			i--;
			
		} 		  		
	}
	return numBlobs;	 
}
void free_blobs(blob *blobs, int numBlobs) {
	int i;
	 // free the points first
	 for(i = 0; i < numBlobs; i++) {       
        free(blobs[i].points);       
    }
    free(blobs);
}

coord get_blob_center(blob bl) {

    int i;

    int xSum, ySum;
    xSum = ySum = 0;

    coord *points = bl.points;
    
    for(i = 0; i < bl.numPoints; i++) {
        xSum += points[i].x;
        ySum += points[i].y;
    }
    printf("%d %d %d\n", xSum, ySum, bl.numPoints);
    int xCenter = (int)(xSum / bl.numPoints);
    int yCenter = (int)(ySum / bl.numPoints);

    coord center = {xCenter, yCenter};

    return center;
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



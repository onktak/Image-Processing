/*
 * video.c
 *
 * Implementation of video.h functions
 *
 */

#include "video.h"

#include <math.h>
#include <limits.h>

#include <stdio.h>
#include <stdlib.h>

/*---------------------------------------------  PRIVATE FUNCTIONS -----------------------------------------*/
double gradient(coord p1, coord p2);
double distance(coord p1, coord p2);
/*
 * return 1 if long side with 3 collinear points and distance ratio of 1:2 is found else 0
 */
int findLongSide(coord refPoint, coord *point1, coord *point2, coord *points, int numPoints, double errorRate);
/*
 * return 1 if side with 3 collinear points and distance ratio of 1:1 is found else 0
 */
int findShortSide(coord refPoint, double dist, coord *point1, coord *point2, 
						  coord *points, int numPoints, double errorRate);
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

int get_shape(blob *blobs, int numBlobs, shape *sh) {

	int i;
	
	coord *centerCoords = (coord*)malloc(sizeof(coord) * numBlobs);	
	for(i = 0; i < numBlobs; i++) {
		centerCoords[i] = get_blob_center(blobs[i]);	
		//printf("(%d,%d) ", centerCoords[i].x, centerCoords[i].y);	
		
	}
	double PERCENTAGE_ERROR = 0.1;
	
	coord center, top, bottom, left, right;
	int shapeFound = 0;
	int count = 0;
	int num = 0;
	for(i = 0; i < numBlobs; i++) {
		center = centerCoords[i];		
		int longSideFound = findLongSide(center, &top, &bottom, centerCoords, numBlobs, PERCENTAGE_ERROR);
		if(longSideFound) {		
			
			// find the short segment				
			double d =  distance(center, top);
		
			int shortSideFound = findShortSide(center, d, &left, &right, centerCoords, numBlobs, PERCENTAGE_ERROR);
			if(shortSideFound) {
				printf("shape found----------------------!!!!!\n");
				shapeFound = 1;	
				break;		
			}
		} 
	}
	// free memory
	free(centerCoords); 
		
	if(shapeFound) {
			
		sh->head.x = top.x; sh->head.y = top.y;
		sh->center.x = center.x; sh->center.y = center.y;
		sh->tail.x = bottom.x; sh->tail.y = bottom.y;
		sh->left.x = left.x; sh->left.y = left.y;
		sh->right.x = right.x; sh->right.y = right.y;
		
		return 1;
		
	} else {
		return 0;
	}
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
		if(blobs[i].numPoints < 20) {
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

int findLongSide(coord refPoint, coord *point1, coord *point2, coord *points, int numPoints, double errorRate) {

    coord p1, p2;
    int j, k;
	for(j = 0; j < numPoints; j++) {	
		p1 = points[j];
		if(p1.x == refPoint.x && p1.y == refPoint.y) {
			continue;
		} else {				
			double d1 = distance(refPoint, p1);
			double m1 = gradient(refPoint, p1);
			// find another point with gradient = m1 and distance = 2 * d1 from refPoint				
			for(k = 0; k < numPoints; k++) {
				p2 = points[k];
				double d2 = distance(refPoint, p2);
				double distanceSum = d1 + d2;
				
				// account for some error
				if(fabs(distanceSum - distance(p1, p2)) <= fabs(errorRate * distanceSum)) {						

					double m2 = gradient(refPoint, p2);
					if(fabs(m1 - m2) <= fabs(errorRate * m1)) {						
						
						if(fabs(d1 - (2 * d2)) <= fabs(errorRate * d1)) {																
							// shorter distance always first
							point1->x = p2.x; point1->y = p2.y;
							point2->x = p1.x; point2->y = p1.y;
							
							return 1;
							
						} else if(fabs((2 * d1) - d2) <= fabs(errorRate * d2)) {
							point1->x = p1.x; point1->y = p1.y;
							point2->x = p2.x; point2->y = p2.y;
							
							return 1;
						}
					}
				}				
			}

		} // end else
	}
	return 0;
}
int findShortSide(coord refPoint, double dist, coord *point1, coord *point2, 
						  coord *points, int numPoints, double errorRate) {

    coord p1, p2;
    int j, k;
	for(j = 0; j < numPoints; j++) {	
		p1 = points[j];
		if(p1.x == refPoint.x && p1.y == refPoint.y) {
			continue;
		} else {				
			double d1 = distance(refPoint, p1);
			double m1 = gradient(refPoint, p1);
			// find another point with gradient = m1 and distance = d1 from refPoint				
			for(k = 0; k < numPoints; k++) {
				p2 = points[k];
				if((p2.x == refPoint.x && p2.y == refPoint.y) || (p2.x == p1.x && p2.y == p1.y)) {
					continue;
				} else {
					double d2 = distance(refPoint, p2);				
				
					// make sure the points are collinear
					double m2 = gradient(refPoint, p2);
					if(fabs(m1 - m2) <= fabs(errorRate * m1)) {						
					
						if((fabs(d1 - dist) <= fabs(errorRate * d1)) && (fabs(d2 - dist) <= fabs(errorRate * d2))) {			
							point1->x = p1.x; point1->y = p1.y;
							point2->x = p2.x; point2->y = p2.y;
					
							return 1;							
						}						
					}	
				}			
			}
		} // end else
	}
	return 0;

}
double gradient(coord p1, coord p2) {
	int deltaY = p1.y - p2.y;
	int deltaX = p1.x - p2.x;

	if(deltaX == 0) {
		return INT_MAX;
	} else {
		return (deltaY * 1.0 / deltaX * 1.0);
	}	
}
double distance(coord p1, coord p2) {
	double deltaY = p1.y - p2.y;
	double deltaX = p1.x - p2.x;

	return (sqrt((deltaY * deltaY) + (deltaX * deltaX)));

}


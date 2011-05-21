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

/*----------------------------------------------------------------------------------------------------------*/

void convert_grayscale_to_rgb(unsigned char *pixels, unsigned char processedPixels[][FRAME_WIDTH], unsigned int width, 
 		unsigned int height) {
 		
	int i;
    for(i = 0; i < height *  width * 3; i+=3) {
        int shift = i / 3;            	
        int row = shift / width;
        int col = shift % width;

        pixels[i] = 0; // red;
        pixels[i + 1] = 0; // green
        pixels[i + 2] = processedPixels[row][col];
    } 		
 }

void filter(unsigned char *pixels, unsigned char processedPixels[][FRAME_WIDTH], unsigned int width, unsigned int height){

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

void print_point(coord co) {
	printf("(%d,%d) ", co.x, co.y);
}


int find_collinear(coord refPoint, coord *point1, coord *point2, coord *points, int numPoints, double errorRate) {

	coord p1, p2;
    int j, k;
	for(j = 0; j < numPoints; j++) {	
		p1 = points[j];
		if(p1.x == refPoint.x && p1.y == refPoint.y) {
			continue;
		} else {			
			double m1 = gradient(refPoint, p1);
			// find another point with gradient = m1 			
			for(k = 0; k < numPoints; k++) {
				p2 = points[k];
				if((p2.x == refPoint.x && p2.y == refPoint.y) || (p2.x == p1.x && p2.y == p1.y)) {
					continue;
				} else {						
					double m2 = gradient(refPoint, p2);				
					if(fabs(m1 - m2) <= 0.04) {	
						point1->x = p1.x; point1->y = p1.y;
						point2->x = p2.x; point2->y = p2.y;							
						return 1;
					}
				}						
			}

		} // end else
	}
	return 0;
}
int collinear_contains(collinear co, coord c) {
	if(co.point1.x == c.x && co.point1.y == c.y) {
		return 1;
	}
	if(co.point2.x == c.x && co.point2.y == c.y) {
		return 1;
	}
	if(co.point3.x == c.x && co.point3.y == c.y) {
		return 1;
	}
	return 0;
}
int collinear_already_added(collinear co, collinear *points, int numPoints) {
	int i;
	for(i = 0; i < numPoints; i++ ) {
		collinear c = {points[i].point1, points[i].point2, points[i].point3};
		if(collinear_contains(c, co.point1) && collinear_contains(c, co.point2) && 
			collinear_contains(c, co.point3)) {
				return 1;
		}
	}
	return 0;
}

int compare_points(coord c1, coord c2) {
	if(c1.x > c2.x) {
		return 1;
	}
	if(c1.x == c2.x) {
		if(c1.y > c2.y){
			return 1;
		}
		if(c1.y == c2.y) {
			return 0;
		}
	}
	return -1;
}
coord get_smallest_coord(collinear *co) {
	coord p1 = co->point1;
	coord p2 = co->point2;
	coord p3 = co->point3;
	
	coord small;
	if(compare_points(p1, p2) == 1) {
		small.x = p2.x; small.y = p2.y;
	} else {
		small.x = p1.x; small.y = p1.y;
	}
	if(compare_points(small, p3) == 1) {
		small.x = p3.x; small.y = p3.y;
	}
	return small;
}
coord get_largest_coord(collinear *co) {
	coord p1 = co->point1;
	coord p2 = co->point2;
	coord p3 = co->point3;
	
	coord big;
	if(compare_points(p1, p2) == 1) {
		big.x = p1.x; big.y = p1.y;
	} else {
		big.x = p2.x; big.y = p2.y;
	}
	if(compare_points(big, p3) == -1) {
		big.x = p3.x; big.y = p3.y;
	}
	return big;
}
void order_coords(collinear *co) {

	coord small = get_smallest_coord(co);
	coord big = get_largest_coord(co);
	coord middle ;
	
	int isOne, isTwo, isThree;
	isOne = isTwo = isThree = 0;
	if(co->point1.x == small.x && co->point1.y == small.y || co->point1.x == big.x && co->point1.y == big.y) {
		isOne = 1;
	}
	if(co->point2.x == small.x && co->point2.y == small.y || co->point2.x == big.x && co->point2.y == big.y) {
		isTwo = 1;
	}
	if(co->point3.x == small.x && co->point3.y == small.y || co->point3.x == big.x && co->point3.y == big.y) {
		isThree = 1;
	}
	if(!isOne) {
		middle = co->point1;
	}
	if(!isTwo) {
		middle = co->point2;
	}
	if(!isThree) {
		middle = co->point3;
	}
	co->point1 = small;
	co->point2 = middle;
	co->point3 = big;
}

int get_more_straight_sides(coord centerCoords[MAX_BLOBS], int numBlobs, collinear linear[MAX_BLOBS]) {
	
	int i; 
	
	double PERCENTAGE_ERROR = 0.05;
	
	coord point1, point2, point3;
	int num = 0;
	int size = numBlobs;
	for(i = 0; i < numBlobs; i++) {
		point1 = centerCoords[i];		
		int found = find_collinear(point1, &point2, &point3, centerCoords, numBlobs, PERCENTAGE_ERROR);
		if(found) {		
			collinear co = {point1, point2, point3};
			order_coords(&co);
			if(!collinear_already_added(co, linear, num)) {
				
				// remove points which do not have 2:1 or 1:1 ratio distance between
				
				double d1 = distance(co.point1, co.point2);
				double d2 = distance(co.point2, co.point3); 
				
				double big = max(d1, d2);
				double small = min(d1, d2);
				
				int ratio = (int)round(big/small);
				// only consider points with ratio 2:1 or 1:1
				if(ratio == 2 || ratio == 1) {
				
					linear[num].point1.x = co.point1.x;
					linear[num].point2.x = co.point2.x;
					linear[num].point3.x = co.point3.x;
			
					linear[num].point1.y = co.point1.y;
					linear[num].point2.y = co.point2.y;
					linear[num].point3.y = co.point3.y;
			
					num++;	
				}
		   }	
		} 
	}
	return num;	
}
int get_short_side(coord* centerCoords, int numCenters, collinear linear, collinear *foundPoints) {
	
	int i, j;
	// check if this collinear points have a corresponding short or long side 
	for(i = 0; i < numCenters; i++) {
		coord p1 = centerCoords[i];
		int alreadyIn = collinear_contains(linear, p1);
		if(!alreadyIn) {
			double d1 = distance(p1, linear.point2);
			for(j = 0; j < numCenters; j++) {
				coord p2 = centerCoords[j];
				alreadyIn = collinear_contains(linear, p2);
				if(!alreadyIn && compare_points(p1, p2) != 0) {
					double d2 = distance(p2, linear.point2);
					int ratio = (int)round(d2/d1);
					
					if(ratio == 1) {
					
						foundPoints->point1 = p1;
						foundPoints->point2 = linear.point2;
						foundPoints->point3 = p2;
						return 1;
					}
				}								
			}	
		}	
	}
	return 0;
}
int get_long_side(coord* centerCoords, int numCenters, collinear linear, collinear *foundPoints) {
	
	int i, j;
	// check if this collinear points have a corresponding short or long side 
	for(i = 0; i < numCenters; i++) {
		coord p1 = centerCoords[i];
		int alreadyIn = collinear_contains(linear, p1);
		if(!alreadyIn) {
			double d1 = distance(p1, linear.point2);
			for(j = 0; j < numCenters; j++) {
				coord p2 = centerCoords[j];
				alreadyIn = collinear_contains(linear, p2);
				if(!alreadyIn && compare_points(p1, p2) != 0) {
					double d2 = distance(p2, linear.point2);
					
					double big = max(d1, d2);
					double small = min(d1, d2);
					int ratio = (int)round(big/small);
					
					if(ratio == 2) {
					
						foundPoints->point1 = p1;
						foundPoints->point2 = linear.point2;
						foundPoints->point3 = p2;
						return 1;
					}
				}								
			}	
		}	
	}
	return 0;
}
void get_blob_centers(blob blobs[MAX_BLOBS], int numBlobs, coord centerCoords[MAX_BLOBS]) {
	int i;
	for(i = 0; i < numBlobs; i++) {
		centerCoords[i] = get_blob_center(blobs[i]);
	}
}

void extract_blobs(blob blobs[MAX_BLOBS], int numBlobs, int blobLabels[][FRAME_WIDTH], 
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
int apply_blob_size_heuristic(blob blobs[MAX_BLOBS], int numBlobs) {
	int i;
	for(i = 0; i < numBlobs; i++) {
		// numbers choosen arbitrarily from analysis
		if(blobs[i].numPoints < 20 || blobs[i].numPoints > 2500) {
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
    //free(blobs);
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
		
		loc = ((FRAME_WIDTH * Y1) + (i + X1)) * 3;
		loc1 = ((FRAME_WIDTH * Y2) + (i + X1)) * 3;
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
		
		
		loc = ((FRAME_WIDTH * (i + Y1)) + X1) * 3;
		loc1 = ((FRAME_WIDTH * (i + Y1)) + X2) * 3;
		
		frame[loc] = (unsigned char) 255;
		frame[loc + 1] =  (unsigned char) 255;
		frame[loc + 2] =  (unsigned char) 255;
		
		frame[loc1] = (unsigned char) 255;
		frame[loc1 + 1] =  (unsigned char) 255;
		frame[loc1 + 2] =  (unsigned char) 255;
	}
}

int is_long_side(collinear co) {
	// long side has ration 2:1 between points
	double d1 = distance(co.point1, co.point2);
	double d2 = distance(co.point2, co.point3); 		
	double big = max(d1, d2);
	double small = min(d1, d2);		
	int ratio = (int)round(big/small);
	
	if(ratio == 2) {
		return 1;
	} else {
		return 0;
	}	
}
int is_short_side(collinear co) {
	// long side has ration 1:1 between points
	double d1 = distance(co.point1, co.point2);
	double d2 = distance(co.point2, co.point3); 		
	double big = max(d1, d2);
	double small = min(d1, d2);		
	int ratio = (int)round(big/small);
	
	if(ratio == 1) {
		return 1;
	} else {
		return 0;
	}	
}
double gradient(coord p1, coord p2) {
	int deltaY = p1.y - p2.y;
	int deltaX = p1.x - p2.x;

	if(deltaX == 0) {
		return INT_MAX;
	} else {
		return ((deltaY * 1.0) / (deltaX * 1.0));
	}	
}
double distance(coord p1, coord p2) {
	double deltaY = p1.y - p2.y;
	double deltaX = p1.x - p2.x;

	return (sqrt((deltaY * deltaY) + (deltaX * deltaX)));
}

void print_image(coord centerCoords[MAX_BLOBS], int numCoords, coord shapeCoords[5], int numShapes) {
	double scale_x = FRAME_WIDTH / 64.0;
	double scale_y = FRAME_HEIGHT / 32.0;
	
	int i, j;
	int cols = 64;
	int blobs[32][cols];
	for(i = 0; i < 32; i++) {
		for(j = 0; j < cols; j++) {
			blobs[i][j] = 0;
		}
	}
	for(i = 0; i < numCoords; i++) {
        int x = round(centerCoords[i].x * 1.0 / scale_x);
        int y = round(centerCoords[i].y * 1.0 / scale_y);
        
        if(x > 0 && x < cols && y > 0 && y < 32) {
        	int found = 0;
        	for(j = 0; j < 5; j++) {
        		if(centerCoords[i].x == shapeCoords[j].x && centerCoords[i].y == shapeCoords[j].y ) {
    				found = 1;
    				break;
    			}
        	}
        	if(found) {
        		blobs[y][x] = 2;
        	} else {
        		blobs[y][x] = 1;
        	}
        }
    }
    printf("\n");
    printf("\t*");
    for(i = 0; i < cols; i++){
    	printf("-");
    }
    printf("*\n");
    for(i = 0; i < 32; i++) {
    	printf("\t|");
		for(j = 0; j < cols; j++) {
			if(blobs[i][j] == 0) {
				printf(" ");
			} else if(blobs[i][j] == 1){
				printf(".");
			} else {
				printf("x");
			}
		}
		printf("|\n");
	}
	printf("\t*");
    for(i = 0; i < cols; i++){
    	printf("-");
    }
    printf("*\n");
}

double distance_from_center(coord shapeCenter) {
	
	int imageCenterX = FRAME_WIDTH / 2;
	int imageCenterY = FRAME_HEIGHT / 2;	
	
	coord center;
	center.x = imageCenterX;
	center.y = imageCenterY;
	
	return distance(shapeCenter, center);
	
}

int quadrant(coord c1) {

	int imageCenterX = FRAME_WIDTH / 2;
	int imageCenterY = FRAME_HEIGHT / 2;	
	
	coord center;
	center.x = imageCenterX;
	center.y = imageCenterY;
	
	// 1st or 2nd
	if(c1.y >= center.y) {
		if(c1.x <= center.x) {
			return 1;
		} else {
			return 2;
		}
		
	} else {
		// 3rd or 4th
		if(c1.x <= center.x) {
			return 4;
		} else {
			return 3;
		}
		
	}
	
}

double angle(coord p1, coord p2) {
	double deltaY = p1.y - p2.y;
	double deltaX = p1.x - p2.x;
	
	if(deltaX == 0) {
		return 90.0;
	} else {
		double angle = atan(deltaY / deltaX) * 180 / PI;
		return fabs(angle);
	}		
}
double quadrant_angle(coord p1) {

	int quad = quadrant(p1);

	int imageCenterX = FRAME_WIDTH / 2;
	int imageCenterY = FRAME_HEIGHT / 2;	
	
	coord center;
	center.x = imageCenterX;
	center.y = imageCenterY;
	
	double ang = angle(p1, center);
	
	printf("quadrant %d angle %f\n\n", quad, ang);
	
	double quadrantAngle  = ang;
	if(quad == 2) {
		quadrantAngle += 90;
	} else if(quad == 3) {
		quadrantAngle += 180;
	} else if(quad == 4) {
		quadrantAngle += 270;
	}
	return 	quadrantAngle;
}
void print_direction(double angle) {
	int i, j;
	char matrix[5][5] = {
		{' ',' ',' ',' ',' '},
		{' ',' ',' ',' ',' '},
		{' ',' ',' ',' ',' '},
		{' ',' ',' ',' ',' '},
		{' ',' ',' ',' ',' '}
	};
	int ang = (int)angle;
	if((ang % 90) == 0)  {
		if(ang == 180 || ang == 360) {
			for(i = 0; i < 5; i++) {
				matrix[2][i] = '.';
			}
			if(ang == 180) {
				matrix[2][0] = 'x';
			}
			if(ang == 360) {
				matrix[2][4] = 'x';
			}
		} else {
			for(i = 0; i < 5; i++) {
				matrix[i][2] = '.';
			}
			if(ang == 90) {
				matrix[0][2] = 'x';
			}
			if(ang == 270) {
				matrix[4][2] = 'x';
			}
		}

	} else {
		if(ang < 90 || (ang > 180 && ang < 270)) {
			for(i = 0; i < 5; i++) {
				matrix[4 - i][i] = '.';
			}
			if(ang < 90) {
				matrix[4][0] = 'x';
			} else {
				matrix[0][4] = 'x';
			}
		} else {
			for(i = 0; i < 5; i++) {
				matrix[i][i] = '.';
			}
			if(ang < 180) {
				matrix[4][4] = 'x';
			} else {
				matrix[0][0] = 'x';
			}	
		}	
	}
	printf("\n");
	for(i = 0; i < 5; i++) {
		for(j = 0; j < 5; j++) {
			printf("%c", matrix[i][j]);
		}
		printf("\n");
	}
	printf("\n");	
}



/*
 *
 *  This module processes video frames to identify blobs of a given colour, red
 *  locate them and find the blobs forming a cross. The cross identified is used
 *  to determine how high and how far off the quadrotor is from the landing guidance
 *  system.
 *
 */


#ifndef VIDEO_H_
#define VIDEO_H_

#include "constants.h"

#define max(a, b)  (((a) > (b)) ? (a) : (b)) 
#define min(a, b)  (((a) < (b)) ? (a) : (b))

#define PI 3.141593

typedef struct {
    int x;
    int y;
} coord;

typedef struct {
	int x;
	int y;
	int z;
} coord3D;

typedef struct {
    // arrray to store the number of points 
    // making the blob
    coord *points;  
    int numPoints;
    int size; // keep track of memory allocated to blob
} blob;

typedef struct {
	coord point1;
	coord point2;
	coord point3;	
} collinear;

/*
 * stores the coordinates of the 'cross' shape used to identify the pattern
 */
typedef struct {
 	collinear shortLine;
 	collinear longLine;
} cross;

/* 
 * convert a 2D array of grayscale image to a linear array of RGB format
 * the grayscale represents varying blue color
 */
 void convert_grayscale_to_rgb(unsigned char *pixels, unsigned char processedPixels[][FRAME_WIDTH], unsigned int width, 
 		unsigned int height);
 

/*
 * this function filters all the other colors except variations of red
 *
 * pixels : RGB pixels of the image
 *
 * processedPixels : stores the processed pixels in grayscale format with blue and green pixels thrown away.
 *
 */
void filter(unsigned char *pixels, unsigned char processedPixels[][FRAME_WIDTH], unsigned int width, unsigned int height);

/*
 * this function retrieves the 'cross' shape from the blobs 
 *
 * return 1 if the shape is found else 0
 *
 * shp : the shape struct to store the cross coordinates *
 *
 */
/* int get_shape(blob *blobs, int numBlobs, shape *shp); */

/*
 * blobs : an array to store the blobs in, 
 * numBlobs : the number of blobs to extract
 * blob labels : 2D array containing the blobs labels
 *             : each blob has unique label
 * width : width for the blob labels array
 * height : height for the blob labels array
 */

void extract_blobs(blob blobs[MAX_BLOBS], int numBlobs, int blobLabels[][FRAME_WIDTH], int width, int height);

/*
 * this function removes blobs that are too big or too small 
 *
 * return the number of blobs left with the blobs array updated correctly.
 */
 int apply_blob_size_heuristic(blob blobs[MAX_BLOBS], int numBlobs); 

/*
 *  return the center coordinates of a blob
 */
coord get_blob_center(blob blob);

/*
 * free memory used by the blobs
 */ 
 void free_blobs(blob blobs[MAX_BLOBS], int numBlobs);

void draw_box(unsigned char *frame, int x, int y, int w, int h);


void get_blob_centers(blob blobs[MAX_BLOBS], int numBlobs, coord centerCoords[FRAME_ALL_PIXELS]);

int get_more_straight_sides(coord centerCoords[MAX_BLOBS], int numCenters, collinear linear[MAX_BLOBS]);

int get_short_side(coord centerCoords[MAX_BLOBS], int numCenters, collinear linear, collinear *foundPoints); 
int get_long_side(coord centerCoords[MAX_BLOBS], int numCenters, collinear linear, collinear *foundPoints); 

void print_point(coord co);
double gradient(coord p1, coord p2);
double distance(coord p1, coord p2);

int is_long_side(collinear co);
int is_short_side(collinear co);

void print_image(coord centerCoords[MAX_BLOBS], int numCoords, coord shapeCoords[5], int numShapes);

double distance_from_center(coord shapeCenter);
double quadrant_angle(coord p1);
void print_direction(double angle);

#endif /* VIDEO_H_ */


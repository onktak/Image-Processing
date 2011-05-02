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

#define max(a, b)  (((a) > (b)) ? (a) : (b)) 
#define min(a, b)  (((a) < (b)) ? (a) : (b))

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
 * this function filters all the other colors except variations of red
 *
 * pixels : RGB pixels of the image
 *
 * processedPixels : stores the processed pixels in grayscale format with blue and green pixels thrown away.
 *
 */
void filter(unsigned char *pixels, unsigned char **processedPixels, unsigned int width, unsigned int height);

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

void extract_blobs(blob *blobs, int numBlobs, int **blobLabels, int width, int height);

/*
 * this function removes blobs that are too big or too small 
 *
 * return the number of blobs left with the blobs array updated correctly.
 */
 int apply_blob_size_heuristic(blob *blobs, int numBlobs); 

/*
 *  return the center coordinates of a blob
 */
coord get_blob_center(blob blob);

/*
 * free memory used by the blobs
 */ 
 void free_blobs(blob *blobs, int numBlobs);

void draw_box(unsigned char *frame, int x, int y, int w, int h);

int get_collinear_points(blob *blobs, int numBlobs, collinear** linear);
int get_intersecting_collinear_points( collinear** linear, int numPoints, cross *crosses);
//int get_crosses( collinear** linear, int numPoints, cross *crosses); 


void get_blob_centers(blob *blobs, int numBlobs, coord* centerCoords);
int get_more_straight_sides(coord* centerCoords, int numCenters, collinear* linear);
int get_short_side(coord* centerCoords, int numCenters, collinear linear, collinear *foundPoints); 
int get_long_side(coord* centerCoords, int numCenters, collinear linear, collinear *foundPoints); 

void print_point(coord co);
double gradient(coord p1, coord p2);
double distance(coord p1, coord p2);

int is_long_side(collinear co);
int is_short_side(collinear co);

#endif /* VIDEO_H_ */


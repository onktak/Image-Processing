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
    // arrray to store the number of points 
    // making the blob
    coord *points;  
    int numPoints;
    int size; // keep track of memory allocated to blob
} blob;


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
 * blobs : an array to store the blobs in, 
 * numBlobs : the number of blobs to extract
 * blob labels : 2D array containing the blobs labels
 *             : each blob has unique label
 * width : width for the blob labels array
 * height : height for the blob labels array
 */

void extract_blobs(blob *blobs, int numBlobs, int **blobLabels, int width, int height);

void draw_box(unsigned char *frame, int x, int y, int w, int h);


#endif /* VIDEO_H_ */


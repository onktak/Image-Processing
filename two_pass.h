/*********************************************************************************
 * Implementation of the two-pass algorithm for connected component labelling. 
 * Used for detecting blobs in the image.
 *
 *********************************************************************************/

/*
 * Identifies connected components in the image pixels. The background pixels are 
 * black
 *
 * pixels : image pixels
 * labels : 2D array to store the labels of each non background pixel. 
 * width  : image width
 * height : image height
 *
 */

void two_pass(unsigned char **pixels, int **labels, int width, int height);

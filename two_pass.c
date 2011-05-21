/****************************************************************************
 * Name        : two_pass.c
 * Author      :
 * Version     :
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include "union_find.h"
#include "two_pass.h"

/*--------------------------------------- PRIVATE PROTOTYPES ------------------------------------------*/

/*
 * returns the number of blobs before continuous regions are merged
 */
int first_pass(unsigned char pixels[][FRAME_WIDTH], int labels[][FRAME_WIDTH], int width, int height, long *linked);
void second_pass(int labels[][FRAME_WIDTH], int width, int height, long *linked);

/*
 * return the number of blobs found
 *
 * implementation of the two-pass algorithm for blob detection
 * Utilises 8-connectivity to determine neighbours.
 *
 */

int two_pass(unsigned char pixels[][FRAME_WIDTH], int labels[][FRAME_WIDTH], int width, int height) {

    long *linked = (long *)malloc(width * height * sizeof(long));


    // go first pass
    int numBeforeMerge = first_pass(pixels, labels, width, height, linked);
    // second pass
    second_pass(labels, width, height, linked);
    
    int i, j;
    int *blobLabels = (int *)malloc( numBeforeMerge * sizeof(int));
    int labelsAdded = 0;
    for(i = 0; i < numBeforeMerge; i++) {
    	int found = 0;
    	for(j = 0; j < labelsAdded; j++) {
    		if(blobLabels[j] == i) {
    			found = 1;
    			break;
    		}
    	}
    	if(!found) {
    		blobLabels[labelsAdded] = i;
    		labelsAdded++;
    	}
    }
    
    free(blobLabels);
    free(linked);
    
    return labelsAdded;

}

int first_pass(unsigned char pixels[][FRAME_WIDTH], int labels[][FRAME_WIDTH], int width, int height, long *linked) {

    int i, j;
    // first pass
    int nextLabel = 0;
    for(i = 0; i < height; i++) {
        for(j = 0; j < width; j++) {

            // not background
            if(pixels[i][j] != 0) {
                unsigned int neighbourLabels[4];
                int num = 0;
                if(j > 0) {
                    // west
                    if(pixels[i][j - 1] != 0) {
                        neighbourLabels[num] = labels[i][j - 1];
                        num++;
                    }
                    if(i > 0) {
                        // north west
                        if(pixels[i - 1][j - 1] != 0) {
                            neighbourLabels[num] = labels[i - 1][j - 1];
                            num++;
                        }
                    }
                }
                if(i > 0) {
                    // north
                    if(pixels[i - 1][j] != 0) {
                        neighbourLabels[num] = labels[i - 1][j];
                        num++;
                    }
                    if(j < (width -1)) {
                        // north East
                        if(pixels[i - 1][j + 1] != 0) {
                            neighbourLabels[num] = labels[i - 1][j + 1];
                            num++;
                        }
                    }
                }
                // all neighbours blank
                if(num == 0) {
                    labels[i][j] = nextLabel;
                    // update the list to reflect in which label the pixel is in.
                    linked[nextLabel] = makeset(nextLabel);

                    nextLabel++;

                } else {
                    int minLabel = INT_MAX;
                    int k;
                    for(k = 0; k < num; k++) {
                        if(neighbourLabels[k] < minLabel) {
                            minLabel = neighbourLabels[k];
                        }
                    }
                    // assign to the minimum of the neighbours
                    if(minLabel != INT_MAX) {
                        
                        labels[i][j] = minLabel;
                        // regroup nodes next to each other
                      
                        for(k = 0; k < num; k++) {
                            unsigned int label = neighbourLabels[k];
                            if(linked[label] != linked[minLabel]) {
                                //printf("merging\n");
                                Union(linked[minLabel], linked[label]);
                                // update all labels that were pointing to same data structure as linked[label]
                                int l;
                                long copy = linked[label];
                                for(l = 0; l < nextLabel; l++) {
                                    if(linked[l] == copy) {
                                        linked[l] = linked[minLabel];
                                    }
                                }
                            }
                        }                        
                    }

                }
            } 
        }
    } 
 	return nextLabel;
    
}

void second_pass(int labels[][FRAME_WIDTH], int width, int height, long *linked) {

    int i, j;
    // second pass
    for(i = 0; i < height; i++) {
        for(j = 0; j < width; j++) {
            if(labels[i][j] != -1) {
                long minLabelAddr = head(linked[labels[i][j]]);
                labels[i][j] = nodeValue(minLabelAddr);
            }
        }
    }
}



/****************************************************************************
 * Name        : twoPass.cpp
 * Author      :
 * Version     :
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include "union_find.h"

/*--------------------------------------- PRIVATE PROTOTYPES ------------------------------------------*/
void first_pass(unsigned char **pixels, int **labels, int width, int height, long *linked);
void second_pass(unsigned char **pixels, int **labels, int width, int height, long *linked);

/*
 * implementation of the two-pass algorithm for blob detection
 * Utilises 8-connectivity to determine neighbours.
 *
 */

void two_pass(unsigned char **pixels, int **labels, int width, int height) {

    long *linked = (long *)malloc(width * height * sizeof(long));


    // go first pass
    first_pass(pixels, labels, width, height, linked);
    // second pass
    second_pass(pixels, labels, width, height, linked);

    free(linked);

}

void first_pass(unsigned char **pixels, int **labels, int width, int height, long *linked) {

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
                    ListSet<int> newUnionList;
                    linked[nextLabel] = newUnionList.makeset(nextLabel);

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
                        ListSet<int> newUnionList;

                        for(k = 0; k < num; k++) {
                            unsigned int label = neighbourLabels[k];
                            if(linked[label] != linked[minLabel]) {
                                //printf("merging\n");
                                newUnionList.Union(linked[minLabel], linked[label]);
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
    
    for(i = 0; i < nextLabel; i++) {
        ListSet<int> newUnionList;
        long start = newUnionList.head(linked[i]);
        long total  = 0;
        while(start != 0) {
            total++;
            start = newUnionList.nextNode(start);
        }
        printf("(%d, %ld)  \n", i, total);

    }
    
}

void second_pass(unsigned char **pixels, int **labels, int width, int height, long *linked) {

    int i, j;
    // second pass
    for(i = 0; i < height; i++) {
        for(j = 0; j < width; j++) {
            if(labels[i][j] != -1) {
                ListSet<int> newUnionList;
                long minLabelAddr = newUnionList.head(linked[labels[i][j]]);
                labels[i][j] = newUnionList.nodeValue(minLabelAddr);
            }
        }
    }
}



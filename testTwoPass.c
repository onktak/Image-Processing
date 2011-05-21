/*
   ============================================================================
Name        : twoPass.c
Author      :
Version     :
Copyright   : Your copyright notice
Description : Hello World in C, Ansi-style
============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include "union_find.h"
#include "two_pass.h"

void print_formatted(int **pixels, int width, int height);

unsigned char decoy[10][18] = {

		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0},
		{0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0},
		{0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0},
		{0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 0, 0},
		{0, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0},
		{0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0},
		{0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

int gWidth = 18;
int gHeight = 10;

int main(void) {

    int i, j;
    int **labels;
    labels = (int **)malloc(sizeof(int *) * 10);
    for(i = 0; i < 10; i++) {
        labels[i] = (int *)malloc(sizeof(int) * 18);
    }
    for(i = 0; i < 10; i++) {
        for(j = 0; j < 18; j++) {
            labels[i][j] = -1;
        }
    }
    unsigned char **pixels;
    pixels = (unsigned char **)malloc(sizeof(unsigned char *) * 10);
    for(i = 0; i < 10; i++) {
        pixels[i] = (unsigned char *)malloc(sizeof(unsigned char) * 18);
    }
    for(i = 0; i < 10; i++) {
        for(j = 0; j < 18; j++) {
            pixels[i][j] = decoy[i][j];
        }
    }

    two_pass(pixels, labels, gWidth, gHeight);
    print_formatted(labels, gWidth, gHeight);

    return EXIT_SUCCESS;
}

void print_formatted(int **pixels, int width, int height){

    int i, j;

    for(i = 0; i < height; i++) {
        for(j = 0; j < width; j++) {
            if(pixels[i][j] != -1) {
                printf("%4d", pixels[i][j]);
            } else {
                printf("    ");
            }
        }
        printf("\n");
    }
    printf("\n\n");
}



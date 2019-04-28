#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "imcore.h"
#include "lacore.h"
#include "barcode_detector.h"

void find_rectangle(matrix_t *in, struct point_t minAreaRect[4])
{
    uint32_t i,j;

    vector_t *points = vector_create(struct point_t);

    // compute the edge intensity
    for (j = 0; j < rows(in); j++)
    {
        for (i = 0; i < cols(in); i++)
        {
            // if mask is 1, find the bounding box
            if (atui8(in, j, i, 0) > 0)
            {
                struct point_t p = point(i,j,0);
                vector_push(points, &p);
            }

        }
    }

    // find rectangle
    point_fit_rectangle(vdata(points, 0), length(points), minAreaRect);

    // free unused variables
    vector_free(&points);
}

void rotate(float *x, float *y, float cx, float cy, float t)
{
    float xp = x[0];
    float yp = y[0];

    x[0] = (xp - cx) * cos(t) - (yp - cy) * sin(t) + cx;
    y[0] = (xp - cx) * sin(t) + (yp - cy) * cos(t) + cy;
}

//tl,tr,bl,br
float find_overlap(struct point_t correct[4], struct point_t detection[4])
{
    // find the center of the rectangles
    float c1x = (correct[0].x + correct[3].x) / 2.0f;
    float c1y = (correct[0].y + correct[3].y) / 2.0f;

    float c2x = (detection[0].x + detection[3].x) / 2.0f;
    float c2y = (detection[0].y + detection[3].y) / 2.0f;

    // find the orientation of the rectangle
    float t1 = atan2f(correct[1].y - correct[0].y, correct[1].x - correct[0].x);
    float t2 = atan2f(detection[1].y - detection[0].y, detection[1].x - detection[0].x);

    // rotate both rectangles around the center with -t angle, so that they can be treathed as rectangle_t
    int i;
    for(i = 0; i < 4; i++)
    {
        rotate(&correct[i].x, &correct[i].y, c1x, c1y, -t1);
        rotate(&detection[i].x, &detection[i].y, c2x, c2y, -t2);
    }

    struct rectangle_t r1 = rectangle(min(correct[0].x,correct[3].x), min(correct[0].y,correct[3].y), abs(correct[3].x - correct[0].x), abs(correct[3].y - correct[0].y), 0);
    struct rectangle_t r2 = rectangle(min(detection[0].x, detection[3].x), min(detection[0].y, detection[3].y), abs(detection[3].x - detection[0].x), abs(detection[3].y - detection[0].y), 0);

    // return the normalized area between the two bounding boxes
    return rectangle_overlap(r1, r2, 1);
}

int main(int argc, unsigned char *argv[]) 
{
    int NumTestSamples = 0;
    int NumCorrectDetections = 0;
    int NumIncorrectDetections = 0;

    char filename[256];

    int test;
    for(test = 0; test < 365; test++)
    {
        sprintf(filename, "../data/dataset/1d_barcode_extended_plain/images/image_%03d.bmp", test + 1);
        matrix_t *original_image = imread(filename);

        sprintf(filename, "../data/dataset/1d_barcode_extended_plain/masks/correctmask_%03d.bmp", test + 1);
        matrix_t *correct_mask = imread(filename);

        // find the bounding rect of the correct mask
        struct point_t correct[4];
        find_rectangle(correct_mask, correct);

        // draw the minimum area rectangle for output
        draw_line(original_image, correct[0], correct[1], RGB(0, 255, 0), 2);
        draw_line(original_image, correct[1], correct[3], RGB(0, 255, 0), 2);
        draw_line(original_image, correct[3], correct[2], RGB(0, 255, 0), 2);
        draw_line(original_image, correct[2], correct[0], RGB(0, 255, 0), 2);

        // find the four corners of the bacode
        struct point_t detection[4];
        find_barcode(original_image, detection);

        // draw the minimum area rectangle for output
        draw_line(original_image, detection[0], detection[1], RGB(255, 100, 0), 2);
        draw_line(original_image, detection[1], detection[3], RGB(255, 100, 0), 2);
        draw_line(original_image, detection[3], detection[2], RGB(255, 100, 0), 2);
        draw_line(original_image, detection[2], detection[0], RGB(255, 100, 0), 2);

        float overlap = find_overlap(correct, detection);
        printf("Overlap[%03d]: %5.2f\n", test + 1, overlap);

        // if the overlapping area is larger than 80% of the total area, assume it correct
        if(overlap > 0.7)
        {
            NumCorrectDetections++;
            sprintf(filename, "../data/dataset/1d_barcode_extended_plain/results/true_foundmask_%03d.bmp", test + 1);
        }
        else
        {
            NumIncorrectDetections++;
            sprintf(filename, "../data/dataset/1d_barcode_extended_plain/results/false_foundmask_%03d.bmp", test + 1);
        }
        NumTestSamples++;
        imwrite(original_image, filename);

        // deallocate the unused memory
        matrix_free(&original_image);
        matrix_free(&correct_mask);
    }

    printf("---------------Test Results---------------\n");
    printf("Total number of test samples        : %03d\n", NumTestSamples);
    printf("Total number of correct detections  : %03d\n", NumCorrectDetections);
    printf("Total number of incorrect detections: %03d\n", NumIncorrectDetections);

    return 0;
}
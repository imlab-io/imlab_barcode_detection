#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "imcore.h"
#include "lacore.h"
#include "barcode_detector.h"

int main(int argc, unsigned char *argv[]) 
{
    matrix_t *img = imread(argv[1]);
    
    // find the four corners of the bacode
    struct point_t minAreaRect[4];
    find_barcode(img, minAreaRect);

    // draw the minimum area rectangle
    draw_line(img, minAreaRect[0], minAreaRect[1], RGB(255, 0, 0), 2);
    draw_line(img, minAreaRect[1], minAreaRect[3], RGB(255, 0, 0), 2);
    draw_line(img, minAreaRect[3], minAreaRect[2], RGB(255, 0, 0), 2);
    draw_line(img, minAreaRect[2], minAreaRect[0], RGB(255, 0, 0), 2);

    // write the result image
    imwrite(img, "results.bmp");

    // deallocate the unused memory
    matrix_free(&img);

    return 0;
}
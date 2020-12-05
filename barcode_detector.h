#ifndef IMLAB_BARCODE_DETECTOR
#define IMLAB_BARCODE_DETECTOR

#include "imcore.h"
#include "lacore.h"

// compute energy map
return_t energy(matrix_t *in, matrix_t *out)
{
    uint32_t i, j, k;

    uint32_t block_width = 31;
    uint32_t block_height = 11;

    // resize the output matrix
    matrix_resize(out, rows(in), cols(in), channels(in));

    // create temp matrix for the edge intensity calculation
    matrix_t *edges = matrix_create(uint8_t, rows(in), cols(in), channels(in));

    // compute the edge intensity
    for (j = 1; j < rows(in) - 1; j++)
    {
        for (i = 1; i < cols(in) - 1; i++)
        {
            int hEdges = abs(atui8(in, j + 1, i, 0) - atui8(in, j - 1, i, 0));
            int vEdges = abs(atui8(in, j, i + 1, 0) - atui8(in, j, i - 1, 0));

            // combine edges
            atui8(edges, j, i, 0) = clamp(vEdges - hEdges, 0, 255);
        }
    }

    // remove noise by using box blur
    box_filter(edges, block_width, block_height, out);

    // delete temp matrix
    matrix_free(&edges);
}

return_t find_barcode(matrix_t *img, struct point_t minAreaRect[4])
{
    matrix_t *img_gray = matrix_create(uint8_t);

    // convert input into grayscale
    rgb2gray(img, img_gray);

    // compute edge energy
    matrix_t *filter = matrix_create(uint8_t, rows(img_gray), cols(img_gray), 1);
    energy(img_gray, filter);

    // threshold energy map
    imthreshold(filter, 0.6 * imotsu(filter), filter);

    // do connected component labeling
    uint32_t numberOfComponenets = 0;
    vector_t **components = bwconncomp(filter, &numberOfComponenets);

    // find the largest connected component
    uint32_t largestPixelCount = 0;
    uint32_t largestID = 0;

    uint32_t i = 0;
    for (i = 0; i < numberOfComponenets; i++)
    {
        if (length(components[i]) > largestPixelCount)
        {
            largestPixelCount = length(components[i]);
            largestID = i;
        }
    }

    // find minimum area rectangle
    point_fit_rectangle(vdata(components[largestID], 0), length(components[largestID]), minAreaRect);

    // deallocate the unused memory
    for (i = 0; i < numberOfComponenets; i++)
    {
        vector_free(&components[i]);
    }
    free(components);
    matrix_free(&filter);
    matrix_free(&img_gray);

    return SUCCESS;
}

#endif
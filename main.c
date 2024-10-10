#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "utils.h"

typedef unsigned char gray;

int main()
{
    FILE *ifp, *ofp;
    gray *graymap;
    gray *product;
    int ich1, ich2, rows, cols, maxval;
    char input_file[100], output_file[100];
    int filter_type, passes;

    printf("Choose a filter to apply:\n");
    printf("1. Binomial Filter 3x3 (Smoothing)\n");
    printf("2. Binomial Filter 5x5 (Smoothing)\n");
    printf("3. Median Filter (Noise Reduction)\n");
    printf("Enter your choice: ");
    scanf("%d", &filter_type);
    if (filter_type != 1 && filter_type != 2 && filter_type != 3)
    {
        printf("Invalid filter type! Choose 1 for binomial 3x3, 2 for binomial 5x5, or 3 for median filter.\n");
        return 1;
    }

    printf("Enter the number of passes: ");
    scanf("%d", &passes);
    printf("Enter the input PGM file name (e.g., boat.pgm): ");
    scanf("%s", input_file);
    printf("Enter the output PGM file name (e.g., render.pgm): ");
    scanf("%s", output_file);

    ifp = fopen(input_file, "rb");
    if (ifp == NULL)
    {
        printf("Error in opening file %s\n", input_file);
        exit(1);
    }

    // Magic number reading
    ich1 = getc(ifp);
    ich2 = getc(ifp);
    if (ich1 != 'P' || ich2 != '5')
    {
        pm_erreur("Wrong file type. Only P5 format is supported.");
    }

    cols = pm_getint(ifp);
    rows = pm_getint(ifp);
    maxval = pm_getint(ifp);

    fgetc(ifp);

    // Memory allocation for the images
    graymap = (gray *)malloc(cols * rows * sizeof(gray));
    product = (gray *)malloc(cols * rows * sizeof(gray));
    if (!graymap || !product)
    {
        printf("Memory allocation error\n");
        fclose(ifp);
        return 1;
    }

    fread(graymap, sizeof(gray), cols * rows, ifp);
    fclose(ifp);

    for (int i = 0; i < passes; i++)
    {
        if (filter_type == 1)
        {
            binomial_filter_3x3(graymap, product, cols, rows);
        }
        else if (filter_type == 2)
        {
            binomial_filter_5x5(graymap, product, cols, rows);
        }
        else if (filter_type == 3)
        {
            median_filter(graymap, product, cols, rows);
        }
        memcpy(graymap, product, cols * rows * sizeof(gray));
    }

    /* Opening output file */
    ofp = fopen(output_file, "wb");
    if (ofp == NULL)
    {
        printf("Error in opening file %s\n", output_file);
        free(graymap); // Free allocated memory before exit
        free(product);
        exit(1);
    }

    /* Writing the output image */
    fprintf(ofp, "P5\n%d %d\n%d\n", cols, rows, maxval);
    fwrite(product, sizeof(gray), cols * rows, ofp);
    fclose(ofp);

    /* Free allocated memory */
    free(graymap);
    free(product);

    printf("Filter applied successfully. Output saved in %s\n", output_file);

    return 0;
}

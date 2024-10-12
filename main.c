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
    int filter_type, passes = 1; // default to 1 pass for filters
    int histogram_values[256];
    int min_value = 255;
    int max_value = 0;

    printf("Choose a filter, or histogram to apply:\n");
    printf("1. Binomial Filter 3x3 (Smoothing)\n");
    printf("2. Binomial Filter 5x5 (Smoothing)\n");
    printf("3. Median Filter (Noise Reduction)\n");
    printf("4. Create Histogram\n");
    printf("5. Histogram Stretching\n");
    printf("6. Histogram Equalization\n");
    printf("Enter your choice (or any other key to exit):\n");
    scanf("%d", &filter_type);

    if (filter_type < 1 || filter_type > 6)
    {
        printf("Invalid filter type! Choose between 1 and 6.\n");
        return 1;
    }

    printf("Enter the input PGM file name: ");
    scanf("%s", input_file);

    if (filter_type == 4 || filter_type == 5 || filter_type == 6)
    {
        printf("Enter the output PNG file name for the histogram: ");
        scanf("%s", output_file);
    }
    else
    {
        printf("Enter the output PGM file name for the filtered image: ");
        scanf("%s", output_file);
        printf("Enter the number of passes for the filter: ");
        scanf("%d", &passes);
    }

    ifp = fopen(input_file, "rb");
    if (ifp == NULL)
    {
        printf("Error in opening file %s\n", input_file);
        exit(1);
    }

    ich1 = getc(ifp);
    ich2 = getc(ifp);
    if (ich1 != 'P' || ich2 != '5')
    {
        printf("Wrong file type. Only P5 format is supported.\n");
        fclose(ifp);
        return 1;
    }
    cols = pm_getint(ifp);
    rows = pm_getint(ifp);
    maxval = pm_getint(ifp);
    fgetc(ifp);
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

    if (filter_type == 4)
    {
        histogram(graymap, rows, cols, histogram_values);

        FILE *histogram_file = fopen("histogram_output.csv", "w");
        if (histogram_file == NULL)
        {
            printf("Error opening histogram output file.\n");
            free(graymap);
            free(product);
            return 1;
        }
        for (int i = 0; i < 256; i++)
        {
            fprintf(histogram_file, "%d,%d\n", i, histogram_values[i]);
        }
        fclose(histogram_file);
        plot_histogram("histogram_output.csv", output_file);

        free(graymap);
        free(product);
        printf("Histogram created successfully and plotted to %s\n", output_file);
        return 0;
    }
    else if (filter_type == 5)
    {
        calculate_min_max(graymap, rows, cols, &min_value, &max_value);
        histogram_stretching(graymap, product, rows, cols, max_value, min_value);
        ofp = fopen("stretched_output.pgm", "wb");
        if (ofp == NULL)
        {
            printf("Error in opening file for stretched image.\n");
            free(graymap);
            free(product);
            exit(1);
        }
        fprintf(ofp, "P5\n%d %d\n%d\n", cols, rows, maxval);
        fwrite(product, sizeof(gray), cols * rows, ofp);
        fclose(ofp);
        histogram(product, rows, cols, histogram_values);
        FILE *histogram_file = fopen("histogram_stretched_output.csv", "w");
        if (histogram_file == NULL)
        {
            printf("Error opening histogram output file.\n");
            free(graymap);
            free(product);
            return 1;
        }
        for (int i = 0; i < 256; i++)
        {
            fprintf(histogram_file, "%d,%d\n", i, histogram_values[i]);
        }
        fclose(histogram_file);

        plot_histogram("histogram_stretched_output.csv", output_file);

        free(graymap);
        free(product);
        printf("Histogram stretching applied. Image saved to stretched_output.pgm and histogram plotted to %s\n", output_file);
        return 0;
    }
    else if (filter_type == 6)
    {
        histogram_equalization(graymap, product, rows, cols);
        ofp = fopen("equalized_output.pgm", "wb");
        if (ofp == NULL)
        {
            printf("Error in opening file for equalized image.\n");
            free(graymap);
            free(product);
            exit(1);
        }
        fprintf(ofp, "P5\n%d %d\n%d\n", cols, rows, maxval);
        fwrite(product, sizeof(gray), cols * rows, ofp);
        fclose(ofp);

        histogram(product, rows, cols, histogram_values);
        FILE *histogram_file = fopen("histogram_equalized_output.csv", "w");
        if (histogram_file == NULL)
        {
            printf("Error opening histogram output file.\n");
            free(graymap);
            free(product);
            return 1;
        }
        for (int i = 0; i < 256; i++)
        {
            fprintf(histogram_file, "%d,%d\n", i, histogram_values[i]);
        }
        fclose(histogram_file);
        plot_histogram("histogram_equalized_output.csv", output_file);

        free(graymap);
        free(product);
        printf("Histogram equalization applied. Image saved to equalized_output.pgm and histogram plotted to %s\n", output_file);
        return 0;
    }

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

    ofp = fopen(output_file, "wb");
    if (ofp == NULL)
    {
        printf("Error in opening file %s\n", output_file);
        free(graymap);
        free(product);
        exit(1);
    }

    fprintf(ofp, "P5\n%d %d\n%d\n", cols, rows, maxval); // magic number
    fwrite(product, sizeof(gray), cols * rows, ofp);
    fclose(ofp);

    free(graymap);
    free(product);

    printf("Filter applied successfully. Output saved in %s\n", output_file);

    return 0;
}

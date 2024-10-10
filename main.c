#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "utils.h"

typedef unsigned char gray;

int main() {
    FILE *ifp, *ofp;
    gray *graymap;
    gray *product;
    int ich1, ich2, rows, cols, maxval;
    char input_file[100], output_file[100];
    int filter_type, passes = 1;  // Default to 1 pass for filters
    int histogram_values[256];

    printf("Choose a filter to apply:\n");
    printf("1. Binomial Filter 3x3 (Smoothing)\n");
    printf("2. Binomial Filter 5x5 (Smoothing)\n");
    printf("3. Median Filter (Noise Reduction)\n");
    printf("4. Create Histogram \n");
    printf("Enter your choice (or any other key to exit):\n");
    scanf("%d", &filter_type);

    // Check for invalid filter type
    if (filter_type < 1 || filter_type > 4) {
        printf("Invalid filter type! Choose 1 for binomial 3x3, 2 for binomial 5x5, 3 for median filter, or 4 for creating Histogram.\n");
        return 1;
    }

    // For filters 1, 2, and 3, ask for the number of passes
    if (filter_type != 4) {
        printf("Enter the number of passes: ");
        scanf("%d", &passes);
           printf("Enter the output PGM file name: ");
    scanf("%s", output_file);
        
    }

    printf("Enter the input PGM file name: ");
    scanf("%s", input_file);
 

    ifp = fopen(input_file, "rb");
    if (ifp == NULL) {
        printf("Error in opening file %s\n", input_file);
        exit(1);
    }
    // Magic number reading
    ich1 = getc(ifp);
    ich2 = getc(ifp);
    if (ich1 != 'P' || ich2 != '5') {
        printf("Wrong file type. Only P5 format is supported.\n");
        fclose(ifp);
        return 1;
    }
    
    // Read image dimensions and max value
    cols = pm_getint(ifp);
    rows = pm_getint(ifp);
    maxval = pm_getint(ifp);
    fgetc(ifp);  // Consume the single whitespace character

    // Memory allocation for the images
    graymap = (gray *)malloc(cols * rows * sizeof(gray));
    product = (gray *)malloc(cols * rows * sizeof(gray));
    if (!graymap || !product) {
        printf("Memory allocation error\n");
        fclose(ifp);
        return 1;
    }
    fread(graymap, sizeof(gray), cols * rows, ifp);
    fclose(ifp);

    // Apply filter based on user choice
    if (filter_type == 4) {
        // Histogram operation doesn't need multiple passes
        histogram(graymap, rows, cols, histogram_values);

        // Save histogram to CSV file
        FILE *histogram_file = fopen("histogram_output.csv", "w");
        if (histogram_file == NULL) {
            printf("Error opening histogram output file.\n");
            free(graymap);
            free(product);
            return 1;
        }
        for (int i = 0; i < 256; i++) {
            fprintf(histogram_file, "%d,%d\n", i, histogram_values[i]);
        }
        fclose(histogram_file);

        // Free memory and exit after histogram
        free(graymap);
        free(product);
        printf("Histogram created successfully in histogram_output.csv\n");
        return 0;
    }

    // Apply the selected filter for the given number of passes
    for (int i = 0; i < passes; i++) {
        if (filter_type == 1) {
            binomial_filter_3x3(graymap, product, cols, rows);
        } else if (filter_type == 2) {
            binomial_filter_5x5(graymap, product, cols, rows);
        } else if (filter_type == 3) {
            median_filter(graymap, product, cols, rows);
        }
        // Copy the processed image back for next pass (if any)
        memcpy(graymap, product, cols * rows * sizeof(gray));
    }

    /* Opening output file */
    ofp = fopen(output_file, "wb");
    if (ofp == NULL) {
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

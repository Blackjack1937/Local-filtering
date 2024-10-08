#include <stdlib.h>
#include <stdio.h>
#include "utils.h"

typedef unsigned char gray;

int main(int argc, char *argv[]) {
    FILE *ifp, *ofp;
    gray *graymap;
    gray *product;
    int ich1, ich2, rows, cols, maxval;
    
    /* Arguments */
    if (argc != 3)
    {
        printf("\nUsage: %s file_in file_out \n\n", argv[0]);
        exit(0);
    }
   
      /* Opening input file */
    ifp = fopen(argv[1], "rb");
    if (ifp == NULL)
    {
        printf("error in opening file %s\n", argv[1]);
        exit(1);
    }

    /* Magic number reading */
    // ich1 = getc(ifp);
    // if (ich1 == EOF)
    // {
    //     pm_erreur("EOF / read error / magic number");
    // }
    // ich2 = getc(ifp);
    // if (ich2 == EOF)
    // {
    //     pm_erreur("EOF / read error / magic number");
    // }

    // if (ich2 != '2' && ich2 != '5')
    // {
    //     pm_erreur("wrong file type");
    // }
    // else
    // {
    //     pgmraw_in = (ich2 == '2') ? 0 : 1; // Simplified assignment
    // }
    ich1 = getc(ifp);
    ich2 = getc(ifp);
    if (ich1 != 'P' || ich2 != '5') {
        pm_erreur("Wrong file type. Only P5 format is supported.");
    }
   /* Reading image dimensions */
    cols = pm_getint(ifp);
    rows = pm_getint(ifp);
    maxval = pm_getint(ifp);
// /* Reading */
//     for (i = 0; i < rows; i++)
//         for (j = 0; j < cols; j++)
//         {
//             if (pgmraw_in)
//             {
//                 graymap[i * cols + j] = pm_getrawbyte(ifp);
//             }
//             else
//             {
//                 graymap[i * cols + j] = pm_getint(ifp);
//             }
//         }

    fgetc(ifp); 
// // Binomial filtering
//     int filter_coeff = 16;
//     int filter[3][3] = {{1, 2, 1}, 
//                         {2, 4, 2}, 
//                         {1, 2, 1}};
//     product = (gray *)malloc(cols * rows * sizeof(gray));
//     if (!product) // Check if memory allocation was successful
//     {
//         printf("Memory allocation error\n");
//         free(graymap); // Free previously allocated memory
//         exit(1);
//     }

//     // Call the binomial_filter function
//     binomial_filter(graymap, product, filter, filter_coeff, cols, rows);

    
    graymap = (gray *)malloc(cols * rows * sizeof(gray));
    product = (gray *)malloc(cols * rows * sizeof(gray));
    if (!graymap || !product) {
        printf("Memory allocation error\n");
        fclose(ifp);
        return 1;
    }

    fread(graymap, sizeof(gray), cols * rows, ifp); 
    fclose(ifp);

   
    binomial_filter(graymap, product, cols, rows);
  /* Opening output file */
    ofp = fopen(argv[2], "wb");
    if (ofp == NULL)
    {
        printf("error in opening file %s\n", argv[2]);
        free(graymap); // Free allocated memory before exit
        free(product);
        exit(1);
    }

    // /* Writing */
    // pgmraw_out = pgmraw_in ? 0 : 1; // Determine output format
    // if (pgmraw_out)
    // {
    //     fprintf(ofp, "P5\n");
    // }
    // else
    // {
    //     fprintf(ofp, "P2\n");
    // }

    // fprintf(ofp, "%d %d\n", cols, rows);
    // fprintf(ofp, "%d\n", maxval);

    // // Write the product (filtered image) to the output file
    // for (i = 0; i < rows; i++)
    // {
    //     for (j = 0; j < cols; j++)
    //     {
    //         if (pgmraw_out)
    //         {
    //             putc(product[i * cols + j], ofp);
    //         }
    //         else
    //         {
    //             fprintf(ofp, "%d ", product[i * cols + j]);
    //         }
    //     }
    //     if (!pgmraw_out) // Add a new line after each row for P2 format
    //     {
    //         fprintf(ofp, "\n");
    //     }
    // }

    fprintf(ofp, "P5\n%d %d\n%d\n", cols, rows, maxval);  
    fwrite(product, sizeof(gray), cols * rows, ofp);  
    fclose(ofp);

    
    free(graymap);
    free(product);

    return 0;
}

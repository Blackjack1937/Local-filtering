#ifndef UTILS_H // include guard to prevent multiple inclusions
#define UTILS_H

typedef unsigned char bit;
typedef unsigned char gray;

char pm_getc(FILE *file);
bit pm_getbit(FILE *file);
unsigned char pm_getrawbyte(FILE *file);
int pm_getint(FILE *ifp);
void pm_erreur(const char *msg);
void binomial_filter_3x3(gray *graymap, gray *product, int cols, int rows);
void binomial_filter_5x5(gray *graymap, gray *product, int cols, int rows);
void median_filter(gray *graymap, gray *product, int cols, int rows);
int compare(const void *a, const void *b);
void histogram(gray *graymap, int rows, int cols, int histogram[256]);
void calculate_min_max(gray *graymap, int rows, int cols, int *min_value, int *max_value);
void histogram_stretching(gray *graymap, gray *product, int rows, int cols, int max_value, int min_value);
void histogram_equalization(gray *graymap, gray *product, int rows, int cols);
void plot_histogram(const char *csv_filename, const char *output_image);
#endif

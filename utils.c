#include <stdlib.h>
#include <stdio.h>
#include "utils.h"

typedef unsigned char gray;

char pm_getc(FILE *file)
{
    int ich;
    char ch;

    ich = getc(file);
    if (ich == EOF)
        pm_erreur("EOF / read error");
    ch = (char)ich;

    if (ch == '#')
    {
        do
        {
            ich = getc(file);
            if (ich == EOF)
                pm_erreur("EOF / read error");
            ch = (char)ich;
        } while (ch != '\n' && ch != '\r');
    }

    return ch;
}

unsigned char pm_getrawbyte(FILE *file)
{
    int iby;

    iby = getc(file);
    if (iby == EOF)
        pm_erreur("EOF / read error");
    return (unsigned char)iby;
}

int pm_getint(FILE *file)
{
    char ch;
    int i;

    do
    {
        ch = pm_getc(file);
    } while (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r');

    if (ch < '0' || ch > '9')
        pm_erreur("read error: != [0-9]");

    i = 0;
    do
    {
        i = i * 10 + ch - '0';
        ch = pm_getc(file);
    } while (ch >= '0' && ch <= '9');

    return i;
}

void pm_erreur(const char *msg)
{
    fprintf(stderr, "\n%s \n\n", msg);
    exit(1);
}

int compare(const void *a, const void *b)
{
    return (*(unsigned char *)a - *(unsigned char *)b);
}

void median_filter(gray *graymap, gray *product, int cols, int rows)
{
    int median_window[9];
    int count;

    for (int i = 1; i < rows - 1; i++)
    {
        for (int j = 1; j < cols - 1; j++)
        {
            count = 0;
            for (int n = -1; n <= 1; n++)
            {
                for (int m = -1; m <= 1; m++)
                {
                    median_window[count++] = graymap[(i + n) * cols + (j + m)];
                }
            }
            qsort(median_window, 9, sizeof(int), compare);
            product[i * cols + j] = median_window[4];
        }
    }
}

void binomial_filter_3x3(gray *graymap, gray *product, int cols, int rows)
{
    int filter[3][3] = {
        {1, 2, 1},
        {2, 4, 2},
        {1, 2, 1}};
    int filter_coeff = 16;

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            int sum = 0;
            for (int n = -1; n <= 1; n++)
            {
                for (int m = -1; m <= 1; m++)
                {

                    if ((i + n) >= 0 && (j + m) >= 0 && (i + n) < rows && (j + m) < cols)
                    {
                        sum += graymap[(i + n) * cols + (j + m)] * filter[n + 1][m + 1];
                    }
                }
            }

            sum = sum / filter_coeff;
            if (sum < 0)
            {
                sum = 0;
            }
            else if (sum > 255)
            {
                sum = 255;
            }
            product[i * cols + j] = (gray)sum;
        }
    }
}

void binomial_filter_5x5(gray *graymap, gray *product, int cols, int rows)
{
    int filter[5][5] = {
        {1, 4, 6, 4, 1},
        {4, 16, 24, 16, 4},
        {6, 24, 36, 24, 6},
        {4, 16, 24, 16, 4},
        {1, 4, 6, 4, 1}};
    int filter_coeff = 256;

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            int sum = 0;
            for (int n = -1; n <= 1; n++)
            {
                for (int m = -1; m <= 1; m++)
                {

                    if ((i + n) >= 0 && (j + m) >= 0 && (i + n) < rows && (j + m) < cols)
                    {
                        sum += graymap[(i + n) * cols + (j + m)] * filter[n + 1][m + 1];
                    }
                }
            }

            sum = sum / filter_coeff;
            if (sum < 0)
            {
                sum = 0;
            }
            else if (sum > 255)
            {
                sum = 255;
            }
            product[i * cols + j] = (gray)sum;
        }
    }
}

void histogram(gray *graymap, int rows, int cols, int histogram[256])
{

    for (int i = 0; i < 256; i++)
    {
        histogram[i] = 0;
    }
    for (int n = 0; n < rows; n++)
    {
        for (int m = 0; m < cols; m++)
        {
            int value = graymap[n * cols + m];
            histogram[value]++;
        }
    }
}

void calculate_min_max(gray *graymap, int rows, int cols, int *min_value, int *max_value)
{
    *min_value = 255;
    *max_value = 0;
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            int value = graymap[i * cols + j];
            if (value > *max_value)
            {
                *max_value = value;
            }
            if (value < *min_value)
            {
                *min_value = value;
            }
        }
    }
}

void histogram_stretching(gray *graymap, gray *product, int rows, int cols, int max_value, int min_value)
{
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            int value = graymap[i * cols + j];
            product[i * cols + j] = (gray)((value - min_value) * 255.0 / (max_value - min_value));
        }
    }
}

void histogram_equalization(gray *graymap, gray *product, int rows, int cols)
{
    int histogram[256] = {0};
    int cdf[256] = {0};          // CDF = Cumulative Distribution Function
    int equalized_lt[256] = {0}; // lookup table
    int total_pixels = rows * cols;
    int min_cdf = 0;

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            histogram[graymap[i * cols + j]]++;
        }
    }

    cdf[0] = histogram[0];
    for (int i = 1; i < 256; i++)
    {
        cdf[i] = cdf[i - 1] + histogram[i];
    }

    for (int i = 0; i < 256; i++)
    {
        if (cdf[i] > 0)
        {
            min_cdf = cdf[i];
            break;
        }
    }

    for (int i = 0; i < 256; i++)
    {
        equalized_lt[i] = (int)(((float)(cdf[i] - min_cdf) / (total_pixels - min_cdf)) * 255.0);
    }
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            product[i * cols + j] = (gray)equalized_lt[graymap[i * cols + j]];
        }
    }
}

void plot_histogram(const char *csv_filename, const char *output_image)
{
    FILE *gnuplot = popen("gnuplot", "w");
    if (gnuplot == NULL)
    {
        printf("Error opening Gnuplot.\n");
        return;
    }

    // Gnuplot commands to generate the histogram plot
    fprintf(gnuplot, "set datafile separator ','\n");
    fprintf(gnuplot, "set terminal png\n");
    fprintf(gnuplot, "set output '%s'\n", output_image); // Output PNG file
    fprintf(gnuplot, "set xlabel 'Intensity Value'\n");
    fprintf(gnuplot, "set ylabel 'Frequency'\n");
    fprintf(gnuplot, "set title 'Image Histogram'\n");
    fprintf(gnuplot, "plot '%s' using 1:2 with boxes\n", csv_filename); // Use CSV for data
    fflush(gnuplot);                                                    // Send all commands to Gnuplot

    pclose(gnuplot); // Close the Gnuplot process
}

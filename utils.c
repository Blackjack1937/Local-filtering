#include <stdlib.h>
#include <stdio.h>
#include "utils.h"

typedef unsigned char gray;

char pm_getc(FILE *file) {
    int ich;
    char ch;

    ich = getc(file);
    if (ich == EOF)
        pm_erreur("EOF / read error");
    ch = (char)ich;

    if (ch == '#') {
        do {
            ich = getc(file);
            if (ich == EOF)
                pm_erreur("EOF / read error");
            ch = (char)ich;
        } while (ch != '\n' && ch != '\r');
    }

    return ch;
}


unsigned char pm_getrawbyte(FILE *file) {
    int iby;

    iby = getc(file);
    if (iby == EOF)
        pm_erreur("EOF / read error");
    return (unsigned char)iby;
}

int pm_getint(FILE *file) {
    char ch;
    int i;

    do {
        ch = pm_getc(file);
    } while (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r');

    if (ch < '0' || ch > '9')
        pm_erreur("read error: != [0-9]");

    i = 0;
    do {
        i = i * 10 + ch - '0';
        ch = pm_getc(file);
    } while (ch >= '0' && ch <= '9');

    return i;
}

void pm_erreur(const char *msg) {
    fprintf(stderr, "\n%s \n\n", msg);
    exit(1);
}


void binomial_filter(gray *graymap, gray *product, int cols, int rows) {
    // int filter[3][3] = {
    //     {1, 2, 1},
    //     {2, 4, 2},
    //     {1, 2, 1}
    // };
    int filter[5][5] = {
        {1,0,0,0,0},
        {1,1,0,0,0},
        {1,2,1,0,0},
        {1,4,6,4,1}
    };
    int filter_coeff = 16;

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            int sum = 0;
            for (int n = -1; n <= 1; n++) {
                for (int m = -1; m <= 1; m++) {
                
                    if ((i + n) >= 0 && (j + m) >= 0 && (i + n) < rows && (j + m) < cols) {
                        sum += graymap[(i + n) * cols + (j + m)] * filter[n + 1][m + 1];
                    }
                }
            }

            sum = sum / filter_coeff;
            if (sum < 0) {
                sum = 0;
            } else if (sum > 255) {
                sum = 255;
            }
            product[i * cols + j] = (gray)sum; 
        }
    }
}

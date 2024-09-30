#include <stdio.h>
#include <stdlib.h>
#include "utils.h"

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

bit pm_getbit(FILE *file)
{
    char ch;

    do
    {
        ch = pm_getc(file);
    } while (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r');

    if (ch != '0' && ch != '1')
        pm_erreur("read error : != [0|1]");

    return (ch == '1') ? 1 : 0;
}

unsigned char pm_getrawbyte(FILE *file)
{
    int iby;

    iby = getc(file);
    if (iby == EOF)
        pm_erreur("EOF / read error ");
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
        pm_erreur("read error : != [0-9]");

    i = 0;
    do
    {
        i = i * 10 + ch - '0';
        ch = pm_getc(file);
    } while (ch >= '0' && ch <= '9');

    return i;
}

void pm_erreur(char *texte)
{
    fprintf(stderr, "\n%s \n\n", texte);
    exit(1);
}

// void binomial_op(char *matrix1, char *matrix2){
//     int total = 0;
//     for(int i=0; i<3; i++){
//         for(int j=0;j<3;j++){
//             total += matrix1[j][i] * matrix2[i][j];
//         }
//     }
//     total = total/16;
// }

void binomial_filter(char *graymap, char *product, int *filter, int filter_coeff, int cols, int rows)
{
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            for (int n = -1; n <= 1; n++)
            {
                int sum = 0;
                for (int m = -1; m <= 1; m++)
                {
                    if ((i + n) < 0)
                    {
                        sum += graymap[i + n, j + m] * filter[n + 1, m + 1];
                    }
                }
            }
        }
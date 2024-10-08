#ifndef UTILS_H  // Include guard to prevent multiple inclusions
#define UTILS_H

typedef unsigned char bit;
typedef unsigned char gray;


char pm_getc(FILE *file);
bit pm_getbit(FILE *file);
unsigned char pm_getrawbyte(FILE *file);
int pm_getint(FILE *ifp);
void pm_erreur(const char *msg);
void binomial_filter(gray *graymap, gray *product, int cols, int rows);

#endif 

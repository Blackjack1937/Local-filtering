typedef unsigned char bit;
typedef unsigned char gray;

char pm_getc(FILE *file);
bit pm_getbit(FILE *file);
unsigned char pm_getrawbyte(FILE *file);
int pm_getint(FILE *file);

void pm_erreur(char *);

void binomial_filter(gray*graymap, gray *product, int filter[3][3], int filter_coeff, int cols, int rows);
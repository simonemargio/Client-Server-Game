#ifndef ERRORI_H_INCLUDED
#define ERRORI_H_INCLUDED
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void errori_generali(int err);
int controlloSocket(int fd);
int controlloIndirizzo(char str[]);

#endif // ERRORI_H_INCLUDED

#ifndef FRACTION_H
#define FRACTION_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* ------- STRUCTS ------- **
** ----------------------- */

typedef struct {
	double numerator;
	double denominator;
	double value;
} fraction;

/* ------ FUNCTIONS ------ **
** ----------------------- */

fraction convertString  (char *input);
fraction convertInt     (int num, int den);
fraction convertDouble  (double num, double den);

#endif
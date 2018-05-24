#ifndef ARCHIVE_H
#define ARCHIVE_H

#include <stdlib.h>
#include <stdio.h>
#include "fraction.c"
#include "linear_programming.h"

#define UNDEFINED -1
#define IMPRACTICABLE 0
#define UNLIMITED 1
#define GREAT 2

/* ------ FUNCTIONS ------ **
** ----------------------- */

linear_programming  *readMatrixInput     (char *input);
void                printConclusion      (linear_programming *pl, int checking);

#endif
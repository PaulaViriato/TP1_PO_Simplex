#ifndef LINEAR_PROGRAMMING_H
#define LINEAR_PROGRAMMING_H

#include <stdlib.h>
#include <stdio.h>

#define MAXDOUBLE 10^4900
#define MINDOUBLE 0.00001
#define UNDEFINED -1
#define IMPRACTICABLE 0
#define UNLIMITED 1
#define GREAT 2

/* ------- STRUCTS ------- **
** ----------------------- */

typedef struct {
    char name[50]; // Archive name
	int   m, n;    // Column and row of the matriz input
	double **mat;  // Input matrix (initial)
	double *c;     // Vector to be optimized
	double **a;    // Restriction matrix
	double *b;     // Vector base constrains
	double *x;     // Linear programming solution
	double vo;     // Objective value
	double *atte;  // Solution certificate
} linear_programming;

typedef struct {
	int column;
	int row;
} indexes;

/* ------ FUNCTIONS ------ **
** ----------------------- */

linear_programming  *saving                 (linear_programming *pl);
void                redistribution          (linear_programming *pl);
void                setFPI                  (linear_programming *pl);
void                pivoting                (linear_programming *pl, indexes *index);
void                printPivoting           (linear_programming *pl);
indexes             *indexSimplexPrimal     (linear_programming *pl);
indexes             *indexSimplexDual       (linear_programming *pl);
int                 checkSimplexPrimal      (linear_programming *pl, linear_programming *ori);
int                 checkSimplexDual        (linear_programming *pl, linear_programming *ori);
linear_programming  *auxiliaryPL            (linear_programming *pl);
int                 simplexPrimal           (linear_programming *pl);
int                 simplexDual             (linear_programming *pl);
int                 auxiliaryResolution     (linear_programming *pl);
int                 insignificance          (double x);
int                 moderateInsignificance  (double x, double ins);

#endif

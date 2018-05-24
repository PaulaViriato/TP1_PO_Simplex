#include "archive.h"

/* ------ FUNCTIONS ------ **
** ----------------------- */

/* Nome: readMatrixInput
*  Funcao: le arquivo de entrada, inicializa a estrutura da programacao linear e
*  associa o arquivo aos campos.
* -------------------------------------------------------------------------------
*/
linear_programming *readMatrixInput (char *input){
	linear_programming *pl = (linear_programming *) malloc (sizeof(linear_programming));
	FILE *archive = fopen (input, "r");
	pl->name[0] = '.';

	if (archive == NULL){
		printf("ERROR! Archive unavailable or non-existent (READ)!\n");
		return pl;
	} else {
		char err[] = "ERROR! Unrecognized format or reading error!\n", aux[22], c;
		int i, j, count;
		fraction frac;

		if (fscanf(archive, "%d\n", &(pl->m)) == EOF){ printf(err); free(pl); return pl; }
		if (fscanf(archive, "%d\n", &(pl->n)) == EOF){ printf(err); free(pl); return pl; }

		pl->m  = pl->m +1;
		pl->n  = pl->n +1;
		pl->vo = 0;

		pl->mat = (double **) malloc ((pl->m)*sizeof(double *));
		pl->a   = (double **) malloc ((pl->m -1)*sizeof(double *));
		pl->b   = (double *)  malloc ((pl->m -1)*sizeof(double));
		pl->c   = (double *)  malloc ((pl->n -1)*sizeof(double));
		pl->x   = (double *)  malloc ((pl->m -1)*sizeof(double));

		if (fgetc(archive) != '['){ printf(err); free(pl); return pl; }
		for (i=0; i < pl->m; i++){
			if (fgetc(archive) != '['){ printf(err); free(pl); return pl; }

			pl->mat[i] = (double *) malloc ((pl->n)*sizeof(double));
			if (i != 0){ pl->a[i-1] = (double *) malloc ((pl->n -1)*sizeof(double)); }

			for (j=0; j < pl->n; j++){
			    count = 0;
                c = fgetc(archive);
                while ((c != ',')&&(c != ']')){
                    aux[count] = c;
                    count ++;
                    c = fgetc(archive);
                }
                aux[count] = '\0';

				frac = convertString(aux);
				pl->mat[i][j] = (double)frac.value;

				if ((i==0)&&(j<(pl->n -1))){ pl->c[j] = frac.value; }
				if (i != 0){
					if (j < (pl->n -1)){ pl->a[i-1][j] = frac.value; }
					else { pl->b[i-1] = frac.value; pl->x[i-1] = 0; }
				}
			}

			if (c != ']'){ printf(err); free(pl); return pl; }
			if (i < (pl->m -1)){
            	if (fgetc(archive) != ','){ printf(err); free(pl); return pl; }
			}
		}
		if (fgetc(archive) != ']'){ printf(err); free(pl); return pl; }

		i=0;
		while (input[i] != '.'){ pl->name[i] = input[i]; i ++; }
		pl->name[i] = '\0';
	}
	fclose (archive);

	return pl;
}

/* Nome: printConclusion
*  Funcao: escreve num arquivo de texto os resultados encontrados para a
*  programacao linear, de acordo com a especificacao.
* -------------------------------------------------------------------------------
*/
void printConclusion (linear_programming *pl, int checking){
    char destiny[50], sufix[15] = "_conclusion.txt";
    int w = 0, x;
    while (pl->name[w] != '\0'){ destiny[w] = pl->name[w]; w++; }
    for (x=w; x < (w+15); x++){ destiny[x] = sufix[x-w]; }
    destiny[w+15] = '\0';

	FILE *archive = fopen (destiny, "w");

	if (archive == NULL){
		printf("ERROR! Archive unavailable (WRITE)!\n");
		return;
	} else {
		int i;
		fprintf(archive, "%d\n", checking);

		if (checking == IMPRACTICABLE){
			fprintf(archive, "[");
			for (i=0; i < (pl->m -1); i++){
				fprintf(archive, "%.5f", pl->atte[i]);
				if (i < (pl->m -2)){ fprintf(archive, ", "); }
			}
			fprintf(archive, "]\n");
		} else if (checking == UNLIMITED){
			fprintf(archive, "[");
			for (i=0; i < (pl->n -1); i++){
				fprintf(archive, "%.5f", pl->atte[i]);
				if (i < (pl->n -2)){ fprintf(archive, ", "); }
			}
			fprintf(archive, "]\n");
		} else if (checking == GREAT){
			fprintf(archive, "[");
			for (i=0; i < (pl->m -1); i++){
				fprintf(archive, "%.5f", pl->x[i]);
				if (i < (pl->m -2)){ fprintf(archive, ", "); }
			}
			fprintf(archive, "]\n");

			fprintf(archive, "%.5f\n", pl->vo);

			fprintf(archive, "[");
			for (i=0; i < (pl->m -1); i++){
				fprintf(archive, "%.5f", pl->atte[i]);
				if (i < (pl->m -2)){ fprintf(archive, ", "); }
			}
			fprintf(archive, "]\n");
		}
	}
	fclose (archive);
}

#include "linear_programming.h"

/* ------ FUNCTIONS ------ **
** ----------------------- */

/* Nome: saving
*  Funcao: cria uma copia de uma programacao linear com o objetivo de ser modifi-
*  cavel durante a execucao dos simplex (dual e primal). Ja multiplica a funcao a
*  ser otimizada por (-1), a fim de ser utilizado diretamente como tablaux.
* --------------------------------------------------------------------------------
*/
linear_programming *saving (linear_programming *pl){
	linear_programming *new_pl = (linear_programming *) malloc (sizeof(linear_programming));

	new_pl->m  = pl->m;
	new_pl->n  = pl->n;
	new_pl->vo = pl->vo;

	new_pl->mat = (double **) malloc ((new_pl->m)*sizeof(double *));
	new_pl->a   = (double **) malloc ((new_pl->m -1)*sizeof(double *));
	new_pl->b   = (double *)  malloc ((new_pl->m -1)*sizeof(double));
	new_pl->c   = (double *)  malloc ((new_pl->n -1)*sizeof(double));
	new_pl->x   = (double *)  malloc ((new_pl->m -1)*sizeof(double));

	int i, j;
	for (i=0; i < new_pl->m; i++){
		new_pl->mat[i] = (double *) malloc ((new_pl->n)*sizeof(double));
		if (i != 0){ new_pl->a[i-1] = (double *) malloc ((new_pl->n -1)*sizeof(double)); }

		for (j=0; j < new_pl->n; j++){
			if (i==0){
				new_pl->mat[i][j] = (-1)*(pl->mat[i][j]);
				if (j < (new_pl->n -1)){ new_pl->c[j] = new_pl->mat[i][j]; }
			} else {
				new_pl->mat[i][j] = pl->mat[i][j];
				if (j < (new_pl->n -1)){ new_pl->a[i-1][j] = pl->a[i-1][j]; }
			}
		}
		new_pl->b[i-1] = pl->b[i-1];
		new_pl->x[i-1] = pl->x[i-1];
	}
	i=0;
	while (pl->name[i] != '\0'){ new_pl->name[i] = pl->name[i]; i++; }
	new_pl->name[i] = '\0';

	return new_pl;
}

/* Nome: redistribution
*  Funcao: atualiza todas as informacoes da programacao linear de acordo com a
*  versao atual do tableaux (pl->mat).
* --------------------------------------------------------------------------------
*/
void redistribution (linear_programming *pl){
	int i, j;
	for (i=0; i < pl->m; i++){
		for (j=0; j < pl->n; j++){
			if (i == 0){
				if (j == (pl->n -1)){ pl->vo = pl->mat[i][j]; }
				else { pl->c[j] = pl->mat[i][j]; }
			} else {
				if (j == (pl->n -1)){ pl->b[i-1] = pl->mat[i][j]; }
				else { pl->a[i-1][j] = pl->mat[i][j]; }
			}
		}
	}
}

/* Nome: setFPI
*  Funcao: transforma uma programacao linear no formato dado de acordo com a
*  especificacao em uma PL no formato FPI (formato padrão de igualdades), colo-
*  cando as respectivas variaveis de folga.
* --------------------------------------------------------------------------------
*/
void setFPI (linear_programming *pl){
	pl->c = (double *)  malloc ((pl->n + pl->m -2)*sizeof(double));

	int i, j, aux_pos = (pl->n -1), aux_stop = 0;
	for (i=0; i < pl->m; i++){
		pl->mat[i] = (double *) realloc (pl->mat[i], ((pl->n)+(pl->m -1))*sizeof(double));
		if (i!=0){ pl->a[i-1] = (double *) realloc (pl->a[i-1], (pl->n + pl->m -2)*sizeof(double)); }

		for (j=(pl->n -1); j<(pl->n + pl->m -2); j++){
			pl->mat[i][j+1] = pl->mat[i][j];

			if (i==0){
				pl->c[j] = 0;
				pl->mat[i][j] = 0;
			} else{
				if ((aux_stop == 0)&&(j == aux_pos)){
					pl->mat[i][j] = 1;
					pl->a[i-1][j] = 1;
					aux_stop = 1;
					aux_pos ++;
				} else {
					pl->mat[i][j] = 0;
					pl->a[i-1][j] = 0;
				}
			}
		}
		aux_stop = 0;
	}
	pl->n = (pl->n)+(pl->m -1);
}

/* Nome: pivoting
*  Funcao: realiza o pivoteamento dentro do tableaux correspondente da programacao
*  linear, dado o par de indices do elemento a ser pivoteado.
* --------------------------------------------------------------------------------
*/
void pivoting (linear_programming *pl, indexes *index){
	int i, j;
	double aux = pl->mat[index->row][index->column];

	for (j=0; j < pl->n; j++){
        pl->mat[index->row][j] = (pl->mat[index->row][j])/aux;
        if (insignificance(pl->mat[index->row][j])){ pl->mat[index->row][j] = 0; }
	}

	for (i=0; i < pl->m; i++){
		if (i != index->row){
            aux = pl->mat[i][index->column];
			for (j=0; j < pl->n; j++){
				pl->mat[i][j] = pl->mat[i][j] - (aux * pl->mat[index->row][j]);
                if (insignificance(pl->mat[i][j])){ pl->mat[i][j] = 0; }
			}
		}
	}
	printPivoting (pl);
}

/* Nome: printPivoting
*  Funcao: escreve em um arquivo de texto o resultado do ultimo pivoteamento
*  realizado pelo programa (arquivo pivoting,txt) da programacao linear realizada.
* --------------------------------------------------------------------------------
*/
void printPivoting (linear_programming *pl){
    char destiny[50], sufix[13] = "_pivoting.txt";
    int w = 0, x;
    while (pl->name[w] != '\0'){ destiny[w] = pl->name[w]; w++; }
    for (x=w; x < (w+13); x++){ destiny[x] = sufix[x-w]; }
    destiny[w+13] = '\0';

	FILE *archive = fopen (destiny, "w");

	if (archive == NULL){
		printf("ERROR! Archive unavailable (WRITE)!\n");
		return;
	} else {
		int i, j;

		fprintf(archive, "[");
		for (i=0; i < pl->m; i++){
			fprintf(archive, "[");
			for (j=0; j < pl->n; j++){
				fprintf(archive, "%.5f", pl->mat[i][j]);
				if (j < (pl->n -1)){ fprintf(archive, ", "); }
			}
			fprintf(archive, "]");
			if (i < (pl->m -1)){ fprintf(archive, ", "); }
		}
		fprintf(archive, "]");
	}
	fclose (archive);
}

/* Nome: indexSimplexPrimal
*  Funcao: busca dentro do tableau o melhor elemento a ser pivoteado pelo simplex
*  primal, de acordo com as regras mostradas em aula. O indice entregue e do ele-
*  mento A[i,j], onde c[j] e menor que zero, A[i,j] e b[i] sao maiores que zero e
*  b[i]/A[i,j] e minimizado.
* --------------------------------------------------------------------------------
*/
indexes *indexSimplexPrimal (linear_programming *pl){
	indexes *index = (indexes *) malloc (sizeof(indexes));
	index->row = (-1); index->column = (-1);
	double min = MAXDOUBLE, current = MAXDOUBLE;
	int i, j;

	for (j=0; j < (pl->n -1); j++){
		if (pl->mat[0][j] < 0){
			for (i=1; i < pl->m; i++){
				if ((pl->mat[i][j] > MINDOUBLE)&&(pl->mat[i][pl->n -1] > MINDOUBLE)){
                    if (((pl->mat[i][pl->n -1])/(pl->mat[i][j])) > MINDOUBLE){
                        current = (pl->mat[i][pl->n -1])/(pl->mat[i][j]);
                    }
				}
				if (current < min){
					min = current;
					index->row = i;
					index->column = j;
				}
			}
		}
	}

	return index;
}

/* Nome: indexSimplexDual
*  Funcao: busca dentro do tableau o melhor elemento a ser pivoteado pelo simplex
*  dual, de acordo com as regras mostradas em aula. O indice entregue e do elemen
*  to A[i,j], onde b[i] e A[i,j] sao menores que zero, e a razao |c[j]|/|A[i,j]|
*  e minimizada, sendo c[j] diferente de zero.
* --------------------------------------------------------------------------------
*/
indexes *indexSimplexDual (linear_programming *pl){
	indexes *index = (indexes *) malloc (sizeof(indexes));
	index->row = (-1); index->column = (-1);
	double min = MAXDOUBLE, current = MAXDOUBLE;
	int i, j;

	for (i=1; i < pl->m; i++){
		if (pl->mat[i][pl->n -1] < 0){
			for (j=0; j < (pl->n -1); j++){
				if ((pl->mat[0][j] != 0)&&(pl->mat[i][j] < 0)){
					if (pl->mat[0][j] > MINDOUBLE){
                        if (((pl->mat[0][j])/((-1)*(pl->mat[i][j]))) > MINDOUBLE){
                            current = (pl->mat[0][j])/((-1)*(pl->mat[i][j]));
                        }
					} else {
                        if ((((-1)*(pl->mat[0][j]))/((-1)*(pl->mat[i][j]))) > MINDOUBLE){
                            current = ((-1)*(pl->mat[0][j]))/((-1)*(pl->mat[i][j]));
                        }
                    }
				}
				if (current < min){
					min = current;
					index->row = i;
					index->column = j;
				}
			}
		}
	}

	return index;
}

/* Nome: checkSimplexPrimal
*  Funcao: confere se o tableaux resultante da aplicacao do metodo do simplex
*  primal configura uma situacao onde a programacao linear e viavel porem ilimi-
*  tada, que é quando todos os valores de alguma coluna sao menores ou iguais a
*  zero, e emite o certificado d, onde Ad = 0, colocando a coluna problematica em
*  evidencia. Caso contrario, testa se o metodo chegou a uma solucao otima, reti-
*  rando o certificado y dos valores das variaveis de folga na funcao a ser oti-
*  mizada (c) e verificando se o mesmo atende (y^T)*b = vo (valor objetivo).
* --------------------------------------------------------------------------------
*/
int checkSimplexPrimal (linear_programming *pl, linear_programming *ori){
	redistribution (pl);
	int checking = UNDEFINED;

	int i, j, k, count, position;
	double checkgreat=0;

	for (j=0; j < (pl->n -1); j++){
		if (pl->mat[0][j] < 0){
			count = 0; checkgreat = (-1);
			for (i=1; i < pl->m; i++){
				if (pl->mat[i][j] > MINDOUBLE){ count ++; }
			}

			if (count == 0){
				checking = UNLIMITED;
				pl->atte = (double *) malloc ((pl->n -1)*sizeof(double));

				for (k=0; k < (pl->n -1); k++){
					if ((k != j)&&(pl->mat[0][k] == 0)){
						position = (-1);
						for (i=1; i < pl->m; i++){
							if ((pl->mat[i][k] == 0)||(pl->mat[i][k] == 1)){
								if (pl->mat[i][k] == 1){ position = i; }
							} else { i = (pl->m); }
						}

						if (position > 0){
							pl->atte[k] = (-1)*(pl->mat[position][j]);
						}
					} else {
						if (k == j){ pl->atte[k] = 1; }
						else { pl->atte[k] = 0; }
					}
				}
				j = (pl->n -1);
			}
		}
	}

	if ((checking == UNDEFINED)&&(checkgreat != (-1))){
		pl->atte = (double *) malloc ((pl->m -1)*sizeof(double));

		checkgreat=0; i=0;
		for (j=((ori->n)-(ori->m)); j < (ori->n -1); j++){
			pl->atte[i] = pl->mat[0][j];
			checkgreat += (pl->atte[i])*(ori->mat[i+1][ori->n -1]);
			i ++;
		}

		if (!(moderateInsignificance((pl->vo - checkgreat), 0.01))){
			for (i=0; i < (pl->m -1); i++){ pl->atte[i] = 0; }
		} else { checking = GREAT; }
	}

	return checking;
}

/* Nome: checkSimplexDual
*  Funcao: confere se o tableaux resultante da aplicacao do metodo do simplex
*  dual chegou ao final, com todos os valores de b e de c sendo positivos ou nu-
*  los, e neste caso confere se a solucao encontrada e otima, retirando o certi-
*  ficado y dos valores das variaveis de folga na funcao a ser otimizada (c) e
*  verificando se o mesmo atende (y^T)*b = vo (valor objetivo).
* --------------------------------------------------------------------------------
*/
int checkSimplexDual (linear_programming *pl, linear_programming *ori){
	redistribution (pl);
	int checking = UNDEFINED;

	int i, j, k, cont=0;
	double checkgreat;
	for (i=1; i < pl->m; i++){
		if (pl->mat[i][pl->n -1] < 0){ cont ++; }
	}

	if (cont == 0){
		for (i=0; i < (pl->n -1); i++){
			if (pl->mat[0][i] < 0){ cont ++; }
		}

		if (cont == 0){
			pl->atte = (double *) malloc ((pl->m -1)*sizeof(double));

			checkgreat=0; i=0;
			for (j=((pl->n)-(pl->m)); j < (pl->n -1); j++){
				pl->atte[i] = pl->mat[0][j];
				checkgreat += (pl->atte[i])*(ori->mat[i+1][ori->n -1]);
				i ++;
			}

			if (!(moderateInsignificance((pl->vo - checkgreat), 0.01))){
				for (i=0; i < (pl->m -1); i++){ pl->atte[i] = 0; }
			} else { checking = GREAT; }
		}
	}

	return checking;
}

/* Nome: auxiliaryPL
*  Funcao: cria uma nova programacao linear, baseada em uma dada anteriormente,
*  em formato auxiliar, na qual e inserida uma matriz identidade a direita da ma-
*  triz de restricoes A e a funcao a ser otimizada c e completada, configurando
*  o valor das novas variaveis como (-1).
* --------------------------------------------------------------------------------
*/
linear_programming *auxiliaryPL (linear_programming *pl){
	linear_programming *new_pl = (linear_programming *) malloc (sizeof(linear_programming));

	new_pl->m  = pl->m;
	new_pl->n  = (pl->n)+(pl->m -1);
	new_pl->vo = pl->vo;

	new_pl->mat = (double **) malloc ((new_pl->m)*sizeof(double *));
	new_pl->a   = (double **) malloc ((new_pl->m -1)*sizeof(double *));
	new_pl->b   = (double *)  malloc ((new_pl->m -1)*sizeof(double));
	new_pl->c   = (double *)  malloc ((new_pl->n -1)*sizeof(double));
	new_pl->x   = (double *)  malloc ((new_pl->m -1)*sizeof(double));

	int i, j, aux_pos = (pl->n -1), aux_stop = 0;
	for (i=0; i < new_pl->m; i++){
		new_pl->mat[i] = (double *) malloc ((new_pl->n)*sizeof(double));
		if (i != 0){ new_pl->a[i-1] = (double *) malloc ((new_pl->n -1)*sizeof(double)); }

		for (j=0; j < (new_pl->n -1); j++){
			if (j < (pl->n -1)){
				if (pl->mat[i][pl->n -1] < 0){ new_pl->mat[i][j] = (-1)*(pl->mat[i][j]); }
				else { new_pl->mat[i][j] = pl->mat[i][j]; }

				if (i==0){ new_pl->c[j] = pl->mat[i][j]; }
				else { new_pl->a[i-1][j] = pl->a[i-1][j]; }
			} else {
				if (i==0){
					new_pl->c[j] = (-1);
					new_pl->mat[i][j] = (-1);
				} else{
					if ((aux_stop == 0)&&(j == aux_pos)){
						new_pl->mat[i][j] = 1;
						new_pl->a[i-1][j] = 1;
						aux_stop = 1;
						aux_pos ++;
					} else {
						new_pl->mat[i][j] = 0;
						new_pl->a[i-1][j] = 0;
					}
				}
			}
			if (pl->mat[i][pl->n -1] < 0){
				new_pl->mat[i][new_pl->n -1] = (-1)*(pl->mat[i][pl->n -1]);
			} else { new_pl->mat[i][new_pl->n -1] = pl->mat[i][pl->n -1]; }
		}
		aux_stop = 0;
		new_pl->b[i-1] = pl->b[i-1];
		new_pl->x[i-1] = pl->x[i-1];
	}
	i=0;
	while (pl->name[i] != '\0'){ new_pl->name[i] = pl->name[i]; i++; }
	new_pl->name[i] = '\0';

	return new_pl;
}

/* Nome: SimplexPrimal
*  Funcao: execucao do metodo simplex primal, que entra em um loop para encontrar
*  elementos a serem pivoteados de acordo com suas regras, dadas por indexSim-
*  plexPrimal. Apos isto verifica o metodo chegou a uma solucao otima e salva a
*  mesma. Se uma coluna j foi pivoteada e a linha onde esta o "1" e i, entao
*  x[j] = b[i], caso contrario x[j] = 0. Também salva o certificado encontrado du-
*  rante a execucao.
* --------------------------------------------------------------------------------
*/
int simplexPrimal (linear_programming *pl){
	int checking = 0, i, j, count, position;

	linear_programming *aux = saving(pl);
	indexes *current = indexSimplexPrimal(aux);

	while ((current->row != (-1))&&(current->column != (-1))){
		pivoting (aux, current);
		current = indexSimplexPrimal(aux);
	}

	checking = checkSimplexPrimal(aux, pl);
	if (checking == GREAT){
		pl->vo = aux->vo;
		pl->atte = (double *) malloc ((pl->m -1)*sizeof(double));

		for (j=0; j < (aux->m -1); j++){
			if (aux->c[j] == 0){
				count = 0; position = (-1);
				for (i=0; i < (aux->m -1); i++){
					if (aux->a[i][j] == 1){
						count ++;
						position = i;
					} else if (aux->a[i][j] > MINDOUBLE){ count = 2; }
				}
				if (count == 1){ pl->x[j] = aux->b[position]; }
				else { pl->x[j] = 0; }
			} else { pl->x[j] = 0; }
			pl->atte[j] = aux->atte[j];
		}
	} else if (checking == UNLIMITED){
		pl->atte = (double *) malloc ((pl->n -1)*sizeof(double));
		for (j=0; j < (aux->n -1); j++){ pl->atte[j] = aux->atte[j]; }
	}

	free (aux);
	return checking;
}

/* Nome: SimplexDual
*  Funcao: execucao do metodo simplex dual, que entra em um loop para encontrar
*  elementos a serem pivoteados de acordo com suas regras, dadas por indexSim-
*  plexDual. Apos isto verifica o metodo chegou a uma solucao otima e salva a
*  mesma. Se uma coluna j foi pivoteada e a linha onde esta o "1" e i, entao
*  x[j] = b[i], caso contrario x[j] = 0. Também salva o certificado encontrado du-
*  rante a execucao. Lembrando que o valor objetivo deve ser multiplicado por (-1).
* ---------------------------------------------------------------------------------
*/
int simplexDual (linear_programming *pl){
	int checking = 0;

	linear_programming *aux = saving(pl);
	indexes *current = indexSimplexDual(aux);

	while ((current->row != (-1))&&(current->column != (-1))){
		pivoting (aux, current);
		current = indexSimplexDual(aux);
	}

	checking = checkSimplexDual(aux, pl);
	if (checking == GREAT){
		pl->vo = aux->vo;
		pl->atte = (double *) malloc ((pl->m -1)*sizeof(double));

		int i, j, count, position;
		for (j=0; j < (aux->m -1); j++){
			if (aux->c[j] == 0){
				count = 0; position = (-1);
				for (i=0; i < (aux->m -1); i++){
					if (aux->a[i][j] == 1){
						count ++;
						position = i;
					} else if (aux->a[i][j] > MINDOUBLE){ count = 2; }
				}
				if (count == 1){ pl->x[j] = aux->b[position]; }
				else { pl->x[j] = 0; }
			} else { pl->x[j] = 0; }
			pl->atte[j] = aux->atte[j];
		}
	}

	free (aux);
	return checking;
}

/* Nome: auxiliaryResolution
*  Funcao: estabelece uma respectiva programacao linear auxiliar, baseada em uma
*  outra dada anteriormente, e a soluciona. As entradas configuradas como (-1) em
*  c sao forcadas para zero atravez de pivoteamentos, e o metodo simplex Primal e
*  executado para a nova programacao linear. Ao fim, caso o valor objetivo seja
*  zero, significa que a programacao linear original e viavel, e o tableaux resul-
*  tante da auxilixar é copiado para a original, preservando apenas a funcao a ser
*  maximixada e o valor objetivo, que serao utilizados para reaplicar o simplex
*  primal. Porem, se a programacao linear for inviavel, e encontrado o certifica-
*  y, que corresponde as entradas em c das variaveis de folga.
* --------------------------------------------------------------------------------
*/
int auxiliaryResolution (linear_programming *pl){
	int i, j, checking = 0;
	linear_programming *aux = auxiliaryPL(pl);

	for (i=0; i < (aux->m); i++){
		for (j=0; j < (aux->n); j++){
			if (i == 0){ aux->mat[i][j] = (-1)*(aux->mat[i][j]); }
			else { aux->mat[0][j] -= aux->mat[i][j]; }
		}
	}

	indexes *current = indexSimplexPrimal(aux);
	while ((current->row != (-1))&&(current->column != (-1))){
		pivoting (aux, current);
		current = indexSimplexPrimal(aux);
	}

	checking = checkSimplexPrimal(aux, pl);
	if ((checking == GREAT)&&(aux->vo > MINDOUBLE)){
		setFPI(pl);
		for (i=1; i < pl->m; i++){
			for (j=0; j < pl->n; j++){
				pl->mat[i][j] = aux->mat[i][j];
			}
		}
		redistribution (pl);
	} else if ((checking == UNDEFINED)||(aux->vo <= MINDOUBLE)){
		checking = IMPRACTICABLE;
		pl->atte = (double *) malloc ((pl->m -1)*sizeof(double));
		i=0;
		for (j=((aux->n)-(2*(aux->m))+1); j < ((aux->n)-(aux->m)); j++){
			pl->atte[i] = aux->mat[0][j];
			i ++;
		}
	} else if (checking == UNLIMITED){
		pl->atte = (double *) malloc ((pl->n -1)*sizeof(double));
		for (j=0; j < (aux->n -1); j++){ pl->atte[j] = aux->atte[j]; }
	}

	free (aux);
	return checking;
}

/* Nome: insignificance
*  Funcao: verificar se um numero e demasiadamente pequeno (insignificante)
* -------------------------------------------------------------------------------
*/
int insignificance (double x){
    if ((x <= MINDOUBLE)&&(x >= 0)){ return 1; }
    else {
        if (((x >= (-1)*(MINDOUBLE)))&&(x <= 0)){ return 1;}
        else{ return 0; }
    }
}

/* Nome: moderateInsignificance
*  Funcao: verificar se um numero e pequeno em certo ponto (insignificante)
* -------------------------------------------------------------------------------
*/
int moderateInsignificance (double x, double ins){
    if ((x <= ins)&&(x >= 0)){ return 1; }
    else {
        if (((x >= (-1)*(ins)))&&(x <= 0)){ return 1;}
        else{ return 0; }
    }
}

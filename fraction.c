#include "fraction.h"

/* ------ FUNCTIONS ------ **
** ----------------------- */

/* Nome: convertString
*  Funcao: converte qualquer formato de fracao, por double ou divisao de inteiros,
*  que seja lidos como string
* --------------------------------------------------------------------------------
*/
fraction convertString (char *input){
	fraction frac;
	frac.numerator = (double) 0;
	frac.denominator = (double) 0;
	frac.value = (double) 0;

	int i = strlen(input), j = 0;
	char *aux = (char *) malloc (i * sizeof(char));
	i = 0;

	while (input[i] != '\0'){
		if (input[i] == '/'){
			aux[j] = '\0'; j = 0;
			frac.numerator = (double) atof(aux);
		} else {
			aux[j] = input[i];
			j ++;
		}
		i ++;
	}

	if (i == j){
		aux[j] = '\0';
		frac.numerator = (double) atof(aux);
		frac.value = frac.numerator;
	} else {
		aux[j] = '\0';
		frac.denominator = (double) atof(aux);
		if ((frac.numerator == 0)||(frac.denominator == 0)){ return frac; }
		frac.value = (double)(frac.numerator / frac.denominator);
	}

	return frac;
}

/* Nome: convertInt
*  Funcao: converte qualquer formato de fracao, por double ou divisao de inteiros,
*  que seja lidos como dois inteiros
* --------------------------------------------------------------------------------
*/
fraction convertInt (int num, int den){
	fraction frac; frac.value = (double) 0;
	frac.numerator = (double) num;
	frac.denominator = (double) den;
	if ((frac.numerator == 0)||(frac.denominator == 0)){ return frac; }
	frac.value = (double)(frac.numerator / frac.denominator);

	return frac;
}

/* Nome: convertDouble
*  Funcao: converte qualquer formato de fracao, por double ou divisao de inteiros,
*  que seja lidos como dois doubles
* --------------------------------------------------------------------------------
*/
fraction convertDouble (double num, double den){
	fraction frac; frac.value = (double) 0;
	frac.numerator = num; frac.denominator = den;
	if ((frac.numerator == 0)||(frac.denominator == 0)){ return frac; }
	frac.value = (double)(frac.numerator / frac.denominator);

	return frac;
}

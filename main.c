#include <stdio.h>
#include <stdlib.h>
#include "linear_programming.c"
#include "archive.c"

/* SEQUENCIA DE PASSOS:
*  (1) Carrega a programacao linear do arquivo de entrada;
*  (2) Coloca em formato FPI (formato padrao de igualdades);
*  (3) Aplica o simplex dual;
*  (4) Se nao ha resultado definido, aplica o simplex primal;
*  (5) Se nao ha resultado definido, cria a programacao linear auxilixar e resolve;
*  (6) Se e viavel, aplica o simplex primal a nova programacao linear;
*  (7) Registra os resultados;
*  (8) Libera memoria.
* ---------------------------------------------------------------------------------
*/
int main (int argc, char *argv[]){
	int checking;
	char input[50];

	scanf ("%s", input);
	linear_programming *pl = readMatrixInput (input);

    if (pl->name[0] != '.'){
        setFPI (pl);
        checking = simplexDual (pl);

        if (checking == UNDEFINED){
            checking = simplexPrimal (pl);

            if (checking == UNDEFINED){
                checking = auxiliaryResolution (pl);
                if (checking == GREAT){ checking = simplexPrimal (pl); }
            }
        }
        printConclusion (pl, checking);
    }
	free (pl);

return 0;
}

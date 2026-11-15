#ifndef QUICKSORT_H
#define QUICKSORT_H

#include <stdbool.h>

#include "lista_turnos.h"

/* Ordenacao dos participantes por iniciativa.
 * Modulo de C puro: nao depende da Raylib nem de janela. */

/* Indica se o no `a` age antes do no `b`: maior iniciativa primeiro.
 * Em empate decide o identificador do personagem (o nome), em ordem
 * alfabetica, para a ordem ser sempre a mesma. */
bool agePrimeiro(const NoTurno *a, const NoTurno *b);

/* Particiona nos[inicio..fim] em torno de um pivo (o ultimo elemento) e
 * devolve a posicao final do pivo: antes dele ficam os nos que agem antes,
 * depois ficam os que agem depois. */
int particionar(NoTurno **nos, int inicio, int fim);

/* Ordena o vetor de referencias nos[inicio..fim] (limites incluidos) por
 * iniciativa decrescente, sem usar qsort(). Os nos nao sao copiados nem
 * alterados: apenas as referencias mudam de posicao no vetor. */
void quickSort(NoTurno **nos, int inicio, int fim);

#endif

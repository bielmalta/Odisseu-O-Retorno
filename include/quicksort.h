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

/* Reorganiza a lista circular na ordem da rodada:
 * 1. copia uma volta da lista para um vetor alocado de NoTurno *;
 * 2. ordena o vetor com quickSort() (iniciativa decrescente);
 * 3. religa proximo/anterior dos MESMOS nos nessa nova ordem;
 * 4. coloca inicio e atual no primeiro participante da rodada;
 * 5. libera o vetor temporario.
 * Nenhum no e criado, copiado ou liberado. Devolve false apenas se faltar
 * memoria para o vetor; nesse caso a lista permanece exatamente como estava.
 * Lista vazia ou com um so participante devolve true sem alocar nada. */
bool ordenarPorVelocidade(ListaTurnos *lista);

#endif

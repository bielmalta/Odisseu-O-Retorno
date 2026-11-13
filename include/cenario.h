#ifndef CENARIO_H
#define CENARIO_H

#include "raylib.h"

#define CENARIO_MAX_CAIXAS 32

/* Cenario de teste para o controle em primeira pessoa (commit 03).
 * A fase base completa (objetivo, interacao, saida) vem no commit 04. */
typedef struct Cenario {
    BoundingBox caixas[CENARIO_MAX_CAIXAS]; /* paredes e obstaculos solidos */
    int quantidade;
} Cenario;

void criarCenarioTeste(Cenario *cenario);
void desenharCenario(const Cenario *cenario);

#endif

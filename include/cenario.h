#ifndef CENARIO_H
#define CENARIO_H

#include "raylib.h"

#define CENARIO_MAX_CAIXAS 32

/* Altura usada para "guardar" uma caixa removida: fica longe do mundo e
 * nunca colide nem aparece. */
#define CENARIO_Y_REMOVIDA (-1000.0f)

/* Conjunto de caixas solidas (paredes e obstaculos). O jogador colide com
 * `caixas`; `cores` so serve para o desenho. */
typedef struct Cenario {
    BoundingBox caixas[CENARIO_MAX_CAIXAS];
    Color cores[CENARIO_MAX_CAIXAS];
    int quantidade;
    Color corChao;
    Vector2 tamanhoChao;
    int divisoesGrade;
} Cenario;

void limparCenario(Cenario *cenario);

void configurarChaoCenario(
    Cenario *cenario,
    Vector2 tamanho,
    Color cor,
    int divisoesGrade
);

/* Retorna o indice da caixa criada ou -1 se nao houver espaco. */
int adicionarCaixaCenario(
    Cenario *cenario, Vector3 centro, Vector3 tamanho, Color cor
);

/* Remove a caixa (ex.: portao que se abre) sem alterar os outros indices. */
void removerCaixaCenario(Cenario *cenario, int indice);

#endif

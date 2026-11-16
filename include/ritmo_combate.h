#ifndef RITMO_COMBATE_H
#define RITMO_COMBATE_H

#include <stdbool.h>

#include "combate.h"
#include "personagem.h"

/* Temporizacao visual da interface; nao decide nem executa a acao da IA. */
#define INTERVALO_VISUAL_TURNO_INIMIGO 0.6f

typedef struct EsperaTurnoInimigo {
    const Personagem *participante;
    unsigned int rodada;
    int turnosConcluidos;
    float decorrido;
} EsperaTurnoInimigo;

void iniciarEsperaTurnoInimigo(EsperaTurnoInimigo *espera);

/* Libera a atualizacao depois do intervalo visual, sem bloquear o laco. */
bool aguardarTurnoInimigo(
    EsperaTurnoInimigo *espera,
    const Combate *combate,
    float deltaTempo
);

#endif

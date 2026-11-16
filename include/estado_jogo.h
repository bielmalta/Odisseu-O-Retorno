#ifndef ESTADO_JOGO_H
#define ESTADO_JOGO_H

/* Modos do jogo. Apenas ESTADO_EXPLORACAO libera o controle em primeira
 * pessoa; os demais estados bloqueiam o movimento e liberam o cursor. */
typedef enum EstadoJogo {
    ESTADO_MENU,
    ESTADO_EXPLORACAO,
    ESTADO_COMBATE,
    ESTADO_PAUSA,
    ESTADO_RETORNO_AO_LAR
} EstadoJogo;

#endif

#ifndef ESTADO_JOGO_H
#define ESTADO_JOGO_H

/* Modos do jogo. Apenas ESTADO_EXPLORACAO libera o controle em primeira
 * pessoa; combate e pausa bloqueiam o movimento e liberam o cursor. */
typedef enum EstadoJogo {
    ESTADO_EXPLORACAO,
    ESTADO_COMBATE,
    ESTADO_PAUSA
} EstadoJogo;

#endif

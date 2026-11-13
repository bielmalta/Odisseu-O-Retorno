#ifndef JOGADOR_H
#define JOGADOR_H

#include <stdbool.h>
#include "raylib.h"

/* Controle em primeira pessoa de Odisseu.
 * Este modulo cuida somente de movimento, camera e colisao; nao conhece
 * combate, objetivos nem HUD. */

#define JOGADOR_ALTURA_OLHOS 1.7f
#define JOGADOR_ALTURA_CORPO 1.8f
#define JOGADOR_RAIO 0.35f
#define JOGADOR_VEL_ANDAR 4.0f
#define JOGADOR_VEL_CORRER 7.0f
#define JOGADOR_FORCA_PULO 7.0f
#define JOGADOR_GRAVIDADE 20.0f
#define JOGADOR_SENSIBILIDADE 0.0025f /* radianos por pixel do mouse */
#define JOGADOR_PITCH_MAX 1.553f      /* ~89 graus */

typedef struct Jogador {
    Vector3 posicao;         /* posicao dos pes */
    float velocidadeVertical;
    float yaw;               /* rotacao horizontal (rad) */
    float pitch;             /* inclinacao vertical (rad), limitada */
    bool noChao;
    bool controleAtivo;      /* false: movimento e giro bloqueados */
    bool descartarMouse;     /* ignora o salto do mouse ao recapturar o cursor */
    Camera3D camera;
} Jogador;

/* Comandos ja lidos da entrada; a fisica nao chama a Raylib de entrada. */
typedef struct ComandosJogador {
    float frente;   /* -1..1 (W/S) */
    float lateral;  /* -1..1 (A/D) */
    float giroX;    /* delta do mouse em pixels */
    float giroY;
    bool pular;
    bool correr;
} ComandosJogador;

void inicializarJogador(Jogador *jogador, Vector3 posicaoInicial, float yaw);

/* Ativa/desativa o controle e captura/libera o cursor do mouse. */
void definirControleJogador(Jogador *jogador, bool ativo);

/* Le teclado e mouse (Raylib). */
ComandosJogador lerComandosJogador(void);

/* Atualiza fisica, colisao com as caixas e a Camera3D. Nao usa janela. */
void atualizarJogador(Jogador *jogador, ComandosJogador comandos,
                      const BoundingBox *obstaculos, int quantidade, float dt);

Vector3 direcaoOlharJogador(const Jogador *jogador);

#endif

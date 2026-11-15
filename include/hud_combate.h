#ifndef HUD_COMBATE_H
#define HUD_COMBATE_H

#include <stdbool.h>

#include "raylib.h"

#include "combate.h"
#include "fase.h"
#include "painel_combate.h"

/* HUD do combate por turnos: vida de Odisseu e dos inimigos, botoes de
 * atacar e defender, alvo selecionado e mensagens de acao.
 * Este modulo so mostra e le comandos: quem aplica as regras e o combate. */

typedef enum {
    COMANDO_NENHUM,
    COMANDO_ATACAR,
    COMANDO_DEFENDER,
    COMANDO_CONTINUAR /* depois da vitoria ou da derrota */
} ComandoCombate;

/* Entrada ja lida da Raylib; interpretarEntradaCombate() nao a chama. */
typedef struct {
    bool teclaAtacar;    /* A */
    bool teclaDefender;  /* D */
    bool teclaProximo;   /* TAB, seta direita ou para baixo */
    bool teclaAnterior;  /* seta esquerda ou para cima */
    bool teclaContinuar; /* ENTER */
    bool clique;         /* botao esquerdo do mouse apertado neste quadro */
    Vector2 mouse;
} EntradaCombate;

EntradaCombate lerEntradaCombate(void);

/* Aplica a entrada ao HUD. Troca o alvo do painel por conta propria (tecla
 * ou clique no inimigo) e devolve o comando pedido:
 *   - atacar/defender so existem na vez de Odisseu (fora dela os botoes
 *     ficam desabilitados e a entrada e ignorada); atacar exige um alvo;
 *   - continuar so existe depois de o combate acabar.
 * `largura` e `altura` sao o tamanho da janela, que define onde ficam os
 * botoes. */
ComandoCombate interpretarEntradaCombate(
    PainelCombate *painel,
    const Combate *combate,
    EntradaCombate entrada,
    int largura,
    int altura
);

/* Desenha o HUD de combate (2D, depois de EndMode3D). */
void desenharHudCombate(
    const PainelCombate *painel,
    const Combate *combate,
    const Fase *fase,
    Camera3D camera
);

#endif

#ifndef TELAS_JOGO_H
#define TELAS_JOGO_H

#include <stdbool.h>

#include "raylib.h"

/* Acoes devolvidas pelas telas. A aplicacao decide como trocar de estado e
 * como limpar ou recriar a partida. */
typedef enum AcaoTelaJogo {
    ACAO_TELA_NENHUMA,
    ACAO_TELA_JOGAR,
    ACAO_TELA_RETOMAR,
    ACAO_TELA_REINICIAR,
    ACAO_TELA_MENU,
    ACAO_TELA_SAIR
} AcaoTelaJogo;

/* Entrada desacoplada da interpretacao para permitir testes sem janela. */
typedef struct EntradaTelaJogo {
    bool confirmar;
    bool cancelar;
    bool reiniciar;
    bool menu;
    bool sair;
    bool clique;
    Vector2 mouse;
} EntradaTelaJogo;

EntradaTelaJogo lerEntradaTelaJogo(void);

AcaoTelaJogo interpretarEntradaMenu(
    EntradaTelaJogo entrada,
    int largura,
    int altura
);

AcaoTelaJogo interpretarEntradaPausa(
    EntradaTelaJogo entrada,
    bool combateEmAndamento,
    int largura,
    int altura
);

void desenharTelaMenu(int largura, int altura);
void desenharTelaPausa(int largura, int altura, bool combateEmAndamento);

#endif

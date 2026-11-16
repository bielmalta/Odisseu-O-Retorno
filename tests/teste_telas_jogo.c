#include <assert.h>
#include <stdio.h>

#include "telas_jogo.h"

enum {
    LARGURA = 1280,
    ALTURA = 720
};

static EntradaTelaJogo entradaVazia(void)
{
    return (EntradaTelaJogo){0};
}

static void testarMenuPorTeclado(void)
{
    EntradaTelaJogo entrada = entradaVazia();

    assert(interpretarEntradaMenu(entrada, LARGURA, ALTURA) ==
           ACAO_TELA_NENHUMA);

    entrada.confirmar = true;
    assert(interpretarEntradaMenu(entrada, LARGURA, ALTURA) ==
           ACAO_TELA_JOGAR);

    entrada = entradaVazia();
    entrada.cancelar = true;
    assert(interpretarEntradaMenu(entrada, LARGURA, ALTURA) ==
           ACAO_TELA_SAIR);

    entrada = entradaVazia();
    entrada.sair = true;
    assert(interpretarEntradaMenu(entrada, LARGURA, ALTURA) ==
           ACAO_TELA_SAIR);
}

static void testarMenuPorClique(void)
{
    EntradaTelaJogo entrada = entradaVazia();

    entrada.clique = true;
    entrada.mouse = (Vector2){ 640.0f, 350.0f };
    assert(interpretarEntradaMenu(entrada, LARGURA, ALTURA) ==
           ACAO_TELA_JOGAR);

    entrada.mouse = (Vector2){ 640.0f, 420.0f };
    assert(interpretarEntradaMenu(entrada, LARGURA, ALTURA) ==
           ACAO_TELA_SAIR);
}

static void testarPausaDeCombate(void)
{
    EntradaTelaJogo entrada = entradaVazia();

    entrada.cancelar = true;
    assert(interpretarEntradaPausa(entrada, true, LARGURA, ALTURA) ==
           ACAO_TELA_RETOMAR);

    entrada = entradaVazia();
    entrada.reiniciar = true;
    assert(interpretarEntradaPausa(entrada, true, LARGURA, ALTURA) ==
           ACAO_TELA_REINICIAR);

    entrada = entradaVazia();
    entrada.menu = true;
    assert(interpretarEntradaPausa(entrada, true, LARGURA, ALTURA) ==
           ACAO_TELA_MENU);

    entrada = entradaVazia();
    entrada.clique = true;
    entrada.mouse = (Vector2){ 640.0f, 380.0f };
    assert(interpretarEntradaPausa(entrada, true, LARGURA, ALTURA) ==
           ACAO_TELA_REINICIAR);

    entrada.mouse = (Vector2){ 640.0f, 450.0f };
    assert(interpretarEntradaPausa(entrada, true, LARGURA, ALTURA) ==
           ACAO_TELA_MENU);
}

static void testarPausaDeExploracao(void)
{
    EntradaTelaJogo entrada = entradaVazia();

    entrada.reiniciar = true;
    assert(interpretarEntradaPausa(entrada, false, LARGURA, ALTURA) ==
           ACAO_TELA_NENHUMA);

    entrada = entradaVazia();
    entrada.clique = true;
    entrada.mouse = (Vector2){ 640.0f, 350.0f };
    assert(interpretarEntradaPausa(entrada, false, LARGURA, ALTURA) ==
           ACAO_TELA_RETOMAR);

    entrada.mouse = (Vector2){ 640.0f, 420.0f };
    assert(interpretarEntradaPausa(entrada, false, LARGURA, ALTURA) ==
           ACAO_TELA_MENU);
}

int main(void)
{
    testarMenuPorTeclado();
    testarMenuPorClique();
    testarPausaDeCombate();
    testarPausaDeExploracao();

    puts("Testes das telas de menu e pausa passaram.");
    return 0;
}

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "acoes_combate.h"
#include "combate.h"
#include "painel_combate.h"

/* Odisseu e dois inimigos prontos para lutar; o painel le a lista. */
typedef struct {
    Combate combate;
    PainelCombate painel;
    Personagem odisseu;
    Personagem sereia;
    Personagem ciclope;
} Encontro;

static void montarEncontro(Encontro *e)
{
    Personagem *inimigos[2];

    e->odisseu = criarPersonagem("Odisseu", TIPO_HEROI, 100, 20, 10, 12);
    e->sereia = criarPersonagem("Sereia", TIPO_INIMIGO, 60, 15, 5, 15);
    e->ciclope = criarPersonagem("Ciclope", TIPO_INIMIGO, 120, 25, 8, 8);
    inimigos[0] = &e->sereia;
    inimigos[1] = &e->ciclope;

    inicializarCombate(&e->combate);
    assert(prepararCombate(&e->combate, &e->odisseu, inimigos, 2));
    iniciarPainelCombate(&e->painel, &e->combate);
}

static void testarInicio(void)
{
    Encontro e;

    montarEncontro(&e);

    /* So os inimigos entram, na ordem em que foram inseridos. */
    assert(e.painel.quantidadeInimigos == 2);
    assert(e.painel.inimigos[0] == &e.sereia);
    assert(e.painel.inimigos[1] == &e.ciclope);
    assert(e.painel.alvo == &e.sereia);
    assert(e.painel.quantidadeMensagens == 0);

    finalizarCombate(&e.combate);

    /* Argumentos nulos nao quebram nada. */
    iniciarPainelCombate(NULL, &e.combate);
    iniciarPainelCombate(&e.painel, NULL);
    assert(e.painel.quantidadeInimigos == 0);
    assert(e.painel.alvo == NULL);
}

static void testarTrocaDeAlvo(void)
{
    Encontro e;

    montarEncontro(&e);

    mudarAlvoPainel(&e.painel, 1);
    assert(e.painel.alvo == &e.ciclope);
    mudarAlvoPainel(&e.painel, 1); /* volta ao comeco */
    assert(e.painel.alvo == &e.sereia);
    mudarAlvoPainel(&e.painel, -1); /* e ao fim */
    assert(e.painel.alvo == &e.ciclope);
    mudarAlvoPainel(&e.painel, 0);
    assert(e.painel.alvo == &e.ciclope);

    /* Por indice: so inimigos vivos e existentes. */
    assert(selecionarAlvoPainel(&e.painel, 0));
    assert(e.painel.alvo == &e.sereia);
    assert(!selecionarAlvoPainel(&e.painel, 2));
    assert(!selecionarAlvoPainel(&e.painel, -1));
    assert(e.painel.alvo == &e.sereia);

    e.ciclope.vida = 0;
    assert(!selecionarAlvoPainel(&e.painel, 1));
    assert(e.painel.alvo == &e.sereia);

    /* Com um so inimigo vivo, trocar mantem o alvo. */
    mudarAlvoPainel(&e.painel, 1);
    assert(e.painel.alvo == &e.sereia);
    mudarAlvoPainel(&e.painel, -1);
    assert(e.painel.alvo == &e.sereia);

    mudarAlvoPainel(NULL, 1);
    assert(!selecionarAlvoPainel(NULL, 0));

    finalizarCombate(&e.combate);
}

static void testarAlvoDerrotado(void)
{
    Encontro e;

    montarEncontro(&e);

    /* O alvo cai: o painel passa ao proximo inimigo vivo. */
    e.sereia.vida = 0;
    atualizarAlvoPainel(&e.painel);
    assert(e.painel.alvo == &e.ciclope);

    /* Um alvo vivo nao e trocado a toa. */
    atualizarAlvoPainel(&e.painel);
    assert(e.painel.alvo == &e.ciclope);

    /* Sem inimigos vivos nao ha alvo, e trocar nao quebra. */
    e.ciclope.vida = 0;
    atualizarAlvoPainel(&e.painel);
    assert(e.painel.alvo == NULL);
    mudarAlvoPainel(&e.painel, 1);
    mudarAlvoPainel(&e.painel, -1);
    assert(e.painel.alvo == NULL);

    /* Os derrotados continuam na lista do painel, para o HUD mostra-los. */
    assert(e.painel.quantidadeInimigos == 2);

    finalizarCombate(&e.combate);
}

static void testarAlvoSemSelecao(void)
{
    Encontro e;

    montarEncontro(&e);

    /* Sem alvo atual, o passo comeca pela ponta de onde sai. */
    e.painel.alvo = NULL;
    mudarAlvoPainel(&e.painel, 1);
    assert(e.painel.alvo == &e.sereia);

    e.painel.alvo = NULL;
    mudarAlvoPainel(&e.painel, -1);
    assert(e.painel.alvo == &e.ciclope);

    finalizarCombate(&e.combate);
}

static void testarMensagens(void)
{
    PainelCombate painel;
    char esperado[PAINEL_TAMANHO_MENSAGEM];

    iniciarPainelCombate(&painel, NULL);

    registrarMensagemPainel(&painel, "primeira");
    registrarMensagemPainel(&painel, "segunda");
    assert(painel.quantidadeMensagens == 2);
    assert(strcmp(painel.mensagens[0], "primeira") == 0);
    assert(strcmp(painel.mensagens[1], "segunda") == 0);

    /* Ao encher, a mais antiga sai. */
    for (int i = 0; i < PAINEL_MAX_MENSAGENS + 2; i++) {
        snprintf(esperado, sizeof esperado, "mensagem %d", i);
        registrarMensagemPainel(&painel, esperado);
    }
    assert(painel.quantidadeMensagens == PAINEL_MAX_MENSAGENS);
    assert(strcmp(painel.mensagens[0], "mensagem 2") == 0);
    assert(strcmp(painel.mensagens[PAINEL_MAX_MENSAGENS - 1], "mensagem 6") == 0);

    /* Texto maior que o espaco e cortado, sem estourar. */
    char comprido[300];
    memset(comprido, 'x', sizeof comprido - 1);
    comprido[sizeof comprido - 1] = '\0';
    registrarMensagemPainel(&painel, comprido);
    assert(strlen(painel.mensagens[PAINEL_MAX_MENSAGENS - 1]) ==
           PAINEL_TAMANHO_MENSAGEM - 1);

    registrarMensagemPainel(&painel, NULL);
    registrarMensagemPainel(NULL, "x");
}

static void testarMensagensDosEventos(void)
{
    Encontro e;

    montarEncontro(&e);
    registrarObservadorCombate(observarEventoPainel, &e.painel);

    /* Ataque comum: 20 - 5 = 15. */
    assert(realizarAtaque(&e.odisseu, &e.sereia) == 15);
    assert(e.painel.quantidadeMensagens == 1);
    assert(strcmp(e.painel.mensagens[0], "Odisseu atacou Sereia: 15 de dano") == 0);

    /* Defesa. */
    assert(realizarDefesa(&e.ciclope));
    assert(strcmp(e.painel.mensagens[1], "Ciclope se defendeu") == 0);

    /* Golpe contra quem se defendeu: 20 - 8 - 5 = 7. */
    assert(realizarAtaque(&e.odisseu, &e.ciclope) == 7);
    assert(strcmp(e.painel.mensagens[2],
                  "Odisseu atacou Ciclope: 7 de dano (defendido)") == 0);

    /* Golpe final: duas mensagens, o ataque e a derrota. */
    e.sereia.vida = 4;
    assert(realizarAtaque(&e.odisseu, &e.sereia) == 4);
    assert(e.painel.quantidadeMensagens == 5);
    assert(strcmp(e.painel.mensagens[3], "Odisseu atacou Sereia: 4 de dano") == 0);
    assert(strcmp(e.painel.mensagens[4], "Sereia foi derrotado!") == 0);

    /* O painel acompanha: o alvo derrotado troca de inimigo. */
    atualizarAlvoPainel(&e.painel);
    assert(e.painel.alvo == &e.ciclope);

    /* Evento sem contexto ou nulo e ignorado. */
    observarEventoPainel(NULL, &e.painel);
    observarEventoPainel(NULL, NULL);
    assert(e.painel.quantidadeMensagens == 5);

    registrarObservadorCombate(NULL, NULL);
    finalizarCombate(&e.combate);
}

static void testarMensagensNoFluxoDoCombate(void)
{
    Encontro e;

    montarEncontro(&e);
    registrarObservadorCombate(observarEventoPainel, &e.painel);

    atualizarCombate(&e.combate); /* inicia a rodada: Sereia age primeiro */
    assert(e.combate.estado == COMBATE_ENEMY_TURN);
    atualizarCombate(&e.combate); /* o inimigo age sozinho */
    assert(e.painel.quantidadeMensagens == 1);
    assert(strncmp(e.painel.mensagens[0], "Sereia atacou Odisseu:", 22) == 0);

    atualizarCombate(&e.combate); /* resolve; vez de Odisseu */
    assert(e.combate.estado == COMBATE_WAITING_PLAYER);
    assert(atacarNoCombate(&e.combate, e.painel.alvo));
    assert(e.painel.quantidadeMensagens == 2);
    assert(strncmp(e.painel.mensagens[1], "Odisseu atacou Sereia:", 22) == 0);

    registrarObservadorCombate(NULL, NULL);
    finalizarCombate(&e.combate);
}

int main(void)
{
    testarInicio();
    testarTrocaDeAlvo();
    testarAlvoDerrotado();
    testarAlvoSemSelecao();
    testarMensagens();
    testarMensagensDosEventos();
    testarMensagensNoFluxoDoCombate();

    printf("\nTESTE DO PAINEL DE COMBATE PASSOU!\n");

    return 0;
}

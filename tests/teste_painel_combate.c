#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "acoes_combate.h"
#include "combate.h"
#include "lista_turnos.h"
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

/* Criterio de aceite da ordem de turnos: o que o HUD mostra e a ordem em que
 * avancarTurno() percorre os nos da lista, a partir do inicio. */
static void conferirOrdemCoincideComAvancarTurno(Combate *combate)
{
    ListaTurnos *lista = &combate->turnos;
    NoTurno *salvo = lista->atual;
    TurnoPainel ordem[PAINEL_MAX_TURNOS];
    const int quantidade = montarOrdemTurnos(combate, ordem, PAINEL_MAX_TURNOS);

    assert(quantidade >= 1);
    assert(quantidade == lista->tamanho);

    lista->atual = lista->inicio;
    assert(personagemDaVez(lista) == ordem[0].personagem);
    for (int i = 1; i < quantidade; i++) {
        assert(avancarTurno(lista) == ordem[i].personagem);
    }
    assert(avancarTurno(lista) == ordem[0].personagem); /* circular */

    lista->atual = salvo;
}

static void conferirSituacoes(
    const Combate *combate,
    const Personagem *const *esperados,
    const SituacaoTurno *situacoes,
    int quantidade
)
{
    TurnoPainel ordem[PAINEL_MAX_TURNOS];

    assert(montarOrdemTurnos(combate, ordem, PAINEL_MAX_TURNOS) == quantidade);
    for (int i = 0; i < quantidade; i++) {
        assert(ordem[i].personagem == esperados[i]);
        assert(ordem[i].situacao == situacoes[i]);
    }
}

static void agirComoJogador(Encontro *e)
{
    bool agiu;

    atualizarAlvoPainel(&e->painel);
    agiu = atacarNoCombate(&e->combate, e->painel.alvo);
    assert(agiu);
    (void)agiu;
}

static void testarOrdemAcompanhaOQuickSort(void)
{
    Encontro e;
    const Personagem *antes[] = {&e.odisseu, &e.sereia, &e.ciclope};
    const Personagem *depois[] = {&e.sereia, &e.odisseu, &e.ciclope};
    const SituacaoTurno aguardando[] = {
        TURNO_AGUARDANDO, TURNO_AGUARDANDO, TURNO_AGUARDANDO
    };
    const SituacaoTurno primeiroDaVez[] = {
        TURNO_ATUAL, TURNO_AGUARDANDO, TURNO_AGUARDANDO
    };

    montarEncontro(&e);

    /* Antes de a rodada comecar: a ordem de insercao, ninguem na vez. */
    assert(e.combate.participanteAtual == NULL);
    conferirSituacoes(&e.combate, antes, aguardando, 3);
    conferirOrdemCoincideComAvancarTurno(&e.combate);

    /* O inicio da rodada roda o QuickSort: Sereia (15), Odisseu (12), Ciclope (8). */
    atualizarCombate(&e.combate);
    assert(e.combate.participanteAtual == &e.sereia);
    conferirSituacoes(&e.combate, depois, primeiroDaVez, 3);
    conferirOrdemCoincideComAvancarTurno(&e.combate);

    finalizarCombate(&e.combate);

    /* Entradas ou capacidade invalidas nao escrevem nada. */
    assert(montarOrdemTurnos(NULL, NULL, 3) == 0);
    {
        TurnoPainel ordem[PAINEL_MAX_TURNOS];

        assert(montarOrdemTurnos(&e.combate, ordem, 0) == 0);
        assert(montarOrdemTurnos(&e.combate, ordem, PAINEL_MAX_TURNOS) == 0);
    }
}

static void testarSituacaoAoLongoDaRodada(void)
{
    Encontro e;
    const Personagem *ordem[] = {&e.sereia, &e.odisseu, &e.ciclope};

    montarEncontro(&e);
    registrarObservadorCombate(NULL, NULL);

    atualizarCombate(&e.combate); /* rodada 1: vez da Sereia */
    {
        const SituacaoTurno s[] = {TURNO_ATUAL, TURNO_AGUARDANDO, TURNO_AGUARDANDO};
        conferirSituacoes(&e.combate, ordem, s, 3);
    }

    atualizarCombate(&e.combate); /* a Sereia age */
    {
        /* Acabou de agir, mas a vez so passa na proxima atualizacao. */
        const SituacaoTurno s[] = {TURNO_ATUAL, TURNO_AGUARDANDO, TURNO_AGUARDANDO};
        conferirSituacoes(&e.combate, ordem, s, 3);
    }

    atualizarCombate(&e.combate); /* vez de Odisseu */
    assert(e.combate.estado == COMBATE_WAITING_PLAYER);
    {
        const SituacaoTurno s[] = {TURNO_JA_AGIU, TURNO_ATUAL, TURNO_AGUARDANDO};
        conferirSituacoes(&e.combate, ordem, s, 3);
    }
    conferirOrdemCoincideComAvancarTurno(&e.combate);

    agirComoJogador(&e);
    atualizarCombate(&e.combate); /* vez do Ciclope */
    assert(e.combate.participanteAtual == &e.ciclope);
    {
        const SituacaoTurno s[] = {TURNO_JA_AGIU, TURNO_JA_AGIU, TURNO_ATUAL};
        conferirSituacoes(&e.combate, ordem, s, 3);
    }

    atualizarCombate(&e.combate); /* o Ciclope age */
    atualizarCombate(&e.combate); /* fim da rodada */
    assert(e.combate.estado == COMBATE_ROUND_START);
    {
        const SituacaoTurno s[] = {
            TURNO_AGUARDANDO, TURNO_AGUARDANDO, TURNO_AGUARDANDO
        };
        conferirSituacoes(&e.combate, ordem, s, 3);
    }

    finalizarCombate(&e.combate);
}

static void testarOrdemAposRemocao(void)
{
    Encontro e;
    const Personagem *ordem[] = {&e.sereia, &e.odisseu, &e.ciclope};
    const Personagem *semSereia[] = {&e.odisseu, &e.ciclope};

    montarEncontro(&e);
    e.sereia.vida = 4; /* cai no primeiro golpe */

    atualizarCombate(&e.combate); /* vez da Sereia */
    atualizarCombate(&e.combate); /* ela age */
    atualizarCombate(&e.combate); /* vez de Odisseu */
    {
        const SituacaoTurno s[] = {TURNO_JA_AGIU, TURNO_ATUAL, TURNO_AGUARDANDO};
        conferirSituacoes(&e.combate, ordem, s, 3);
    }

    /* Odisseu derrota a Sereia, que ja tinha agido: ela sai da lista e do HUD. */
    agirComoJogador(&e);
    assert(e.sereia.vida == 0);
    {
        const SituacaoTurno s[] = {TURNO_ATUAL, TURNO_AGUARDANDO};
        conferirSituacoes(&e.combate, semSereia, s, 2);
    }
    conferirOrdemCoincideComAvancarTurno(&e.combate);

    /* O alvo selecionado tambem passa ao inimigo que resta. */
    atualizarAlvoPainel(&e.painel);
    assert(e.painel.alvo == &e.ciclope);

    /* A proxima rodada segue com os dois que restam: Odisseu (12), Ciclope (8). */
    for (int passo = 0; passo < 20 && e.combate.rodada < 2; passo++) {
        atualizarCombate(&e.combate);
    }
    assert(e.combate.rodada == 2);
    {
        const SituacaoTurno s[] = {TURNO_ATUAL, TURNO_AGUARDANDO};
        conferirSituacoes(&e.combate, semSereia, s, 2);
    }
    conferirOrdemCoincideComAvancarTurno(&e.combate);

    finalizarCombate(&e.combate);
}

/* Joga uma rodada inteira conferindo, a cada turno, que o participante da
 * vez e o que o HUD mostra naquela posicao. */
static void jogarRodadaConferindoOrdem(Encontro *e, const Personagem **esperada, int n)
{
    TurnoPainel ordem[PAINEL_MAX_TURNOS];

    assert(e->combate.estado == COMBATE_ROUND_START);
    atualizarCombate(&e->combate); /* comeca a rodada e ordena */
    assert(montarOrdemTurnos(&e->combate, ordem, PAINEL_MAX_TURNOS) == n);

    for (int i = 0; i < n; i++) {
        TurnoPainel agora[PAINEL_MAX_TURNOS];

        assert(ordem[i].personagem == esperada[i]);
        assert(e->combate.participanteAtual == esperada[i]);

        assert(montarOrdemTurnos(&e->combate, agora, PAINEL_MAX_TURNOS) == n);
        for (int j = 0; j < n; j++) {
            assert(agora[j].personagem == esperada[j]);
            assert(agora[j].situacao == (j < i ? TURNO_JA_AGIU
                                       : j == i ? TURNO_ATUAL : TURNO_AGUARDANDO));
        }
        conferirOrdemCoincideComAvancarTurno(&e->combate);

        if (e->combate.estado == COMBATE_WAITING_PLAYER) {
            agirComoJogador(e);
        } else {
            atualizarCombate(&e->combate); /* o inimigo age */
        }
        atualizarCombate(&e->combate); /* passa a vez (ou fecha a rodada) */
    }

    assert(e->combate.estado == COMBATE_ROUND_START);
}

static void testarOrdemMudaComAVelocidade(void)
{
    Encontro e;
    const Personagem *rodada1[] = {&e.sereia, &e.odisseu, &e.ciclope};
    const Personagem *rodada2[] = {&e.ciclope, &e.sereia, &e.odisseu};
    const Personagem *rodada3[] = {&e.odisseu, &e.ciclope, &e.sereia};

    montarEncontro(&e);
    registrarObservadorCombate(NULL, NULL);

    jogarRodadaConferindoOrdem(&e, rodada1, 3);

    /* Mudar a velocidade muda a posicao no HUD na rodada seguinte. */
    e.ciclope.iniciativa = 20;
    jogarRodadaConferindoOrdem(&e, rodada2, 3);

    e.odisseu.iniciativa = 30;
    jogarRodadaConferindoOrdem(&e, rodada3, 3);

    assert(e.combate.rodada == 3);
    assert(personagemVivo(&e.odisseu));

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
    testarOrdemAcompanhaOQuickSort();
    testarSituacaoAoLongoDaRodada();
    testarOrdemAposRemocao();
    testarOrdemMudaComAVelocidade();

    printf("\nTESTE DO PAINEL DE COMBATE PASSOU!\n");

    return 0;
}

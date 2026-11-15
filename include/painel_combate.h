#ifndef PAINEL_COMBATE_H
#define PAINEL_COMBATE_H

#include <stdbool.h>

#include "acoes_combate.h"
#include "combate.h"
#include "personagem.h"

/* Dados que o HUD de combate precisa mostrar: os inimigos do encontro, o
 * alvo selecionado e as ultimas mensagens de acao.
 * Modulo de C puro (sem Raylib): o desenho e a leitura de teclado e mouse
 * ficam em hud_combate.h. */

#define PAINEL_MAX_INIMIGOS 4
#define PAINEL_MAX_MENSAGENS 5
#define PAINEL_TAMANHO_MENSAGEM 96

typedef struct {
    /* Inimigos na ordem em que entraram no combate; a posicao i corresponde
     * a Fase.pontosInimigos[i]. Derrotados continuam aqui, para o HUD
     * mostra-los como derrotados. */
    Personagem *inimigos[PAINEL_MAX_INIMIGOS];
    int quantidadeInimigos;

    Personagem *alvo; /* inimigo selecionado (vivo), ou NULL */

    /* Mensagens de acao, da mais antiga para a mais nova. */
    char mensagens[PAINEL_MAX_MENSAGENS][PAINEL_TAMANHO_MENSAGEM];
    int quantidadeMensagens;
} PainelCombate;

/* Zera o painel e le os inimigos do combate. Chamar logo depois de
 * prepararCombate(), antes de a lista ser ordenada. Seleciona o primeiro
 * inimigo vivo. */
void iniciarPainelCombate(PainelCombate *painel, const Combate *combate);

/* Mantem o alvo valido: se o selecionado foi derrotado, passa para o proximo
 * inimigo vivo (ou NULL se nao restar nenhum). */
void atualizarAlvoPainel(PainelCombate *painel);

/* Seleciona o inimigo da posicao `indice`. Devolve false, sem mudar nada,
 * se a posicao nao existe ou o inimigo esta derrotado. */
bool selecionarAlvoPainel(PainelCombate *painel, int indice);

/* Passa ao proximo (passo > 0) ou ao anterior (passo < 0) inimigo vivo, de
 * forma circular. */
void mudarAlvoPainel(PainelCombate *painel, int passo);

/* Acrescenta uma mensagem; ao encher, descarta a mais antiga. */
void registrarMensagemPainel(PainelCombate *painel, const char *texto);

/* Observador de eventos do combate (registrarObservadorCombate): transforma
 * cada ataque, defesa e derrota em mensagem. `contexto` e o PainelCombate. */
void observarEventoPainel(const EventoCombate *evento, void *contexto);

/* ---- Ordem de turnos ---- */

/* Odisseu e todos os inimigos que cabem no painel. */
#define PAINEL_MAX_TURNOS (PAINEL_MAX_INIMIGOS + 1)

typedef enum {
    TURNO_JA_AGIU,    /* antes do participante da vez, nesta rodada */
    TURNO_ATUAL,      /* e a vez dele (ou acabou de agir, ate a vez passar) */
    TURNO_AGUARDANDO  /* ainda nao agiu nesta rodada */
} SituacaoTurno;

typedef struct {
    const Personagem *personagem;
    SituacaoTurno situacao;
} TurnoPainel;

/* Le a ordem da rodada direto da lista circular de turnos (a mesma ordem em
 * que avancarTurno() percorre os nos a partir de `inicio`) e marca quem ja
 * agiu, quem esta na vez e quem espera. Como a ordem e lida da lista a cada
 * chamada, ela acompanha o QuickSort do inicio da rodada e a remocao dos
 * derrotados sem nenhuma copia para manter em dia.
 * Escreve ate `capacidade` entradas (no maximo PAINEL_MAX_TURNOS) e devolve
 * quantas escreveu. Entre rodadas, sem participante da vez, todos aparecem
 * como TURNO_AGUARDANDO. */
int montarOrdemTurnos(const Combate *combate, TurnoPainel *saida, int capacidade);

#endif

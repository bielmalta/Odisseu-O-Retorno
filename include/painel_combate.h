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

#endif

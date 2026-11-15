#ifndef FASE_H
#define FASE_H

#include <stdbool.h>

#include "cenario.h"
#include "raylib.h"

#define FASE_MAX_ITENS 8
#define FASE_MAX_INIMIGOS 4
#define FASE_TAMANHO_MENSAGEM 96

/* Maquina de estados da fase:
 * Exploracao -> Confronto -> Saida liberada -> Concluida.
 * (O objetivo cumprido dispara o confronto na mesma atualizacao.) */
typedef enum EtapaFase {
    ETAPA_EXPLORACAO,
    ETAPA_CONFRONTO,
    ETAPA_SAIDA_LIBERADA,
    ETAPA_CONCLUIDA
} EtapaFase;

typedef struct ItemFase {
    Vector3 posicao; /* centro do item */
    bool coletado;
} ItemFase;

typedef struct ObjetivoFase {
    const char *descricao;
    const char *nomeItem;
    int progresso;
    int necessario;
    bool cumprido;
} ObjetivoFase;

typedef struct Fase {
    const char *titulo;
    Cenario cenario;

    Vector3 pontoInicial;
    BoundingBox saida;
    int indicePortao; /* caixa do cenario que fecha a saida */

    ItemFase itens[FASE_MAX_ITENS];
    int quantidadeItens;
    int itemAlvo; /* item no centro da mira, ou -1 */

    ObjetivoFase objetivo;

    /* Posicionamento do confronto: o jogador e colocado em
     * `pontoJogadorConfronto`, de frente para os inimigos. */
    Vector3 pontoJogadorConfronto;
    Vector3 pontosInimigos[FASE_MAX_INIMIGOS];
    int quantidadeInimigos;

    EtapaFase etapa;

    char mensagem[FASE_TAMANHO_MENSAGEM];
    float tempoMensagem;
} Fase;

/* Monta a fase base (Caverna de Polifemo, provisoria). */
void criarFaseBase(Fase *fase);

/* Atualiza qual item esta no centro da mira (raio lancado da camera). */
void atualizarMiraFase(Fase *fase, Ray raio);

/* Executa a acao do objeto observado. Retorna true se algo foi feito. */
bool interagir(Fase *fase);

/* Tempo das mensagens e chegada a saida. */
void atualizarFase(Fase *fase, Vector3 posicaoJogador, float deltaTempo);

/* Fim do confronto: libera a saida. Sera chamado pelo combate quando ele
 * existir; por enquanto e acionado por uma tecla de teste. */
void encerrarConfronto(Fase *fase);

#endif

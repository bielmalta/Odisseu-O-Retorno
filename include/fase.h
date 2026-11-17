#ifndef FASE_H
#define FASE_H

#include <stdbool.h>

#include "cenario.h"
#include "configuracao_fases.h"
#include "objetivo.h"
#include "raylib.h"

#define FASE_MAX_ITENS 8
#define FASE_MAX_INIMIGOS 4
#define FASE_TAMANHO_MENSAGEM 96

#define SEREIAS_ENCANTO_MAXIMO 100.0f
#define CILA_INTERVALO_ATAQUE 14.0f
#define ITACA_SUSPEITA_MAXIMA 100.0f

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
    bool concluido; /* ja foi coletado ou ativado */
} ItemFase;

typedef enum RotaItaca {
    ROTA_ITACA_NENHUMA,
    ROTA_ITACA_FURTIVA,
    ROTA_ITACA_DIRETA
} RotaItaca;

typedef struct Fase {
    const ConfiguracaoFase *configuracao;
    const char *titulo;
    Cenario cenario;

    Vector3 pontoInicial;
    BoundingBox saida;
    int indicePortao; /* caixa do cenario que fecha a saida */

    ItemFase itens[FASE_MAX_ITENS];
    int quantidadeItens;
    int itemAlvo; /* item no centro da mira, ou -1 */

    Objetivo objetivo;

    /* Posicionamento do confronto: o jogador e colocado em
     * `pontoJogadorConfronto`, de frente para os inimigos. */
    Vector3 pontoJogadorConfronto;
    Vector3 pontosInimigos[FASE_MAX_INIMIGOS];
    int quantidadeInimigos;

    EtapaFase etapa;

    char mensagem[FASE_TAMANHO_MENSAGEM];
    float tempoMensagem;

    /* Estado das mecanicas exclusivas de exploracao. */
    float tempoMecanica;
    float medidorPerigo;
    float tempoEvento;
    float tempoImpactoVisual;
    int indiceEvento;
    Vector3 posicaoAmeaca;
    Vector3 patrulhasItaca[2];
    RotaItaca rotaItaca;
    bool detectadoEmItaca;
} Fase;

/* Monta uma fase a partir de uma entrada da tabela de configuracoes. */
bool criarFase(Fase *fase, const ConfiguracaoFase *configuracao);

/* Atalho temporario para a primeira entrada da tabela. */
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

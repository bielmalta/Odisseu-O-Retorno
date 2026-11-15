#ifndef ANIMACAO_COMBATE_H
#define ANIMACAO_COMBATE_H

#include <stdbool.h>

#include "acoes_combate.h"
#include "personagem.h"

/* Linha do tempo das animacoes do combate.
 *
 * O dano e sempre resolvido pela logica (acoes_combate.c), na hora da acao.
 * Este modulo so conta o tempo do que a tela mostra: cada acao recebida como
 * evento vira uma animacao com COMECO, IMPACTO e RETORNO, e o modulo informa
 * (sem tocar em vida, turnos ou dano):
 *   - os marcos que devem disparar sons e mensagens (CueAnimacao);
 *   - a pose de Odisseu em primeira pessoa e a pose de cada inimigo;
 *   - a vida que a barra deve mostrar enquanto o golpe ainda nao acertou.
 * Modulo de C puro: nao usa a Raylib. */

/* Instantes e duracoes, em segundos. */
#define ANIM_INICIO_ESTOCADA 0.25f    /* a lanca comeca a avancar */
#define ANIM_TEMPO_IMPACTO 0.35f      /* o golpe acerta */
#define ANIM_FIM_RETORNO 0.70f        /* a lanca volta ao repouso */
#define ANIM_DURACAO_ATAQUE 0.85f     /* ataque comum, do comeco ao retorno */
#define ANIM_DURACAO_DEFESA 0.60f
#define ANIM_DURACAO_QUEDA_INIMIGO 1.00f
#define ANIM_DURACAO_QUEDA_JOGADOR 1.20f
#define ANIM_ATRASO_SOM_QUEDA 0.15f   /* depois do impacto */

/* Quanto o escudo de Odisseu fica levantado enquanto ele esta em guarda. */
#define ANIM_ESCUDO_GUARDA 0.75f

/* Acoes pendentes que o modulo guarda; so passa de 1 se alguem ignorar o
 * bloqueio dos turnos (o combate espera a animacao terminar). */
#define ANIM_FILA_MAX 4

typedef enum {
    CUE_INICIO_GOLPE,    /* o golpe comeca (som de movimento) */
    CUE_IMPACTO,         /* o golpe acerta; evento.alvoDefendia diz se foi bloqueado */
    CUE_DEFESA_ATIVADA,  /* alguem levantou a guarda */
    CUE_QUEDA            /* alguem foi derrotado e esta tombando */
} TipoCue;

/* Marco da animacao. Carrega o evento da acao para quem for tocar o som ou
 * publicar a mensagem no mesmo instante. */
typedef struct {
    TipoCue tipo;
    EventoCombate evento;
} CueAnimacao;

/* Uma acao em animacao. */
typedef struct {
    EventoCombate evento;
    int vidaAntes;  /* vida do alvo antes do golpe */
    float tempo;
    float duracao;
    bool iniciou;
    bool impactou;
    bool caiu;
} AcaoAnimada;

typedef struct {
    AcaoAnimada fila[ANIM_FILA_MAX]; /* a primeira e a que esta tocando */
    int quantidade;
} AnimacaoCombate;

/* Pose de um inimigo; todos os valores ficam em [0, 1]. */
typedef struct {
    float preparo;  /* recuo antes de investir */
    float avanco;   /* investida em direcao a Odisseu */
    float recuo;    /* empurrao ao ser atingido */
    float agachar;  /* postura de defesa */
    float queda;    /* 0 em pe, 1 tombado */
    float brilho;   /* lampejo do impacto */
} PoseInimigo;

/* Pose de Odisseu em primeira pessoa. */
typedef struct {
    float estocada;       /* [-1, 1]: -1 lanca recuada, +1 lanca no impacto */
    float escudo;         /* [0, 1]: escudo levantado */
    float tremor;         /* [0, 1]: camera tremendo (Odisseu atingido) */
    float lampejoDano;    /* [0, 1]: vinheta vermelha */
    float lampejoDefesa;  /* [0, 1]: vinheta azul (golpe bloqueado) */
    float queda;          /* [0, 1]: Odisseu tombando (derrota) */
} PoseJogador;

void iniciarAnimacaoCombate(AnimacaoCombate *animacao);

/* Recebe um evento do combate e coloca a animacao na fila. Tem a assinatura
 * de ObservadorCombate. Se a fila estiver cheia, a acao nova e descartada:
 * so perde o efeito visual, a logica ja foi resolvida. */
void observarEventoAnimacao(const EventoCombate *evento, void *contexto);

/* Versao direta de observarEventoAnimacao(). */
void enfileirarAcaoAnimada(AnimacaoCombate *animacao, const EventoCombate *evento);

/* true enquanto houver animacao tocando ou esperando. O combate deve esperar
 * por isso para avancar o turno. */
bool animacaoCombateOcupada(const AnimacaoCombate *animacao);

/* Avanca o tempo e escreve em `cues` os marcos atravessados, na ordem, ate
 * `capacidade`. Cada marco sai uma unica vez. Se faltar espaco, para sem
 * perder nada e continua na proxima chamada. Devolve quantos escreveu. */
int atualizarAnimacaoCombate(
    AnimacaoCombate *animacao,
    float deltaTempo,
    CueAnimacao *cues,
    int capacidade
);

/* Vida que a barra deve mostrar: ate o golpe acertar, a vida de antes dele;
 * depois, a vida real. */
int vidaExibida(const AnimacaoCombate *animacao, const Personagem *personagem);

PoseInimigo poseDoInimigo(const AnimacaoCombate *animacao, const Personagem *inimigo);
PoseJogador poseDoJogador(const AnimacaoCombate *animacao, const Personagem *jogador);

/* Curva da lanca de Odisseu em funcao do tempo da acao: 0 em repouso, -1 no
 * ponto mais recuado (antes do golpe), +1 no impacto, de volta a 0 no fim. */
float curvaEstocada(float tempo);

#endif

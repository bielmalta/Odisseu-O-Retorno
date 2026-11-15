#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

#include "acoes_combate.h"
#include "animacao_combate.h"
#include "combate.h"
#include "ia_inimigo.h"
#include "interpolacao.h"
#include "personagem.h"

enum { MAX_CUES = 8 };

#define PI_F 3.14159265358979323846f

static bool quase(float a, float b)
{
    const float diferenca = a - b;

    return (diferenca < 0.0f ? -diferenca : diferenca) < 0.0005f;
}

static EventoCombate eventoAtaque(
    const Personagem *origem, const Personagem *alvo,
    int dano, bool defendia, bool derrotado
)
{
    const EventoCombate evento = {
        EVENTO_COMBATE_ATAQUE, origem, alvo, dano, defendia, derrotado
    };

    return evento;
}

static EventoCombate eventoDefesa(const Personagem *personagem)
{
    const EventoCombate evento = {
        EVENTO_COMBATE_DEFESA, personagem, personagem, 0, true, false
    };

    return evento;
}

/* ---- Interpolacao ---- */

static void testarInterpolacao(void)
{
    assert(quase(limitar01(-3.0f), 0.0f));
    assert(quase(limitar01(0.4f), 0.4f));
    assert(quase(limitar01(7.0f), 1.0f));

    assert(quase(suavizar(0.0f), 0.0f));
    assert(quase(suavizar(1.0f), 1.0f));
    assert(quase(suavizar(0.5f), 0.5f));
    assert(quase(suavizar(-1.0f), 0.0f));
    assert(quase(suavizar(2.0f), 1.0f));

    /* Sobe sempre, e devagar nas pontas. */
    for (int i = 0; i < 100; i++) {
        assert(suavizar((float)i / 100.0f) <= suavizar((float)(i + 1) / 100.0f));
    }
    assert(suavizar(0.1f) < 0.1f);
    assert(suavizar(0.9f) > 0.9f);

    assert(quase(interpolar(10.0f, 20.0f, 0.25f), 12.5f));

    /* Angulos: sempre pelo caminho mais curto. */
    assert(quase(diferencaAngulo(0.0f, 1.0f), 1.0f));
    assert(quase(diferencaAngulo(3.0f, -3.0f), 2.0f * PI_F - 6.0f));
    assert(quase(diferencaAngulo(-3.0f, 3.0f), -(2.0f * PI_F - 6.0f)));
    assert(quase(diferencaAngulo(0.5f, 0.5f + 2.0f * PI_F), 0.0f));
    assert(quase(interpolarAngulo(0.0f, 1.0f, 0.5f), 0.5f));
    assert(quase(interpolarAngulo(0.0f, 1.0f, 2.0f), 1.0f));

    /* De 3.0 a -3.0 atravessa pi em vez de dar a volta pelo zero. */
    assert(interpolarAngulo(3.0f, -3.0f, 0.5f) > 3.0f);
}

/* ---- Linha do tempo ---- */

static void testarCuesDeUmAtaque(void)
{
    AnimacaoCombate anim;
    CueAnimacao cues[MAX_CUES];
    Personagem odisseu = criarPersonagem("Odisseu", TIPO_HEROI, 100, 20, 10, 12);
    Personagem polifemo = criarPersonagem("Polifemo", TIPO_INIMIGO, 120, 25, 8, 8);
    EventoCombate evento;

    iniciarAnimacaoCombate(&anim);
    assert(!animacaoCombateOcupada(&anim));

    /* A logica ja tirou 12 de vida; a tela ainda mostra a de antes. */
    polifemo.vida = 108;
    evento = eventoAtaque(&odisseu, &polifemo, 12, false, false);
    enfileirarAcaoAnimada(&anim, &evento);

    assert(animacaoCombateOcupada(&anim));
    assert(vidaExibida(&anim, &polifemo) == 120);

    /* Comeco: um marco, uma unica vez. */
    assert(atualizarAnimacaoCombate(&anim, 0.0f, cues, MAX_CUES) == 1);
    assert(cues[0].tipo == CUE_INICIO_GOLPE);
    assert(cues[0].evento.origem == &odisseu);

    assert(atualizarAnimacaoCombate(&anim, 0.20f, cues, MAX_CUES) == 0);
    assert(vidaExibida(&anim, &polifemo) == 120);

    /* Impacto: a barra cai agora. */
    assert(atualizarAnimacaoCombate(&anim, 0.20f, cues, MAX_CUES) == 1);
    assert(cues[0].tipo == CUE_IMPACTO);
    assert(cues[0].evento.dano == 12);
    assert(vidaExibida(&anim, &polifemo) == 108);

    /* Retorno: acaba e libera a fila, sem repetir marcos. */
    assert(animacaoCombateOcupada(&anim));
    assert(atualizarAnimacaoCombate(&anim, 0.60f, cues, MAX_CUES) == 0);
    assert(!animacaoCombateOcupada(&anim));
    assert(atualizarAnimacaoCombate(&anim, 1.0f, cues, MAX_CUES) == 0);
    assert(vidaExibida(&anim, &polifemo) == 108);
}

static void testarCuesDeUmaDefesa(void)
{
    AnimacaoCombate anim;
    CueAnimacao cues[MAX_CUES];
    Personagem odisseu = criarPersonagem("Odisseu", TIPO_HEROI, 100, 20, 10, 12);
    EventoCombate evento = eventoDefesa(&odisseu);
    PoseJogador pose;

    iniciarAnimacaoCombate(&anim);
    enfileirarAcaoAnimada(&anim, &evento);

    assert(atualizarAnimacaoCombate(&anim, 0.0f, cues, MAX_CUES) == 1);
    assert(cues[0].tipo == CUE_DEFESA_ATIVADA);

    /* O escudo sobe durante a defesa. */
    atualizarAnimacaoCombate(&anim, 0.15f, cues, MAX_CUES);
    pose = poseDoJogador(&anim, &odisseu);
    assert(quase(pose.escudo, 1.0f));
    assert(quase(pose.estocada, 0.0f));

    assert(atualizarAnimacaoCombate(&anim, ANIM_DURACAO_DEFESA, cues, MAX_CUES) == 0);
    assert(!animacaoCombateOcupada(&anim));

    /* Sem guarda, o escudo repousa; com guarda ativa, fica no meio. */
    assert(quase(poseDoJogador(&anim, &odisseu).escudo, 0.0f));
    odisseu.bonusDefesa = BONUS_DEFESA_ACAO;
    assert(quase(poseDoJogador(&anim, &odisseu).escudo, ANIM_ESCUDO_GUARDA));
}

static void testarCurvaDaEstocada(void)
{
    assert(quase(curvaEstocada(0.0f), 0.0f));
    assert(quase(curvaEstocada(ANIM_INICIO_ESTOCADA), -1.0f));
    assert(quase(curvaEstocada(ANIM_TEMPO_IMPACTO), 1.0f));
    assert(quase(curvaEstocada(ANIM_FIM_RETORNO), 0.0f));
    assert(quase(curvaEstocada(ANIM_DURACAO_ATAQUE), 0.0f));

    /* Recua antes de golpear, e o pico e exatamente no impacto. */
    assert(curvaEstocada(0.10f) < 0.0f);
    assert(curvaEstocada(0.30f) > -1.0f && curvaEstocada(0.30f) < 1.0f);
    for (int i = 0; i <= 100; i++) {
        const float t = (float)i / 100.0f;

        assert(curvaEstocada(t) >= -1.0f && curvaEstocada(t) <= 1.0f);
        assert(curvaEstocada(t) <= curvaEstocada(ANIM_TEMPO_IMPACTO) + 0.0001f);
    }
}

static void testarInimigoDerrotadoCai(void)
{
    AnimacaoCombate anim;
    CueAnimacao cues[MAX_CUES];
    Personagem odisseu = criarPersonagem("Odisseu", TIPO_HEROI, 100, 20, 10, 12);
    Personagem ciclope = criarPersonagem("Ciclope", TIPO_INIMIGO, 30, 25, 8, 8);
    EventoCombate evento;
    PoseInimigo pose;

    iniciarAnimacaoCombate(&anim);

    /* A logica derrotou o Ciclope; a tela ainda o mostra em pe. */
    ciclope.vida = 0;
    evento = eventoAtaque(&odisseu, &ciclope, 6, false, true);
    enfileirarAcaoAnimada(&anim, &evento);
    assert(vidaExibida(&anim, &ciclope) == 6);

    atualizarAnimacaoCombate(&anim, 0.20f, cues, MAX_CUES);
    assert(quase(poseDoInimigo(&anim, &ciclope).queda, 0.0f));

    /* Impacto e, pouco depois, o som da queda: marcos distintos. */
    assert(atualizarAnimacaoCombate(&anim, 0.20f, cues, MAX_CUES) == 1);
    assert(cues[0].tipo == CUE_IMPACTO);
    assert(atualizarAnimacaoCombate(&anim, 0.10f, cues, MAX_CUES) == 1);
    assert(cues[0].tipo == CUE_QUEDA);

    /* Caindo: no meio do caminho. */
    atualizarAnimacaoCombate(&anim, 0.40f, cues, MAX_CUES);
    pose = poseDoInimigo(&anim, &ciclope);
    assert(pose.queda > 0.0f && pose.queda < 1.0f);

    /* Termina caido e continua caido, mesmo sem animacao. */
    atualizarAnimacaoCombate(&anim, 5.0f, cues, MAX_CUES);
    assert(!animacaoCombateOcupada(&anim));
    assert(quase(poseDoInimigo(&anim, &ciclope).queda, 1.0f));
    assert(vidaExibida(&anim, &ciclope) == 0);

    /* Um inimigo vivo nunca aparece caido. */
    assert(quase(poseDoInimigo(&anim, &odisseu).queda, 0.0f));
}

static void testarPoseDoInimigoQueAtaca(void)
{
    AnimacaoCombate anim;
    CueAnimacao cues[MAX_CUES];
    Personagem odisseu = criarPersonagem("Odisseu", TIPO_HEROI, 100, 20, 10, 12);
    Personagem polifemo = criarPersonagem("Polifemo", TIPO_INIMIGO, 120, 25, 8, 8);
    EventoCombate evento;
    PoseInimigo pose;

    iniciarAnimacaoCombate(&anim);
    odisseu.vida = 85;
    evento = eventoAtaque(&polifemo, &odisseu, 15, false, false);
    enfileirarAcaoAnimada(&anim, &evento);

    /* Recua, investe ate o impacto e volta. */
    atualizarAnimacaoCombate(&anim, 0.20f, cues, MAX_CUES);
    pose = poseDoInimigo(&anim, &polifemo);
    assert(pose.preparo > 0.5f && pose.avanco < 0.01f);

    atualizarAnimacaoCombate(&anim, 0.15f, cues, MAX_CUES); /* t = 0.35 */
    pose = poseDoInimigo(&anim, &polifemo);
    assert(quase(pose.avanco, 1.0f) && quase(pose.preparo, 0.0f));

    atualizarAnimacaoCombate(&anim, 0.50f, cues, MAX_CUES); /* t = 0.85 */
    assert(!animacaoCombateOcupada(&anim));
    pose = poseDoInimigo(&anim, &polifemo);
    assert(quase(pose.avanco, 0.0f) && quase(pose.recuo, 0.0f));
}

static void testarInimigoAtingidoRecua(void)
{
    AnimacaoCombate anim;
    CueAnimacao cues[MAX_CUES];
    Personagem odisseu = criarPersonagem("Odisseu", TIPO_HEROI, 100, 20, 10, 12);
    Personagem polifemo = criarPersonagem("Polifemo", TIPO_INIMIGO, 120, 25, 8, 8);
    EventoCombate evento;
    PoseInimigo pose;

    iniciarAnimacaoCombate(&anim);
    polifemo.vida = 108;
    evento = eventoAtaque(&odisseu, &polifemo, 12, false, false);
    enfileirarAcaoAnimada(&anim, &evento);

    atualizarAnimacaoCombate(&anim, 0.30f, cues, MAX_CUES);
    pose = poseDoInimigo(&anim, &polifemo);
    assert(quase(pose.recuo, 0.0f) && quase(pose.brilho, 0.0f));

    atualizarAnimacaoCombate(&anim, 0.06f, cues, MAX_CUES); /* logo apos o impacto */
    pose = poseDoInimigo(&anim, &polifemo);
    assert(pose.recuo > 0.8f && pose.brilho > 0.8f);

    atualizarAnimacaoCombate(&anim, 0.40f, cues, MAX_CUES);
    pose = poseDoInimigo(&anim, &polifemo);
    assert(quase(pose.recuo, 0.0f) && quase(pose.brilho, 0.0f));
}

static void testarInimigoEmGuarda(void)
{
    AnimacaoCombate anim;
    CueAnimacao cues[MAX_CUES];
    Personagem odisseu = criarPersonagem("Odisseu", TIPO_HEROI, 100, 20, 10, 12);
    Personagem polifemo = criarPersonagem("Polifemo", TIPO_INIMIGO, 120, 25, 8, 8);
    EventoCombate evento = eventoDefesa(&polifemo);

    iniciarAnimacaoCombate(&anim);
    assert(quase(poseDoInimigo(&anim, &polifemo).agachar, 0.0f));

    /* Defende: abaixa durante a acao e continua em guarda enquanto o bonus vale. */
    polifemo.bonusDefesa = BONUS_DEFESA_ACAO;
    enfileirarAcaoAnimada(&anim, &evento);
    atualizarAnimacaoCombate(&anim, 0.30f, cues, MAX_CUES);
    assert(quase(poseDoInimigo(&anim, &polifemo).agachar, 1.0f));
    atualizarAnimacaoCombate(&anim, 1.00f, cues, MAX_CUES);
    assert(quase(poseDoInimigo(&anim, &polifemo).agachar, 0.6f));

    /* Golpe contra a guarda: a logica gasta o bonus, mas a tela mantem a
     * postura ate o golpe acertar. */
    polifemo.bonusDefesa = 0;
    polifemo.vida = 113;
    evento = eventoAtaque(&odisseu, &polifemo, 7, true, false);
    enfileirarAcaoAnimada(&anim, &evento);
    assert(quase(poseDoInimigo(&anim, &polifemo).agachar, 0.6f));
}

static void testarJogadorAtingido(void)
{
    AnimacaoCombate anim;
    CueAnimacao cues[MAX_CUES];
    Personagem odisseu = criarPersonagem("Odisseu", TIPO_HEROI, 100, 20, 10, 12);
    Personagem polifemo = criarPersonagem("Polifemo", TIPO_INIMIGO, 120, 25, 8, 8);
    EventoCombate evento;
    PoseJogador pose;

    /* Golpe sem defesa: tremor e vinheta vermelha, so depois do impacto. */
    iniciarAnimacaoCombate(&anim);
    odisseu.vida = 85;
    evento = eventoAtaque(&polifemo, &odisseu, 15, false, false);
    enfileirarAcaoAnimada(&anim, &evento);

    atualizarAnimacaoCombate(&anim, 0.30f, cues, MAX_CUES);
    pose = poseDoJogador(&anim, &odisseu);
    assert(quase(pose.tremor, 0.0f) && quase(pose.lampejoDano, 0.0f));

    atualizarAnimacaoCombate(&anim, 0.06f, cues, MAX_CUES);
    pose = poseDoJogador(&anim, &odisseu);
    assert(pose.tremor > 0.8f && pose.lampejoDano > 0.8f);
    assert(quase(pose.lampejoDefesa, 0.0f));

    atualizarAnimacaoCombate(&anim, 5.0f, cues, MAX_CUES);
    pose = poseDoJogador(&anim, &odisseu);
    assert(quase(pose.tremor, 0.0f) && quase(pose.lampejoDano, 0.0f));

    /* Golpe contra a guarda: o escudo ja estava levantado, e o lampejo e azul. */
    odisseu.vida = 90;
    odisseu.bonusDefesa = 0; /* a logica ja gastou o bonus */
    evento = eventoAtaque(&polifemo, &odisseu, 10, true, false);
    enfileirarAcaoAnimada(&anim, &evento);

    atualizarAnimacaoCombate(&anim, 0.30f, cues, MAX_CUES);
    assert(quase(poseDoJogador(&anim, &odisseu).escudo, ANIM_ESCUDO_GUARDA));

    atualizarAnimacaoCombate(&anim, 0.06f, cues, MAX_CUES);
    pose = poseDoJogador(&anim, &odisseu);
    assert(pose.lampejoDefesa > 0.8f && quase(pose.lampejoDano, 0.0f));
    assert(pose.tremor > 0.8f && pose.escudo > 0.5f);

    atualizarAnimacaoCombate(&anim, 5.0f, cues, MAX_CUES);
    assert(quase(poseDoJogador(&anim, &odisseu).escudo, 0.0f));
}

static void testarJogadorDerrotado(void)
{
    AnimacaoCombate anim;
    CueAnimacao cues[MAX_CUES];
    Personagem odisseu = criarPersonagem("Odisseu", TIPO_HEROI, 10, 20, 0, 12);
    Personagem polifemo = criarPersonagem("Polifemo", TIPO_INIMIGO, 120, 25, 8, 8);
    EventoCombate evento;

    iniciarAnimacaoCombate(&anim);
    odisseu.vida = 0;
    evento = eventoAtaque(&polifemo, &odisseu, 10, false, true);
    enfileirarAcaoAnimada(&anim, &evento);

    atualizarAnimacaoCombate(&anim, 0.20f, cues, MAX_CUES);
    assert(quase(poseDoJogador(&anim, &odisseu).queda, 0.0f));

    atualizarAnimacaoCombate(&anim, 0.80f, cues, MAX_CUES);
    assert(poseDoJogador(&anim, &odisseu).queda > 0.0f);

    atualizarAnimacaoCombate(&anim, 5.0f, cues, MAX_CUES);
    assert(!animacaoCombateOcupada(&anim));
    assert(quase(poseDoJogador(&anim, &odisseu).queda, 1.0f));
}

static void testarFilaDeAcoes(void)
{
    AnimacaoCombate anim;
    CueAnimacao cues[MAX_CUES];
    Personagem odisseu = criarPersonagem("Odisseu", TIPO_HEROI, 100, 20, 10, 12);
    Personagem polifemo = criarPersonagem("Polifemo", TIPO_INIMIGO, 120, 25, 8, 8);
    EventoCombate ataque = eventoAtaque(&odisseu, &polifemo, 12, false, false);
    EventoCombate defesa = eventoDefesa(&polifemo);
    const TipoCue esperados[] = {CUE_INICIO_GOLPE, CUE_IMPACTO, CUE_DEFESA_ATIVADA};
    int recebidos = 0;

    /* Um tempo enorme de uma vez: todos os marcos, na ordem. */
    iniciarAnimacaoCombate(&anim);
    enfileirarAcaoAnimada(&anim, &ataque);
    enfileirarAcaoAnimada(&anim, &defesa);
    assert(atualizarAnimacaoCombate(&anim, 10.0f, cues, MAX_CUES) == 3);
    for (int i = 0; i < 3; i++) {
        assert(cues[i].tipo == esperados[i]);
    }
    assert(!animacaoCombateOcupada(&anim));

    /* Com espaco para um marco por vez, nada se perde nem se repete. */
    enfileirarAcaoAnimada(&anim, &ataque);
    enfileirarAcaoAnimada(&anim, &defesa);
    for (int chamada = 0; chamada < 20 && animacaoCombateOcupada(&anim); chamada++) {
        const int n = atualizarAnimacaoCombate(&anim, 10.0f, cues, 1);

        assert(n <= 1);
        if (n == 1) {
            assert(recebidos < 3 && cues[0].tipo == esperados[recebidos]);
            recebidos++;
        }
    }
    assert(recebidos == 3);
    assert(!animacaoCombateOcupada(&anim));

    /* Fila cheia: o excesso e descartado, sem estourar. */
    for (int i = 0; i < ANIM_FILA_MAX + 3; i++) {
        enfileirarAcaoAnimada(&anim, &ataque);
    }
    assert(anim.quantidade == ANIM_FILA_MAX);
    for (int chamada = 0; chamada < 50 && animacaoCombateOcupada(&anim); chamada++) {
        atualizarAnimacaoCombate(&anim, 1.0f, cues, MAX_CUES);
    }
    assert(!animacaoCombateOcupada(&anim));
}

static void testarArgumentosInvalidos(void)
{
    AnimacaoCombate anim;
    CueAnimacao cues[MAX_CUES];
    Personagem odisseu = criarPersonagem("Odisseu", TIPO_HEROI, 100, 20, 10, 12);
    EventoCombate semAlvo = { EVENTO_COMBATE_ATAQUE, &odisseu, NULL, 0, false, false };

    iniciarAnimacaoCombate(NULL);
    iniciarAnimacaoCombate(&anim);
    enfileirarAcaoAnimada(NULL, &semAlvo);
    enfileirarAcaoAnimada(&anim, NULL);
    enfileirarAcaoAnimada(&anim, &semAlvo);
    observarEventoAnimacao(NULL, &anim);
    assert(!animacaoCombateOcupada(&anim));
    assert(!animacaoCombateOcupada(NULL));

    assert(atualizarAnimacaoCombate(NULL, 1.0f, cues, MAX_CUES) == 0);
    assert(atualizarAnimacaoCombate(&anim, 1.0f, NULL, MAX_CUES) == 0);
    assert(atualizarAnimacaoCombate(&anim, 1.0f, cues, 0) == 0);
    assert(atualizarAnimacaoCombate(&anim, -1.0f, cues, MAX_CUES) == 0);

    assert(vidaExibida(NULL, &odisseu) == odisseu.vida);
    assert(vidaExibida(&anim, NULL) == 0);
    assert(quase(poseDoInimigo(NULL, &odisseu).queda, 0.0f));
    assert(quase(poseDoJogador(&anim, NULL).queda, 0.0f));
}

/* ---- Combate inteiro com animacao: os turnos nao travam ---- */

typedef struct {
    AnimacaoCombate animacao;
    int ataques;
    int defesas;
    int inicios;
    int impactos;
    int defesasAtivadas;
    int quedas;
    int maiorFila;
} Contagem;

static Contagem contagem;

static void contarEEnfileirar(const EventoCombate *evento, void *contexto)
{
    (void)contexto;
    if (evento->tipo == EVENTO_COMBATE_ATAQUE) {
        contagem.ataques++;
    } else {
        contagem.defesas++;
    }
    enfileirarAcaoAnimada(&contagem.animacao, evento);
}

/* Mesmo gancho do jogo: o combate so avanca com a animacao parada. */
static int jogarLutaComAnimacao(Combate *combate, Personagem *alvo)
{
    EsperaTurnoInimigo espera;
    const float dt = 1.0f / 60.0f;
    int quadros = 0;

    iniciarEsperaTurnoInimigo(&espera);
    registrarObservadorCombate(contarEEnfileirar, NULL);

    while (quadros < 60 * 120) { /* 2 minutos de jogo no maximo */
        CueAnimacao cues[MAX_CUES];
        const int n = atualizarAnimacaoCombate(&contagem.animacao, dt, cues, MAX_CUES);

        for (int i = 0; i < n; i++) {
            switch (cues[i].tipo) {
            case CUE_INICIO_GOLPE: contagem.inicios++; break;
            case CUE_IMPACTO: contagem.impactos++; break;
            case CUE_DEFESA_ATIVADA: contagem.defesasAtivadas++; break;
            case CUE_QUEDA: contagem.quedas++; break;
            }
        }

        if (contagem.animacao.quantidade > contagem.maiorFila) {
            contagem.maiorFila = contagem.animacao.quantidade;
        }

        if (!animacaoCombateOcupada(&contagem.animacao)) {
            if (combate->estado == COMBATE_VICTORY ||
                combate->estado == COMBATE_DEFEAT) {
                break;
            }
            if (aguardarTurnoInimigo(&espera, combate, dt)) {
                atualizarCombate(combate);
            }
            if (combate->estado == COMBATE_WAITING_PLAYER) {
                const bool agiu = atacarNoCombate(combate, alvo);

                assert(agiu);
                (void)agiu;
            }
        }

        quadros++;
    }

    registrarObservadorCombate(NULL, NULL);
    return quadros;
}

static void testarTurnosNaoTravam(void)
{
    Combate combate;
    Personagem odisseu = criarPersonagem("Odisseu", TIPO_HEROI, 100, 20, 10, 12);
    Personagem ciclope = criarPersonagem("Ciclope", TIPO_INIMIGO, 30, 25, 8, 8);
    Personagem *inimigos[] = {&ciclope};
    int quadros;

    contagem = (Contagem){0};
    iniciarAnimacaoCombate(&contagem.animacao);
    inicializarCombate(&combate);
    assert(prepararCombate(&combate, &odisseu, inimigos, 1));

    quadros = jogarLutaComAnimacao(&combate, &ciclope);

    /* Termina, em tempo razoavel, com Odisseu vencendo. */
    assert(quadros < 60 * 30);
    assert(combate.estado == COMBATE_VICTORY);
    assert(!animacaoCombateOcupada(&contagem.animacao));

    /* Cada acao teve comeco e, se foi golpe, impacto; nenhuma sobrou. */
    assert(contagem.ataques == 4 && contagem.defesas == 1);
    assert(contagem.inicios == contagem.ataques);
    assert(contagem.impactos == contagem.ataques);
    assert(contagem.defesasAtivadas == contagem.defesas);
    assert(contagem.quedas == 1);

    /* O combate esperou: nunca houve duas animacoes na fila. */
    assert(contagem.maiorFila == 1);

    /* No fim a tela mostra a vida real e o inimigo caido. */
    assert(vidaExibida(&contagem.animacao, &ciclope) == 0);
    assert(quase(poseDoInimigo(&contagem.animacao, &ciclope).queda, 1.0f));

    finalizarCombate(&combate);
}

static void testarDerrotaNaoTrava(void)
{
    Combate combate;
    Personagem odisseu = criarPersonagem("Odisseu", TIPO_HEROI, 40, 10, 0, 5);
    Personagem ciclope = criarPersonagem("Ciclope", TIPO_INIMIGO, 200, 30, 5, 9);
    Personagem *inimigos[] = {&ciclope};
    int quadros;

    contagem = (Contagem){0};
    iniciarAnimacaoCombate(&contagem.animacao);
    inicializarCombate(&combate);
    assert(prepararCombate(&combate, &odisseu, inimigos, 1));

    quadros = jogarLutaComAnimacao(&combate, &ciclope);

    assert(quadros < 60 * 30);
    assert(combate.estado == COMBATE_DEFEAT);
    assert(!animacaoCombateOcupada(&contagem.animacao));
    assert(contagem.impactos == contagem.ataques);
    assert(contagem.quedas == 1);
    assert(quase(poseDoJogador(&contagem.animacao, &odisseu).queda, 1.0f));

    finalizarCombate(&combate);
}

int main(void)
{
    testarInterpolacao();
    testarCuesDeUmAtaque();
    testarCuesDeUmaDefesa();
    testarCurvaDaEstocada();
    testarInimigoDerrotadoCai();
    testarPoseDoInimigoQueAtaca();
    testarInimigoAtingidoRecua();
    testarInimigoEmGuarda();
    testarJogadorAtingido();
    testarJogadorDerrotado();
    testarFilaDeAcoes();
    testarArgumentosInvalidos();
    testarTurnosNaoTravam();
    testarDerrotaNaoTrava();

    printf("\nTESTE DAS ANIMACOES DE COMBATE PASSOU!\n");

    return 0;
}

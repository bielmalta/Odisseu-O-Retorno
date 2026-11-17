#include "animacao_combate.h"

#include <stddef.h>
#include <string.h>

#include "interpolacao.h"

/* ---- Duracoes ---- */

static float duracaoQueda(const Personagem *personagem)
{
    return personagem->tipo == TIPO_HEROI ? ANIM_DURACAO_QUEDA_JOGADOR
                                          : ANIM_DURACAO_QUEDA_INIMIGO;
}

static bool tipoEhAtaque(TipoEventoCombate tipo)
{
    return tipo == EVENTO_COMBATE_ATAQUE ||
           tipo == EVENTO_COMBATE_ATAQUE_PESADO;
}

static float duracaoDaAcao(const EventoCombate *evento)
{
    if (!tipoEhAtaque(evento->tipo)) {
        return ANIM_DURACAO_DEFESA;
    }

    if (evento->alvoDerrotado) {
        return ANIM_TEMPO_IMPACTO + duracaoQueda(evento->alvo);
    }

    return ANIM_DURACAO_ATAQUE;
}

static bool ehAtaque(const AcaoAnimada *acao)
{
    return tipoEhAtaque(acao->evento.tipo);
}

/* ---- Fila ---- */

void iniciarAnimacaoCombate(AnimacaoCombate *animacao)
{
    if (animacao != NULL) {
        memset(animacao, 0, sizeof *animacao);
    }
}

void enfileirarAcaoAnimada(AnimacaoCombate *animacao, const EventoCombate *evento)
{
    AcaoAnimada *acao;

    if (animacao == NULL || evento == NULL || evento->origem == NULL ||
        evento->alvo == NULL || animacao->quantidade >= ANIM_FILA_MAX) {
        return;
    }

    acao = &animacao->fila[animacao->quantidade++];
    memset(acao, 0, sizeof *acao);
    acao->evento = *evento;
    /* O evento sai depois do dano: a vida de antes e a de agora mais o dano. */
    acao->vidaAntes = evento->alvo->vida + evento->dano;
    acao->duracao = duracaoDaAcao(evento);
}

void observarEventoAnimacao(const EventoCombate *evento, void *contexto)
{
    enfileirarAcaoAnimada(contexto, evento);
}

bool animacaoCombateOcupada(const AnimacaoCombate *animacao)
{
    return animacao != NULL && animacao->quantidade > 0;
}

static void descartarPrimeira(AnimacaoCombate *animacao)
{
    memmove(
        &animacao->fila[0], &animacao->fila[1],
        (size_t)(animacao->quantidade - 1) * sizeof animacao->fila[0]
    );
    animacao->quantidade--;
}

/* Escreve o marco se houver espaco; devolve false se a lista de marcos
 * estiver cheia. */
static bool emitirCue(
    TipoCue tipo, const AcaoAnimada *acao,
    CueAnimacao *cues, int capacidade, int *escritos
)
{
    if (*escritos >= capacidade) {
        return false;
    }

    cues[*escritos].tipo = tipo;
    cues[*escritos].evento = acao->evento;
    (*escritos)++;
    return true;
}

int atualizarAnimacaoCombate(
    AnimacaoCombate *animacao,
    float deltaTempo,
    CueAnimacao *cues,
    int capacidade
)
{
    int escritos = 0;
    float restante = deltaTempo > 0.0f ? deltaTempo : 0.0f;

    if (animacao == NULL || cues == NULL || capacidade <= 0) {
        return 0;
    }

    while (animacao->quantidade > 0) {
        AcaoAnimada *acao = &animacao->fila[0];
        const float tempoSom = ANIM_TEMPO_IMPACTO + ANIM_ATRASO_SOM_QUEDA;
        float novoTempo = acao->tempo + restante;

        if (novoTempo > acao->duracao) {
            novoTempo = acao->duracao;
        }

        /* Comeco: sai na primeira vez que a acao e atendida. */
        if (!acao->iniciou) {
            const TipoCue inicio = ehAtaque(acao) ? CUE_INICIO_GOLPE
                                                  : CUE_DEFESA_ATIVADA;

            if (!emitirCue(inicio, acao, cues, capacidade, &escritos)) {
                return escritos;
            }
            acao->iniciou = true;
        }

        /* Impacto. */
        if (ehAtaque(acao) && !acao->impactou && novoTempo >= ANIM_TEMPO_IMPACTO) {
            if (!emitirCue(CUE_IMPACTO, acao, cues, capacidade, &escritos)) {
                return escritos;
            }
            acao->impactou = true;
        }

        /* Queda de quem foi derrotado. */
        if (ehAtaque(acao) && acao->evento.alvoDerrotado && !acao->caiu &&
            novoTempo >= tempoSom) {
            if (!emitirCue(CUE_QUEDA, acao, cues, capacidade, &escritos)) {
                return escritos;
            }
            acao->caiu = true;
        }

        restante -= novoTempo - acao->tempo;
        acao->tempo = novoTempo;

        if (acao->tempo < acao->duracao) {
            break; /* todo o tempo foi usado nesta acao */
        }

        /* Retorno concluido: passa para a proxima acao com o tempo que sobrou. */
        descartarPrimeira(animacao);
    }

    return escritos;
}

/* ---- Estado visual ---- */

int vidaExibida(const AnimacaoCombate *animacao, const Personagem *personagem)
{
    if (personagem == NULL) {
        return 0;
    }

    if (animacao != NULL) {
        for (int i = 0; i < animacao->quantidade; i++) {
            const AcaoAnimada *acao = &animacao->fila[i];

            if (ehAtaque(acao) && acao->evento.alvo == personagem &&
                !acao->impactou) {
                return acao->vidaAntes;
            }
        }
    }

    return personagem->vida;
}

/* Quanto do tombo de `personagem` ja aconteceu: 0 em pe, 1 tombado. Quem
 * foi derrotado pela logica continua em pe ate o golpe acertar. */
static float progressoQueda(const AnimacaoCombate *animacao, const Personagem *personagem)
{
    if (personagemVivo(personagem)) {
        return 0.0f;
    }

    for (int i = 0; i < animacao->quantidade; i++) {
        const AcaoAnimada *acao = &animacao->fila[i];

        if (ehAtaque(acao) && acao->evento.alvo == personagem &&
            acao->evento.alvoDerrotado) {
            if (i == 0 && acao->tempo >= ANIM_TEMPO_IMPACTO) {
                return suavizar(
                    (acao->tempo - ANIM_TEMPO_IMPACTO) / duracaoQueda(personagem)
                );
            }
            return 0.0f;
        }
    }

    return 1.0f;
}

/* Em guarda: com o bonus de defesa ativo ou com um golpe bloqueado ainda a
 * caminho (a logica ja gastou o bonus, mas o golpe ainda nao chegou). */
static bool emGuarda(const AnimacaoCombate *animacao, const Personagem *personagem)
{
    if (personagem->bonusDefesa > 0) {
        return true;
    }

    for (int i = 0; i < animacao->quantidade; i++) {
        const AcaoAnimada *acao = &animacao->fila[i];

        if (ehAtaque(acao) && acao->evento.alvo == personagem &&
            acao->evento.alvoDefendia && !acao->impactou) {
            return true;
        }
    }

    return false;
}

/* 0 -> 1 -> 1 -> 0 ao longo de uma defesa. */
static float curvaDefesa(float tempo)
{
    const float subida = 0.15f;
    const float descida = ANIM_DURACAO_DEFESA - 0.15f;

    if (tempo < subida) {
        return suavizar(tempo / subida);
    }
    if (tempo < descida) {
        return 1.0f;
    }

    return 1.0f - suavizar((tempo - descida) / (ANIM_DURACAO_DEFESA - descida));
}

float curvaEstocada(float tempo)
{
    if (tempo < ANIM_INICIO_ESTOCADA) {
        return -suavizar(tempo / ANIM_INICIO_ESTOCADA);
    }
    if (tempo < ANIM_TEMPO_IMPACTO) {
        return -1.0f + 2.0f * suavizar(
            (tempo - ANIM_INICIO_ESTOCADA) /
            (ANIM_TEMPO_IMPACTO - ANIM_INICIO_ESTOCADA)
        );
    }
    if (tempo < ANIM_FIM_RETORNO) {
        return 1.0f - suavizar(
            (tempo - ANIM_TEMPO_IMPACTO) / (ANIM_FIM_RETORNO - ANIM_TEMPO_IMPACTO)
        );
    }

    return 0.0f;
}

PoseInimigo poseDoInimigo(const AnimacaoCombate *animacao, const Personagem *inimigo)
{
    PoseInimigo pose = {0};
    const AcaoAnimada *acao;

    if (animacao == NULL || inimigo == NULL) {
        return pose;
    }

    pose.queda = progressoQueda(animacao, inimigo);

    if (emGuarda(animacao, inimigo)) {
        pose.agachar = 0.6f;
    }

    if (animacao->quantidade == 0) {
        return pose;
    }

    acao = &animacao->fila[0];

    if (acao->evento.origem == inimigo && ehAtaque(acao)) {
        /* Recua, investe ate o impacto e volta. */
        const float t = acao->tempo;

        if (t < ANIM_INICIO_ESTOCADA) {
            pose.preparo = suavizar(t / ANIM_INICIO_ESTOCADA);
        } else if (t < ANIM_TEMPO_IMPACTO) {
            const float p = suavizar(
                (t - ANIM_INICIO_ESTOCADA) /
                (ANIM_TEMPO_IMPACTO - ANIM_INICIO_ESTOCADA)
            );

            pose.preparo = 1.0f - p;
            pose.avanco = p;
        } else if (t < ANIM_FIM_RETORNO) {
            pose.avanco = 1.0f - suavizar(
                (t - ANIM_TEMPO_IMPACTO) / (ANIM_FIM_RETORNO - ANIM_TEMPO_IMPACTO)
            );
        }
    } else if (acao->evento.origem == inimigo &&
               !ehAtaque(acao)) {
        const float agachar = 0.6f + 0.4f * curvaDefesa(acao->tempo);

        pose.agachar = agachar > pose.agachar ? agachar : pose.agachar;
    }

    if (acao->evento.alvo == inimigo && ehAtaque(acao) &&
        acao->evento.dano > 0 && acao->tempo >= ANIM_TEMPO_IMPACTO) {
        const float depois = acao->tempo - ANIM_TEMPO_IMPACTO;

        pose.recuo = 1.0f - suavizar(depois / 0.30f);
        pose.brilho = 1.0f - suavizar(depois / 0.25f);
    }

    return pose;
}

PoseJogador poseDoJogador(const AnimacaoCombate *animacao, const Personagem *jogador)
{
    PoseJogador pose = {0};
    const AcaoAnimada *acao;

    if (animacao == NULL || jogador == NULL) {
        return pose;
    }

    pose.queda = progressoQueda(animacao, jogador);

    if (emGuarda(animacao, jogador)) {
        pose.escudo = ANIM_ESCUDO_GUARDA;
    }

    if (animacao->quantidade == 0) {
        return pose;
    }

    acao = &animacao->fila[0];

    if (acao->evento.origem == jogador) {
        if (ehAtaque(acao)) {
            pose.estocada = curvaEstocada(acao->tempo);
        } else {
            const float escudo = curvaDefesa(acao->tempo);

            pose.escudo = escudo > pose.escudo ? escudo : pose.escudo;
        }
    }

    if (acao->evento.alvo == jogador && ehAtaque(acao) &&
        acao->evento.dano > 0 && acao->tempo >= ANIM_TEMPO_IMPACTO) {
        const float depois = acao->tempo - ANIM_TEMPO_IMPACTO;

        pose.tremor = 1.0f - suavizar(depois / 0.35f);

        if (acao->evento.alvoDefendia) {
            /* O escudo recebe o golpe e depois baixa. */
            const float guarda = ANIM_ESCUDO_GUARDA * (1.0f - suavizar(depois / 0.50f));

            pose.lampejoDefesa = 1.0f - suavizar(depois / 0.40f);
            pose.escudo = guarda;
        } else {
            pose.lampejoDano = 1.0f - suavizar(depois / 0.50f);
        }
    }

    return pose;
}

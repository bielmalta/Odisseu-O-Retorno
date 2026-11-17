#include "ia_inimigo.h"

#include <stddef.h>
#include <string.h>

#include "acoes_combate.h"

static bool alvoValido(const Personagem *inimigo, const Personagem *alvo)
{
    return alvo != NULL &&
           alvo != inimigo &&
           personagemVivo(alvo) &&
           alvo->tipo != inimigo->tipo;
}

Personagem *selecionarAlvoInimigo(const Combate *combate,
                                  const Personagem *inimigo)
{
    const NoTurno *no;

    if (combate == NULL || inimigo == NULL) {
        return NULL;
    }

    /* Percorre uma volta da lista real; so quem esta nela pode ser alvo. */
    no = combate->turnos.inicio;
    for (int i = 0; i < combate->turnos.tamanho; i++) {
        if (alvoValido(inimigo, no->personagem)) {
            return no->personagem;
        }
        no = no->proximo;
    }

    return NULL;
}

static bool vidaBaixa(const Personagem *personagem)
{
    /* Comparacao em inteiros: vida/vidaMaxima <= limiar/100. */
    return personagem->vida * 100 <=
           personagem->vidaMaxima * IA_LIMIAR_DEFESA_PORCENTO;
}

AcaoInimigo escolherAcaoInimigo(const Personagem *inimigo,
                                const Personagem *alvo)
{
    if (inimigo == NULL || alvo == NULL) {
        return ACAO_INIMIGO_DEFENDER;
    }

    if (calcularDano(inimigo->ataque, alvo->defesa, alvo->bonusDefesa) >=
        alvo->vida) {
        return ACAO_INIMIGO_ATACAR;
    }

    if (vidaBaixa(inimigo) && inimigo->bonusDefesa == 0) {
        return ACAO_INIMIGO_DEFENDER;
    }

    return ACAO_INIMIGO_ATACAR;
}

static Personagem *aliadoFerido(const Combate *combate, const Personagem *protetor)
{
    const NoTurno *no = combate->turnos.inicio;
    Personagem *escolhido = NULL;

    for (int i = 0; i < combate->turnos.tamanho; i++) {
        Personagem *candidato = no->personagem;

        if (candidato != protetor && candidato->tipo == protetor->tipo &&
            personagemVivo(candidato) && candidato->bonusDefesa == 0 &&
            candidato->vida * 100 <= candidato->vidaMaxima * 60 &&
            (escolhido == NULL ||
             candidato->vida * escolhido->vidaMaxima <
             escolhido->vida * candidato->vidaMaxima)) {
            escolhido = candidato;
        }
        no = no->proximo;
    }

    return escolhido;
}

static AcaoInimigo escolherAcaoEspecial(
    Combate *combate,
    Personagem *inimigo,
    Personagem **alvoEspecial
)
{
    *alvoEspecial = NULL;

    if (combate->identidade == IDENTIDADE_POLIFEMO) {
        if (combate->golpePesadoPreparado) {
            return ACAO_INIMIGO_GOLPE_PESADO;
        }
        if (combate->rodada >= 2 && combate->rodada % 3 == 2) {
            return ACAO_INIMIGO_PREPARAR_PESADO;
        }
    }

    if (combate->identidade == IDENTIDADE_SEREIAS &&
        combate->rodada >= 2 && combate->rodada % 2 == 0 &&
        combate->rodadaUltimoEncanto != combate->rodada &&
        strstr(inimigo->nome, "Ligeia") != NULL) {
        return ACAO_INIMIGO_ENFEITICAR;
    }

    if (combate->identidade == IDENTIDADE_ITACA) {
        *alvoEspecial = aliadoFerido(combate, inimigo);
        if (*alvoEspecial != NULL) {
            return ACAO_INIMIGO_PROTEGER_ALIADO;
        }
    }

    return ACAO_INIMIGO_ATACAR;
}

bool executarTurnoInimigo(Combate *combate)
{
    Personagem *inimigo;
    Personagem *alvo;
    Personagem *alvoEspecial;
    AcaoInimigo acao;
    bool agiu = false;

    if (combate == NULL || combate->estado != COMBATE_ENEMY_TURN ||
        combate->participanteAtual == NULL ||
        combate->participanteAtual->tipo != TIPO_INIMIGO) {
        return false;
    }

    inimigo = combate->participanteAtual;
    alvo = selecionarAlvoInimigo(combate, inimigo);

    if (alvo != NULL) {
        acao = escolherAcaoEspecial(combate, inimigo, &alvoEspecial);

        if (acao == ACAO_INIMIGO_PREPARAR_PESADO) {
            agiu = prepararGolpePesadoNoCombate(combate);
        } else if (acao == ACAO_INIMIGO_GOLPE_PESADO) {
            agiu = golpePesadoNoCombate(combate, alvo);
        } else if (acao == ACAO_INIMIGO_ENFEITICAR) {
            agiu = enfeiticarNoCombate(combate, alvo);
        } else if (acao == ACAO_INIMIGO_PROTEGER_ALIADO) {
            agiu = protegerAliadoNoCombate(combate, alvoEspecial);
        } else if (escolherAcaoInimigo(inimigo, alvo) == ACAO_INIMIGO_DEFENDER) {
            agiu = defenderNoCombate(combate);
        } else {
            agiu = atacarNoCombate(combate, alvo);
        }
    }

    if (!agiu) {
        concluirAcaoCombate(combate);
    }

    return true;
}

const char *intencaoEspecialInimigo(const Combate *combate)
{
    if (combate == NULL) {
        return NULL;
    }

    if (combate->identidade == IDENTIDADE_POLIFEMO &&
        combate->golpePesadoPreparado) {
        return "ALERTA: golpe pesado de Polifemo! Use Defender agora.";
    }

    if (combate->identidade == IDENTIDADE_SEREIAS &&
        combate->penalidadeIniciativaPendente < 0) {
        return "O canto reduzira sua iniciativa na proxima rodada.";
    }

    return NULL;
}

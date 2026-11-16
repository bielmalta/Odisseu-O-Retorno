#include "ia_inimigo.h"

#include <stddef.h>

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

bool executarTurnoInimigo(Combate *combate)
{
    Personagem *inimigo;
    Personagem *alvo;
    bool agiu = false;

    if (combate == NULL || combate->estado != COMBATE_ENEMY_TURN ||
        combate->participanteAtual == NULL ||
        combate->participanteAtual->tipo != TIPO_INIMIGO) {
        return false;
    }

    inimigo = combate->participanteAtual;
    alvo = selecionarAlvoInimigo(combate, inimigo);

    if (alvo != NULL) {
        if (escolherAcaoInimigo(inimigo, alvo) == ACAO_INIMIGO_DEFENDER) {
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

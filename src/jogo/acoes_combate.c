#include "acoes_combate.h"

#include <stdio.h>

static ObservadorCombate observadorAtual = NULL;
static void *contextoObservador = NULL;

static void emitirEvento(const EventoCombate *evento)
{
    if (observadorAtual != NULL) {
        observadorAtual(evento, contextoObservador);
    }
}

void registrarObservadorCombate(ObservadorCombate observador, void *contexto)
{
    observadorAtual = observador;
    contextoObservador = observador != NULL ? contexto : NULL;
}

int calcularDano(int ataque, int defesaAlvo, int bonusDefesa)
{
    const int dano = ataque - defesaAlvo - bonusDefesa;

    return dano < DANO_MINIMO ? DANO_MINIMO : dano;
}

static bool ataqueValido(const Personagem *atacante, const Personagem *alvo)
{
    return atacante != NULL && alvo != NULL &&
           atacante != alvo &&
           personagemVivo(atacante) && personagemVivo(alvo) &&
           atacante->tipo != alvo->tipo;
}

int realizarAtaque(Personagem *atacante, Personagem *alvo)
{
    if (!ataqueValido(atacante, alvo)) {
        return 0;
    }

    /* Agir de novo encerra a guarda que o atacante tinha levantado. */
    atacante->bonusDefesa = 0;

    const bool alvoDefendia = alvo->bonusDefesa > 0;
    const int dano = aplicarDano(
        alvo,
        calcularDano(atacante->ataque, alvo->defesa, alvo->bonusDefesa)
    );

    /* A guarda protege so contra o proximo golpe. */
    alvo->bonusDefesa = 0;

    const EventoCombate evento = {
        EVENTO_COMBATE_ATAQUE, atacante, alvo, dano,
        alvoDefendia, !personagemVivo(alvo)
    };
    emitirEvento(&evento);

    return dano;
}

bool realizarDefesa(Personagem *personagem)
{
    if (!personagemVivo(personagem)) {
        return false;
    }

    personagem->bonusDefesa = BONUS_DEFESA_ACAO;

    const EventoCombate evento = {
        EVENTO_COMBATE_DEFESA, personagem, personagem, 0, true, false
    };
    emitirEvento(&evento);

    return true;
}

static bool acaoPermitida(const Combate *combate)
{
    return combate != NULL &&
           combate->participanteAtual != NULL &&
           (combate->estado == COMBATE_WAITING_PLAYER ||
            combate->estado == COMBATE_ENEMY_TURN) &&
           personagemVivo(combate->participanteAtual);
}

static bool participaDoCombate(const Combate *combate, const Personagem *p)
{
    const NoTurno *no = combate->turnos.inicio;

    for (int i = 0; i < combate->turnos.tamanho; i++) {
        if (no->personagem == p) {
            return true;
        }
        no = no->proximo;
    }

    return false;
}

bool atacarNoCombate(Combate *combate, Personagem *alvo)
{
    if (!acaoPermitida(combate) || !participaDoCombate(combate, alvo)) {
        return false;
    }

    Personagem *atacante = combate->participanteAtual;
    const int dano = realizarAtaque(atacante, alvo);

    if (dano == 0) {
        return false;
    }

    printf("[RODADA %u] %s atacou %s: %d de dano (vida %d/%d).\n",
           combate->rodada, atacante->nome, alvo->nome,
           dano, alvo->vida, alvo->vidaMaxima);

    concluirAcaoCombate(combate);
    return true;
}

bool defenderNoCombate(Combate *combate)
{
    if (!acaoPermitida(combate) ||
        !realizarDefesa(combate->participanteAtual)) {
        return false;
    }

    printf("[RODADA %u] %s se defendeu (+%d de defesa no proximo golpe).\n",
           combate->rodada, combate->participanteAtual->nome,
           BONUS_DEFESA_ACAO);

    concluirAcaoCombate(combate);
    return true;
}

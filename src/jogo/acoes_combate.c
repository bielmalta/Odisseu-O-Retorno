#include "acoes_combate.h"

#include <stddef.h>

#define ACAO_MAX_ALVOS 8 /* mais que qualquer encontro do jogo */

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

static void ganharEnergia(Personagem *personagem, int quantidade)
{
    personagem->energia += quantidade;
    if (personagem->energia > ENERGIA_MAXIMA) {
        personagem->energia = ENERGIA_MAXIMA;
    }
}

/* Golpe com `percentual` do ataque (100 = ataque normal). */
static int golpear(Personagem *atacante, Personagem *alvo, int percentual)
{
    if (!ataqueValido(atacante, alvo)) {
        return 0;
    }

    /* Agir de novo encerra a guarda e a iniciativa extra do atacante. */
    atacante->bonusDefesa = 0;
    atacante->bonusIniciativa = 0;

    const bool alvoDefendia = alvo->bonusDefesa > 0;
    const int dano = aplicarDano(
        alvo,
        calcularDano(
            atacante->ataque * percentual / 100, alvo->defesa, alvo->bonusDefesa
        )
    );

    /* A guarda protege so contra o proximo golpe. */
    alvo->bonusDefesa = 0;
    ganharEnergia(alvo, GANHO_ENERGIA_DANO);

    const EventoCombate evento = {
        EVENTO_COMBATE_ATAQUE, atacante, alvo, dano,
        alvoDefendia, !personagemVivo(alvo)
    };
    emitirEvento(&evento);

    return dano;
}

int realizarAtaque(Personagem *atacante, Personagem *alvo)
{
    const int dano = golpear(atacante, alvo, 100);

    if (dano > 0) {
        ganharEnergia(atacante, GANHO_ENERGIA_ATAQUE);
    }
    return dano;
}

bool realizarDefesa(Personagem *personagem)
{
    if (!personagemVivo(personagem)) {
        return false;
    }

    personagem->bonusDefesa = BONUS_DEFESA_ACAO;
    if (personagem->tipo == TIPO_HEROI) {
        personagem->bonusIniciativa = BONUS_INICIATIVA_DEFESA;
    }
    ganharEnergia(personagem, GANHO_ENERGIA_DEFESA);

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

    concluirAcaoCombate(combate);
    return true;
}

bool defenderNoCombate(Combate *combate)
{
    if (!acaoPermitida(combate) ||
        !realizarDefesa(combate->participanteAtual)) {
        return false;
    }

    concluirAcaoCombate(combate);
    return true;
}

bool giroNoCombate(Combate *combate)
{
    Personagem *alvos[ACAO_MAX_ALVOS];
    Personagem *heroi;
    int quantidade;

    if (!acaoPermitida(combate) ||
        combate->participanteAtual != combate->jogador ||
        combate->jogador->energia < CUSTO_GIRO) {
        return false;
    }

    heroi = combate->jogador;
    heroi->energia -= CUSTO_GIRO;

    /* Uma volta pela lista circular de turnos. */
    quantidade = percorrerParticipantes(&combate->turnos, alvos, ACAO_MAX_ALVOS);
    for (int i = 0; i < quantidade; i++) {
        if (alvos[i]->tipo == TIPO_INIMIGO) {
            golpear(heroi, alvos[i], PERCENTUAL_GIRO);
        }
    }

    concluirAcaoCombate(combate);
    return true;
}

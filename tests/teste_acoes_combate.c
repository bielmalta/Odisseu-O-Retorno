#include <assert.h>
#include <stdio.h>

#include "acoes_combate.h"

enum { MAX_EVENTOS = 16 };

typedef struct {
    EventoCombate eventos[MAX_EVENTOS];
    int quantidade;
} RegistroEventos;

static void guardarEvento(const EventoCombate *evento, void *contexto)
{
    RegistroEventos *registro = contexto;

    if (registro->quantidade < MAX_EVENTOS) {
        registro->eventos[registro->quantidade++] = *evento;
    }
}

static Personagem criarOdisseu(void)
{
    return criarPersonagem("Odisseu", TIPO_HEROI, 100, 20, 10, 12);
}

static Personagem criarPolifemo(void)
{
    return criarPersonagem("Polifemo", TIPO_INIMIGO, 120, 25, 8, 8);
}

static void testarFormulaDoDano(void)
{
    assert(calcularDano(20, 8, 0) == 12);
    assert(calcularDano(20, 8, BONUS_DEFESA_ACAO) == 12 - BONUS_DEFESA_ACAO);

    /* Defesa igual ou maior que o ataque ainda causa o dano minimo. */
    assert(calcularDano(10, 10, 0) == DANO_MINIMO);
    assert(calcularDano(5, 10, 0) == DANO_MINIMO);
    assert(calcularDano(1, 100, 50) == DANO_MINIMO);
    assert(calcularDano(0, 0, 0) == DANO_MINIMO);
}

static void testarAtaque(void)
{
    Personagem odisseu = criarOdisseu();
    Personagem polifemo = criarPolifemo();

    assert(odisseu.bonusDefesa == 0);

    assert(realizarAtaque(&odisseu, &polifemo) == 12);
    assert(polifemo.vida == 108);
    assert(odisseu.vida == 100);

    assert(realizarAtaque(&polifemo, &odisseu) == 15);
    assert(odisseu.vida == 85);

    /* Atacante fraco ainda tira pelo menos 1. */
    Personagem fraco = criarPersonagem("Fraco", TIPO_INIMIGO, 10, 0, 0, 1);
    assert(realizarAtaque(&fraco, &odisseu) == DANO_MINIMO);
    assert(odisseu.vida == 84);
}

static void testarVidaNuncaNegativa(void)
{
    Personagem odisseu = criarOdisseu();
    Personagem ferido = criarPersonagem("Ferido", TIPO_INIMIGO, 50, 5, 0, 3);

    ferido.vida = 3;

    /* O golpe seria de 20, mas so existem 3 pontos de vida. */
    assert(realizarAtaque(&odisseu, &ferido) == 3);
    assert(ferido.vida == 0);
    assert(!personagemVivo(&ferido));

    /* Derrotados nao atacam nem apanham. */
    assert(realizarAtaque(&odisseu, &ferido) == 0);
    assert(realizarAtaque(&ferido, &odisseu) == 0);
    assert(ferido.vida == 0);
    assert(odisseu.vida == 100);
}

static void testarAtaquesInvalidos(void)
{
    Personagem odisseu = criarOdisseu();
    Personagem polifemo = criarPolifemo();
    Personagem sereia = criarPersonagem("Sereia", TIPO_INIMIGO, 60, 15, 5, 15);

    assert(realizarAtaque(NULL, &polifemo) == 0);
    assert(realizarAtaque(&odisseu, NULL) == 0);
    assert(realizarAtaque(&odisseu, &odisseu) == 0);
    assert(realizarAtaque(&polifemo, &sereia) == 0); /* mesmo lado */

    assert(odisseu.vida == 100);
    assert(polifemo.vida == 120);
    assert(sereia.vida == 60);
}

static void testarDefesa(void)
{
    Personagem odisseu = criarOdisseu();
    Personagem polifemo = criarPolifemo();

    /* Sem defesa: 25 - 10 = 15. */
    assert(realizarAtaque(&polifemo, &odisseu) == 15);

    /* Defendendo, o proximo golpe cai pelo bonus. */
    assert(realizarDefesa(&odisseu));
    assert(odisseu.bonusDefesa == BONUS_DEFESA_ACAO);
    assert(realizarAtaque(&polifemo, &odisseu) == 15 - BONUS_DEFESA_ACAO);

    /* A guarda foi gasta: o golpe seguinte volta ao normal. */
    assert(odisseu.bonusDefesa == 0);
    assert(realizarAtaque(&polifemo, &odisseu) == 15);

    /* Agir de novo desfaz a guarda que nao chegou a ser usada. */
    assert(realizarDefesa(&odisseu));
    assert(realizarAtaque(&odisseu, &polifemo) == 12);
    assert(odisseu.bonusDefesa == 0);
    assert(realizarAtaque(&polifemo, &odisseu) == 15);

    /* Defender nao empilha: repetir mantem um unico bonus. */
    assert(realizarDefesa(&odisseu));
    assert(realizarDefesa(&odisseu));
    assert(odisseu.bonusDefesa == BONUS_DEFESA_ACAO);

    /* Derrotado nao se defende. */
    polifemo.vida = 0;
    assert(!realizarDefesa(&polifemo));
    assert(polifemo.bonusDefesa == 0);
    assert(!realizarDefesa(NULL));
}

static void testarDefesaForteAindaCausaDanoMinimo(void)
{
    Personagem odisseu = criarOdisseu();
    Personagem arqueiro = criarPersonagem("Arqueiro", TIPO_INIMIGO, 30, 12, 0, 6);

    /* 12 - 10 - 5 seria negativo: vale o minimo. */
    assert(realizarDefesa(&odisseu));
    assert(realizarAtaque(&arqueiro, &odisseu) == DANO_MINIMO);
    assert(odisseu.vida == 99);
}

static void testarEventos(void)
{
    Personagem odisseu = criarOdisseu();
    Personagem polifemo = criarPolifemo();
    RegistroEventos registro = {0};

    registrarObservadorCombate(guardarEvento, &registro);

    assert(realizarDefesa(&odisseu));
    assert(realizarAtaque(&polifemo, &odisseu) == 10);

    polifemo.vida = 5;
    assert(realizarAtaque(&odisseu, &polifemo) == 5);

    assert(registro.quantidade == 3);

    assert(registro.eventos[0].tipo == EVENTO_COMBATE_DEFESA);
    assert(registro.eventos[0].origem == &odisseu);
    assert(registro.eventos[0].alvo == &odisseu);
    assert(registro.eventos[0].dano == 0);

    assert(registro.eventos[1].tipo == EVENTO_COMBATE_ATAQUE);
    assert(registro.eventos[1].origem == &polifemo);
    assert(registro.eventos[1].alvo == &odisseu);
    assert(registro.eventos[1].dano == 10);
    assert(registro.eventos[1].alvoDefendia);
    assert(!registro.eventos[1].alvoDerrotado);

    assert(registro.eventos[2].dano == 5);
    assert(!registro.eventos[2].alvoDefendia);
    assert(registro.eventos[2].alvoDerrotado);

    /* Ataque invalido nao gera evento; sem observador nada quebra. */
    assert(realizarAtaque(&odisseu, &polifemo) == 0);
    assert(registro.quantidade == 3);

    registrarObservadorCombate(NULL, NULL);
    assert(realizarDefesa(&odisseu));
    assert(registro.quantidade == 3);
}

static void testarIntegracaoComOControladorDeTurnos(void)
{
    Combate combate;
    Personagem odisseu = criarOdisseu();
    Personagem polifemo = criarPolifemo();
    Personagem intruso = criarPersonagem("Intruso", TIPO_INIMIGO, 10, 1, 1, 1);
    Personagem *inimigos[] = {&polifemo};

    inicializarCombate(&combate);

    /* Fora do combate nada acontece. */
    assert(!atacarNoCombate(&combate, &polifemo));
    assert(!defenderNoCombate(&combate));
    assert(!atacarNoCombate(NULL, &polifemo));
    assert(!defenderNoCombate(NULL));

    assert(prepararCombate(&combate, &odisseu, inimigos, 1));
    atualizarCombate(&combate); /* inicia a rodada: Odisseu age primeiro */
    assert(combate.estado == COMBATE_WAITING_PLAYER);
    assert(combate.participanteAtual == &odisseu);

    /* Alvos invalidos nao gastam o turno. */
    assert(!atacarNoCombate(&combate, NULL));
    assert(!atacarNoCombate(&combate, &odisseu));
    assert(!atacarNoCombate(&combate, &intruso));
    assert(combate.estado == COMBATE_WAITING_PLAYER);
    assert(polifemo.vida == 120);

    /* Turno 1: Odisseu ataca e a acao e encerrada. */
    assert(atacarNoCombate(&combate, &polifemo));
    assert(polifemo.vida == 108);
    assert(combate.estado == COMBATE_RESOLVING);
    assert(combate.turnosConcluidos == 1);

    /* Nao da para agir duas vezes no mesmo turno. */
    assert(!atacarNoCombate(&combate, &polifemo));
    assert(!defenderNoCombate(&combate));
    assert(polifemo.vida == 108);

    atualizarCombate(&combate); /* resolve e passa a vez ao inimigo */
    assert(combate.estado == COMBATE_ENEMY_TURN);
    assert(combate.participanteAtual == &polifemo);

    /* Turno 2: Polifemo ataca Odisseu sem defesa. */
    assert(atacarNoCombate(&combate, &odisseu));
    assert(odisseu.vida == 85);

    atualizarCombate(&combate); /* fim da rodada 1 */
    atualizarCombate(&combate); /* rodada 2 */
    assert(combate.rodada == 2);
    assert(combate.estado == COMBATE_WAITING_PLAYER);

    /* Rodada 2: Odisseu defende e o golpe de Polifemo diminui. */
    assert(defenderNoCombate(&combate));
    assert(odisseu.bonusDefesa == BONUS_DEFESA_ACAO);
    assert(combate.estado == COMBATE_RESOLVING);

    atualizarCombate(&combate);
    assert(combate.participanteAtual == &polifemo);
    assert(atacarNoCombate(&combate, &odisseu));
    assert(odisseu.vida == 85 - (15 - BONUS_DEFESA_ACAO));
    assert(odisseu.bonusDefesa == 0);

    finalizarCombate(&combate);
}

int main(void)
{
    testarFormulaDoDano();
    testarAtaque();
    testarVidaNuncaNegativa();
    testarAtaquesInvalidos();
    testarDefesa();
    testarDefesaForteAindaCausaDanoMinimo();
    testarEventos();
    testarIntegracaoComOControladorDeTurnos();

    printf("\nTESTE DAS ACOES DE COMBATE PASSOU!\n");

    return 0;
}

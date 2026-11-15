#ifndef ACOES_COMBATE_H
#define ACOES_COMBATE_H

#include <stdbool.h>

#include "combate.h"
#include "personagem.h"

/* Regras de ataque, defesa e dano.
 * Modulo de C puro: nao usa a Raylib. Animacao, HUD e som recebem o que
 * aconteceu por eventos (registrarObservadorCombate) e nunca ficam dentro
 * das regras. */

/* Nenhum golpe causa menos que isto, por mais forte que seja a defesa. */
#define DANO_MINIMO 1

/* Defesa extra ganha ao escolher "defender". */
#define BONUS_DEFESA_ACAO 5

typedef enum {
    EVENTO_COMBATE_ATAQUE,
    EVENTO_COMBATE_DEFESA
} TipoEventoCombate;

/* Descricao do que acabou de acontecer, so para leitura. */
typedef struct {
    TipoEventoCombate tipo;
    const Personagem *origem;  /* quem agiu */
    const Personagem *alvo;    /* quem foi atingido (na defesa, o proprio) */
    int dano;                  /* vida realmente perdida pelo alvo */
    bool alvoDefendia;         /* a defesa do alvo reduziu este golpe */
    bool alvoDerrotado;        /* a vida do alvo chegou a zero */
} EventoCombate;

typedef void (*ObservadorCombate)(const EventoCombate *evento, void *contexto);

/* Unico ponto onde a formula do dano existe:
 *   dano = max(DANO_MINIMO, ataque - defesaAlvo - bonusDefesa) */
int calcularDano(int ataque, int defesaAlvo, int bonusDefesa);

/* `atacante` golpeia `alvo`. Devolve a vida que o alvo perdeu (nunca mais
 * do que ele tinha, entao a vida nunca fica negativa) ou 0 se o ataque nao
 * e valido: ponteiro nulo, o proprio personagem, alguem derrotado ou alguem
 * do mesmo lado. O bonus de defesa do alvo vale so para este golpe, e a
 * defesa antiga do atacante acaba ao ele agir de novo. */
int realizarAtaque(Personagem *atacante, Personagem *alvo);

/* `personagem` assume postura de defesa: o proximo golpe que sofrer sera
 * reduzido em BONUS_DEFESA_ACAO. Devolve false se ele estiver derrotado. */
bool realizarDefesa(Personagem *personagem);

/* Registra quem recebe os eventos (NULL remove). Ha um unico observador. */
void registrarObservadorCombate(ObservadorCombate observador, void *contexto);

/* Versoes ligadas ao controlador de turnos: agem em nome do participante da
 * vez, conferem se e o momento certo e encerram a acao dele. Devolvem false,
 * sem gastar o turno, se a acao nao pode ser feita agora. */
bool atacarNoCombate(Combate *combate, Personagem *alvo);
bool defenderNoCombate(Combate *combate);

#endif

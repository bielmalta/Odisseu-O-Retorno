#ifndef IA_INIMIGO_H
#define IA_INIMIGO_H

#include <stdbool.h>

#include "combate.h"
#include "personagem.h"

/* Inteligencia artificial basica dos inimigos.
 * Modulo de C puro: nao usa a Raylib nem numeros aleatorios, entao a mesma
 * situacao de combate sempre produz a mesma decisao. */

/* Abaixo desta porcentagem da vida maxima o inimigo passa a se defender. */
#define IA_LIMIAR_DEFESA_PORCENTO 30

/* Pausa, em segundos, antes de o inimigo agir. Serve so para o jogador
 * enxergar de quem e a vez; a decisao e a acao nao dependem dela. */
#define IA_INTERVALO_TURNO 0.6f

typedef enum {
    ACAO_INIMIGO_ATACAR,
    ACAO_INIMIGO_DEFENDER
} AcaoInimigo;

/* Escolhe o alvo de `inimigo`: o primeiro heroi vivo na ordem atual da
 * lista de turnos. Nunca devolve um personagem derrotado, alguem fora do
 * combate ou do mesmo lado; devolve NULL se nao houver alvo valido. */
Personagem *selecionarAlvoInimigo(const Combate *combate,
                                  const Personagem *inimigo);

/* Regras, nesta prioridade:
 * 1. se o golpe derrota o alvo, ataca;
 * 2. se a vida esta em ate IA_LIMIAR_DEFESA_PORCENTO % e ele ainda nao esta
 *    em guarda, defende;
 * 3. caso contrario, ataca.
 * Como a guarda so e renovada quando ja foi gasta, o inimigo nunca passa
 * dois turnos seguidos apenas defendendo. */
AcaoInimigo escolherAcaoInimigo(const Personagem *inimigo,
                                const Personagem *alvo);

/* Executa o turno do participante atual quando ele e um inimigo, sem
 * intervencao do jogador. Se nenhuma acao for possivel, o turno e
 * encerrado mesmo assim, para o combate nunca travar.
 * Devolve false se nao for a vez de um inimigo. */
bool executarTurnoInimigo(Combate *combate);

/* Controle do intervalo visual antes da acao do inimigo. */
typedef struct {
    const Personagem *participante;
    unsigned int rodada;
    int turnosConcluidos;
    float decorrido;
} EsperaTurnoInimigo;

void iniciarEsperaTurnoInimigo(EsperaTurnoInimigo *espera);

/* Chamada a cada quadro antes de atualizarCombate(). Devolve true quando o
 * combate pode ser atualizado: sempre fora do turno inimigo e, no turno
 * inimigo, depois de IA_INTERVALO_TURNO segundos. Nao bloqueia o laco do
 * jogo: apenas soma o tempo do quadro. */
bool aguardarTurnoInimigo(EsperaTurnoInimigo *espera,
                          const Combate *combate,
                          float deltaTempo);

#endif

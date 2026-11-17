
#ifndef COMBATE_H
#define COMBATE_H

#include <stdbool.h>

#include "identidade_fase.h"
#include "lista_turnos.h"
#include "personagem.h"

/* Regras de combate por turnos em C puro, sem Raylib ou interface. */
/* Estados do controlador de combate. */
typedef enum {
    COMBATE_SETUP,
    COMBATE_ROUND_START,
    COMBATE_WAITING_PLAYER,
    COMBATE_ENEMY_TURN,
    COMBATE_RESOLVING,
    COMBATE_VICTORY,
    COMBATE_DEFEAT
} EstadoCombate;

/* Controla a ordem e o andamento das rodadas. */
typedef struct {
    ListaTurnos turnos;
    EstadoCombate estado;
    unsigned int rodada;
    int turnosConcluidos;
    Personagem *jogador;
    Personagem *participanteAtual;

    /* Regras especiais do encontro atual. Permanecem em C puro: a interface
     * apenas consulta estes sinais para explicar o que esta acontecendo. */
    IdentidadeFase identidade;
    bool golpePesadoPreparado;
    unsigned int rodadaUltimoEncanto;
    int penalidadeIniciativaPendente;
    int inimigosDerrotados;
    bool vantagemFurtiva;
} Combate;

/* Prepara o controlador para receber participantes. */
void inicializarCombate(Combate *combate);

/* Insere o jogador e os inimigos na lista circular. */
bool prepararCombate(
    Combate *combate,
    Personagem *jogador,
    Personagem **inimigos,
    int quantidadeInimigos
);

/* Liga ao controlador as regras exclusivas da fase. Chamar depois de
 * prepararCombate() e antes do primeiro atualizarCombate(). */
void configurarIdentidadeCombate(
    Combate *combate,
    IdentidadeFase identidade,
    bool vantagemFurtiva
);

/* Atualiza a maquina de estados do combate. */
void atualizarCombate(Combate *combate);

/* Remove inimigos derrotados e informa se o combate terminou. */
bool verificarFimCombate(Combate *combate);

/* Finaliza a acao do participante atual. */
void concluirAcaoCombate(Combate *combate);

/* Libera os nos da lista e reinicializa o controlador. */
void finalizarCombate(Combate *combate);

#endif

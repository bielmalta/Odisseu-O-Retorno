
#include "combate.h"
#include "ia_inimigo.h"
#include "quicksort.h"

#include <stddef.h>

void inicializarCombate(Combate *combate)
{
    if (combate == NULL) {
        return;
    }

    iniciarLista(&combate->turnos);
    combate->estado = COMBATE_SETUP;
    combate->rodada = 0;
    combate->turnosConcluidos = 0;
    combate->jogador = NULL;
    combate->participanteAtual = NULL;
    combate->identidade = IDENTIDADE_NENHUMA;
    combate->golpePesadoPreparado = false;
    combate->rodadaUltimoEncanto = 0;
    combate->penalidadeIniciativaPendente = 0;
    combate->inimigosDerrotados = 0;
    combate->vantagemFurtiva = false;
}

bool prepararCombate(
    Combate *combate,
    Personagem *jogador,
    Personagem **inimigos,
    int quantidadeInimigos
)
{
    if (combate == NULL || jogador == NULL ||
        inimigos == NULL || quantidadeInimigos <= 0 ||
        !personagemVivo(jogador) ||
        jogador->tipo != TIPO_HEROI) {
        return false;
    }

    /* Valida todos os participantes antes de modificar a lista. */
    for (int i = 0; i < quantidadeInimigos; i++) {
        if (inimigos[i] == NULL ||
            !personagemVivo(inimigos[i]) ||
            inimigos[i]->tipo != TIPO_INIMIGO ||
            inimigos[i] == jogador) {
            return false;
        }

        for (int j = 0; j < i; j++) {
            if (inimigos[i] == inimigos[j]) {
                return false;
            }
        }
    }

    /* O controlador deve ter sido inicializado e estar vazio. */
    if (combate->turnos.tamanho != 0 ||
        combate->estado != COMBATE_SETUP) {
        return false;
    }

    combate->jogador = jogador;

    if (!inserirPersonagem(&combate->turnos, jogador)) {
        combate->jogador = NULL;
        return false;
    }

    for (int i = 0; i < quantidadeInimigos; i++) {
        if (!inserirPersonagem(&combate->turnos, inimigos[i])) {
            limparListaCombate(&combate->turnos);
            combate->jogador = NULL;
            return false;
        }
    }

    combate->rodada = 0;
    combate->turnosConcluidos = 0;
    combate->participanteAtual = NULL;
    combate->estado = COMBATE_ROUND_START;

    return true;
}

void configurarIdentidadeCombate(
    Combate *combate,
    IdentidadeFase identidade,
    bool vantagemFurtiva
)
{
    if (combate == NULL || combate->estado == COMBATE_SETUP) {
        return;
    }

    combate->identidade = identidade;
    combate->vantagemFurtiva = vantagemFurtiva;
    combate->golpePesadoPreparado = false;
    combate->rodadaUltimoEncanto = 0;
    combate->penalidadeIniciativaPendente = 0;
    combate->inimigosDerrotados = 0;

    if (vantagemFurtiva && combate->jogador != NULL) {
        combate->jogador->bonusIniciativa += 5;
    }
}

static void iniciarRodada(Combate *combate)
{
    if (combate->jogador != NULL &&
        combate->penalidadeIniciativaPendente != 0) {
        combate->jogador->bonusIniciativa +=
            combate->penalidadeIniciativaPendente;
        combate->penalidadeIniciativaPendente = 0;
    }

    if (!ordenarPorVelocidade(&combate->turnos)) {
        return;
    }

    combate->rodada++;
    combate->turnosConcluidos = 0;
    combate->participanteAtual = personagemDaVez(&combate->turnos);

    if (combate->participanteAtual == combate->jogador) {
        combate->estado = COMBATE_WAITING_PLAYER;
    } else {
        combate->estado = COMBATE_ENEMY_TURN;
    }
}

static void enfurecerTentaculosRestantes(Combate *combate)
{
    NoTurno *no = combate->turnos.inicio;

    for (int i = 0; i < combate->turnos.tamanho; i++) {
        Personagem *personagem = no->personagem;

        if (personagem->tipo == TIPO_INIMIGO && personagemVivo(personagem)) {
            personagem->iniciativa += 2;
        }
        no = no->proximo;
    }
}

static bool participanteJaAgiu(
    const Combate *combate,
    const Personagem *personagem
)
{
    NoTurno *no;
    int quantidade;

    if (combate->turnos.atual == NULL ||
        combate->turnosConcluidos <= 0) {
        return false;
    }

    no = combate->turnos.atual;
    quantidade = combate->turnosConcluidos;

    /* Em RESOLVING, o participante atual ja concluiu sua acao. */
    if (combate->estado != COMBATE_RESOLVING) {
        no = no->anterior;
    }

    for (int i = 0; i < quantidade; i++) {
        if (no->personagem == personagem) {
            return true;
        }

        no = no->anterior;
    }

    return false;
}

bool verificarFimCombate(Combate *combate)
{
    NoTurno *no;
    int quantidadeOriginal;
    int inimigosRestantes = 0;

    if (combate == NULL || combate->jogador == NULL) {
        return false;
    }

    if (combate->estado == COMBATE_VICTORY ||
        combate->estado == COMBATE_DEFEAT) {
        return true;
    }

    if (!personagemVivo(combate->jogador)) {
        combate->participanteAtual = NULL;
        combate->estado = COMBATE_DEFEAT;
        return true;
    }

    no = combate->turnos.inicio;
    quantidadeOriginal = combate->turnos.tamanho;

    for (int i = 0; i < quantidadeOriginal; i++) {
        NoTurno *proximo = no->proximo;
        Personagem *personagem = no->personagem;

        if (personagem->tipo == TIPO_INIMIGO) {
            if (personagemVivo(personagem)) {
                inimigosRestantes++;
            } else {
                bool eraParticipanteAtual =
                    personagem == combate->participanteAtual;

                if (participanteJaAgiu(combate, personagem) &&
                    combate->turnosConcluidos > 0) {
                    combate->turnosConcluidos--;
                }

                removerPersonagem(&combate->turnos, personagem);
                combate->inimigosDerrotados++;

                if (combate->identidade == IDENTIDADE_CILA) {
                    /* Cada cabeca perdida acelera as restantes e muda a ordem
                     * real calculada pelo QuickSort. */
                    enfurecerTentaculosRestantes(combate);
                }

                if (eraParticipanteAtual) {
                    combate->participanteAtual =
                        personagemDaVez(&combate->turnos);

                    if (combate->estado == COMBATE_WAITING_PLAYER ||
                        combate->estado == COMBATE_ENEMY_TURN) {
                        combate->estado = COMBATE_RESOLVING;
                    }
                }
            }
        }

        no = proximo;
    }

    if (inimigosRestantes == 0) {
        combate->participanteAtual = NULL;
        combate->estado = COMBATE_VICTORY;
        return true;
    }

    return false;
}

void concluirAcaoCombate(Combate *combate)
{
    if (combate == NULL || combate->participanteAtual == NULL) {
        return;
    }

    if (combate->estado != COMBATE_WAITING_PLAYER &&
        combate->estado != COMBATE_ENEMY_TURN) {
        return;
    }

    if (verificarFimCombate(combate)) {
        return;
    }

    /* A remocao do participante atual muda o estado para RESOLVING. */
    if (combate->estado == COMBATE_RESOLVING) {
        return;
    }

    combate->turnosConcluidos++;
    combate->estado = COMBATE_RESOLVING;
}

static void resolverTurno(Combate *combate)
{
    if (verificarFimCombate(combate)) {
        return;
    }

    if (combate->turnosConcluidos >= combate->turnos.tamanho) {
        combate->participanteAtual = NULL;
        combate->estado = COMBATE_ROUND_START;
        return;
    }

    /* Avanca pela lista circular real, sem usar um vetor paralelo. */
    combate->participanteAtual = avancarTurno(&combate->turnos);

    if (combate->participanteAtual == combate->jogador) {
        combate->estado = COMBATE_WAITING_PLAYER;
    } else {
        combate->estado = COMBATE_ENEMY_TURN;
    }
}

void atualizarCombate(Combate *combate)
{
    if (combate == NULL) {
        return;
    }

    if (verificarFimCombate(combate)) {
        return;
    }

    switch (combate->estado) {
        case COMBATE_SETUP:
            /* Aguarda prepararCombate(). */
            break;

        case COMBATE_ROUND_START:
            iniciarRodada(combate);
            break;

        case COMBATE_WAITING_PLAYER:
            /* Aguarda um comando do jogador. */
            break;

        case COMBATE_ENEMY_TURN:
            /* O inimigo decide e age sozinho (ia_inimigo.c). */
            executarTurnoInimigo(combate);
            break;

        case COMBATE_RESOLVING:
            resolverTurno(combate);
            break;

        case COMBATE_VICTORY:
        case COMBATE_DEFEAT:
            /* Entradas de combate ficam bloqueadas apos o resultado. */
            break;
    }
}

void finalizarCombate(Combate *combate)
{
    if (combate == NULL) {
        return;
    }

    limparListaCombate(&combate->turnos);
    inicializarCombate(combate);
}

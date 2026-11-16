#include "ritmo_combate.h"

#include <stddef.h>

void iniciarEsperaTurnoInimigo(EsperaTurnoInimigo *espera)
{
    if (espera == NULL) {
        return;
    }

    espera->participante = NULL;
    espera->rodada = 0;
    espera->turnosConcluidos = -1;
    espera->decorrido = 0.0f;
}

bool aguardarTurnoInimigo(
    EsperaTurnoInimigo *espera,
    const Combate *combate,
    float deltaTempo
)
{
    if (espera == NULL || combate == NULL ||
        combate->estado != COMBATE_ENEMY_TURN) {
        iniciarEsperaTurnoInimigo(espera);
        return true;
    }

    if (espera->participante != combate->participanteAtual ||
        espera->rodada != combate->rodada ||
        espera->turnosConcluidos != combate->turnosConcluidos) {
        espera->participante = combate->participanteAtual;
        espera->rodada = combate->rodada;
        espera->turnosConcluidos = combate->turnosConcluidos;
        espera->decorrido = 0.0f;
    }

    if (deltaTempo > 0.0f) {
        espera->decorrido += deltaTempo;
    }

    return espera->decorrido >= INTERVALO_VISUAL_TURNO_INIMIGO;
}

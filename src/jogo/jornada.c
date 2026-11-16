#include "jornada.h"

#include <string.h>

bool iniciarJornada(
    Jornada *jornada,
    Personagem odisseu,
    int quantidadeEtapas
)
{
    if (jornada == NULL || quantidadeEtapas <= 0 ||
        odisseu.tipo != TIPO_HEROI || !personagemVivo(&odisseu)) {
        return false;
    }

    memset(jornada, 0, sizeof *jornada);
    jornada->indiceEtapaAtual = 0;
    jornada->quantidadeEtapas = quantidadeEtapas;
    jornada->odisseu = odisseu;
    return true;
}

void registrarVitoriaJornada(Jornada *jornada)
{
    if (jornada == NULL || jornada->concluida) {
        return;
    }

    jornada->combateVencido = true;
}

ResultadoAvancoJornada avancarJornada(
    Jornada *jornada,
    bool objetivoCumprido,
    bool saidaAlcancada
)
{
    int proximoIndice;

    if (jornada == NULL || jornada->concluida ||
        !objetivoCumprido || !jornada->combateVencido ||
        !saidaAlcancada) {
        return JORNADA_AVANCO_BLOQUEADO;
    }

    proximoIndice = jornada->indiceEtapaAtual + 1;
    if (proximoIndice >= jornada->quantidadeEtapas) {
        jornada->concluida = true;
        return JORNADA_RETORNO_AO_LAR;
    }

    jornada->indiceEtapaAtual = proximoIndice;
    jornada->combateVencido = false;
    return JORNADA_PROXIMA_FASE;
}

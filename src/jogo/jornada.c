#include "jornada.h"

#include <string.h>

bool iniciarJornada(Jornada *jornada, Fase *fase)
{
    const ConfiguracaoFase *configuracao;

    if (jornada == NULL || fase == NULL) {
        return false;
    }

    configuracao = obterConfiguracaoFase(0);
    if (configuracao == NULL || !criarFase(fase, configuracao)) {
        return false;
    }

    memset(jornada, 0, sizeof *jornada);
    jornada->indiceEtapaAtual = 0;
    jornada->odisseu = criarPersonagemConfigurada(&configuracao->heroi);
    return true;
}

void registrarVitoriaJornada(Jornada *jornada)
{
    if (jornada == NULL || jornada->concluida) {
        return;
    }

    jornada->combateVencido = true;
}

ResultadoAvancoJornada avancarJornada(Jornada *jornada, Fase *fase)
{
    int proximoIndice;
    const ConfiguracaoFase *configuracaoAtual;
    const ConfiguracaoFase *proximaConfiguracao;
    Fase proximaFase;

    if (jornada == NULL || fase == NULL || jornada->concluida) {
        return JORNADA_AVANCO_BLOQUEADO;
    }

    configuracaoAtual = obterConfiguracaoFase(jornada->indiceEtapaAtual);
    if (configuracaoAtual == NULL || fase->configuracao != configuracaoAtual ||
        !verificarObjetivo(&fase->objetivo) ||
        !jornada->combateVencido || fase->etapa != ETAPA_CONCLUIDA) {
        return JORNADA_AVANCO_BLOQUEADO;
    }

    proximoIndice = jornada->indiceEtapaAtual + 1;
    if (proximoIndice >= quantidadeConfiguracoesFase()) {
        jornada->concluida = true;
        return JORNADA_RETORNO_AO_LAR;
    }

    proximaConfiguracao = obterConfiguracaoFase(proximoIndice);
    if (proximaConfiguracao == NULL ||
        !criarFase(&proximaFase, proximaConfiguracao)) {
        return JORNADA_AVANCO_BLOQUEADO;
    }

    *fase = proximaFase;
    jornada->indiceEtapaAtual = proximoIndice;
    jornada->combateVencido = false;
    return JORNADA_PROXIMA_FASE;
}

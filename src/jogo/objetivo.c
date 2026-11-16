#include "objetivo.h"

#include <stddef.h>

Objetivo criarObjetivo(const ConfiguracaoObjetivo *configuracao)
{
    Objetivo objetivo = {0};

    if (configuracao == NULL) {
        return objetivo;
    }

    objetivo.tipo = configuracao->tipo;
    objetivo.descricao = configuracao->descricao;
    objetivo.nomeAlvo = configuracao->nomeAlvo;
    objetivo.necessario = configuracao->quantidadeNecessaria > 0
                              ? configuracao->quantidadeNecessaria
                              : 1;
    return objetivo;
}

void iniciarObjetivo(Objetivo *objetivo)
{
    if (objetivo == NULL) {
        return;
    }

    objetivo->progresso = 0;
    objetivo->cumprido = false;
    objetivo->iniciado = true;
}

void concluirObjetivo(Objetivo *objetivo)
{
    if (objetivo == NULL || !objetivo->iniciado) {
        return;
    }

    objetivo->progresso = objetivo->necessario;
    objetivo->cumprido = true;
}

bool registrarProgresso(Objetivo *objetivo, int quantidade)
{
    if (objetivo == NULL || !objetivo->iniciado || objetivo->cumprido ||
        quantidade <= 0) {
        return false;
    }

    if (quantidade >= objetivo->necessario - objetivo->progresso) {
        concluirObjetivo(objetivo);
        return true;
    }

    objetivo->progresso += quantidade;
    return false;
}

bool verificarObjetivo(const Objetivo *objetivo)
{
    return objetivo != NULL && objetivo->iniciado && objetivo->cumprido;
}

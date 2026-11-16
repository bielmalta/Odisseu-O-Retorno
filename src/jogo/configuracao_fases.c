#include "configuracao_fases.h"

#include <stddef.h>

#define QUANTIDADE(array) ((int)(sizeof(array) / sizeof((array)[0])))
#define COR_PEDRA { 150, 135, 110, 255 }
#define COR_PEDRA_ESCURA { 120, 108, 90, 255 }
#define COR_PORTAO { 140, 50, 40, 255 }

static const ConfiguracaoCaixaCenario CAIXAS_CAVERNA[] = {
    { { 0.0f, 2.0f, 20.5f }, { 42.0f, 4.0f, 1.0f }, COR_PEDRA, false },
    { { -20.5f, 2.0f, 0.0f }, { 1.0f, 4.0f, 42.0f }, COR_PEDRA, false },
    { { 20.5f, 2.0f, 0.0f }, { 1.0f, 4.0f, 42.0f }, COR_PEDRA, false },
    { { -11.5f, 2.0f, -20.5f }, { 19.0f, 4.0f, 1.0f }, COR_PEDRA, false },
    { { 11.5f, 2.0f, -20.5f }, { 19.0f, 4.0f, 1.0f }, COR_PEDRA, false },
    { { 0.0f, 2.0f, -23.5f }, { 5.0f, 4.0f, 1.0f }, COR_PEDRA_ESCURA, false },
    { { -2.5f, 2.0f, -22.0f }, { 1.0f, 4.0f, 4.0f }, COR_PEDRA_ESCURA, false },
    { { 2.5f, 2.0f, -22.0f }, { 1.0f, 4.0f, 4.0f }, COR_PEDRA_ESCURA, false },
    { { 0.0f, 2.0f, -20.5f }, { 4.0f, 4.0f, 1.0f }, COR_PORTAO, true },
    { { 0.0f, 2.0f, -8.0f }, { 6.0f, 4.0f, 1.0f }, COR_PEDRA, false },
    { { -8.0f, 2.0f, 4.0f }, { 2.0f, 4.0f, 2.0f }, COR_PEDRA_ESCURA, false },
    { { 8.0f, 2.0f, 4.0f }, { 2.0f, 4.0f, 2.0f }, COR_PEDRA_ESCURA, false },
    { { 6.0f, 0.4f, -2.0f }, { 2.0f, 0.8f, 2.0f }, COR_PEDRA, false },
    { { 8.5f, 0.8f, -2.0f }, { 3.0f, 1.6f, 2.0f }, COR_PEDRA, false }
};

static const ConfiguracaoItemFase ITENS_CAVERNA[] = {
    { { 8.5f, 1.75f, -2.0f } },
    { { -9.8f, 0.15f, 2.5f } },
    { { -15.0f, 0.15f, -15.0f } }
};

static const ConfiguracaoInimigo INIMIGOS_POLIFEMO[] = {
    {
        { "Polifemo", TIPO_INIMIGO, 120, 25, 8, 8 },
        { 0.0f, 0.0f, -2.0f },
        {
            "ciclope_primitivo",
            { 120, 80, 70, 255 },
            { 150, 105, 90, 255 },
            1.0f
        }
    }
};

static const ConfiguracaoFase FASES[] = {
    {
        .id = "caverna_polifemo",
        .titulo = "Fase 1 - A Caverna de Polifemo",
        .instrucaoInicial = "Explore a caverna e cumpra o objetivo",
        .pontoInicial = { 0.0f, 0.0f, 14.0f },
        .saida = {
            { -1.8f, 0.0f, -23.0f },
            { 1.8f, 3.0f, -21.2f }
        },
        .caixas = CAIXAS_CAVERNA,
        .quantidadeCaixas = QUANTIDADE(CAIXAS_CAVERNA),
        .corChao = { 90, 80, 60, 255 },
        .tamanhoChao = { 40.0f, 40.0f },
        .divisoesGrade = 40,
        .itens = ITENS_CAVERNA,
        .quantidadeItens = QUANTIDADE(ITENS_CAVERNA),
        .tamanhoItem = { 0.9f, 0.3f, 0.3f },
        .corItem = { 130, 85, 40, 255 },
        .corContornoItem = { 60, 35, 15, 255 },
        .objetivo = {
            OBJETIVO_COLETA,
            "Encontre pecas de madeira para preparar a estaca",
            "peca de madeira",
            3
        },
        .heroi = { "Odisseu", TIPO_HEROI, 100, 20, 10, 12 },
        .encontro = {
            "Confronto com Polifemo",
            INIMIGOS_POLIFEMO,
            QUANTIDADE(INIMIGOS_POLIFEMO),
            { 0.0f, 0.0f, 6.0f },
            "Polifemo foi vencido. A passagem da caverna se abriu!"
        },
        .mensagemObjetivoConcluido =
            "Estaca pronta! Polifemo surge diante de voce",
        .mensagemFaseConcluida =
            "Fase concluida! A jornada continua...",
        .textoObjetivoConfronto = "Objetivo: derrote Polifemo",
        .textoObjetivoSaida =
            "Objetivo: ache a saida da caverna, ao norte"
    }
};

int quantidadeConfiguracoesFase(void)
{
    return QUANTIDADE(FASES);
}

const ConfiguracaoFase *obterConfiguracaoFase(int indice)
{
    if (indice < 0 || indice >= quantidadeConfiguracoesFase()) {
        return NULL;
    }
    return &FASES[indice];
}

Personagem criarPersonagemConfigurada(
    const ConfiguracaoPersonagem *configuracao
)
{
    if (configuracao == NULL) {
        return criarPersonagem(NULL, TIPO_INIMIGO, 1, 0, 0, 0);
    }

    return criarPersonagem(
        configuracao->nome,
        configuracao->tipo,
        configuracao->vidaMaxima,
        configuracao->ataque,
        configuracao->defesa,
        configuracao->iniciativa
    );
}

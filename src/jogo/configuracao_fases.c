#include "configuracao_fases.h"

#include <stddef.h>

#define QUANTIDADE(array) ((int)(sizeof(array) / sizeof((array)[0])))

#define COR_PEDRA { 150, 135, 110, 255 }
#define COR_PEDRA_ESCURA { 120, 108, 90, 255 }
#define COR_PORTAO { 140, 50, 40, 255 }
#define COR_MADEIRA { 112, 72, 42, 255 }
#define COR_MADEIRA_ESCURA { 74, 45, 30, 255 }
#define COR_ROCHA { 82, 88, 98, 255 }
#define COR_ROCHA_ESCURA { 48, 54, 65, 255 }
#define COR_MARMORE { 206, 196, 178, 255 }
#define COR_MARMORE_ESCURO { 145, 132, 116, 255 }

/* ---- Fase 1: Caverna de Polifemo ---- */

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
    { { 8.5f, 1.75f, -2.0f }, true },
    { { -9.8f, 0.15f, 2.5f }, true },
    { { -15.0f, 0.15f, -15.0f }, true }
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

/* ---- Fase 2: Canto das Sereias ---- */

static const ConfiguracaoCaixaCenario CAIXAS_SEREIAS[] = {
    { { -9.5f, 1.0f, 0.0f }, { 1.0f, 2.0f, 24.0f }, COR_MADEIRA, false },
    { { 9.5f, 1.0f, 0.0f }, { 1.0f, 2.0f, 24.0f }, COR_MADEIRA, false },
    { { 0.0f, 1.0f, 11.5f }, { 20.0f, 2.0f, 1.0f }, COR_MADEIRA, false },
    { { -6.5f, 1.0f, -11.5f }, { 7.0f, 2.0f, 1.0f }, COR_MADEIRA, false },
    { { 6.5f, 1.0f, -11.5f }, { 7.0f, 2.0f, 1.0f }, COR_MADEIRA, false },
    { { 0.0f, 1.0f, -11.5f }, { 6.0f, 2.0f, 1.0f }, COR_PORTAO, true },
    { { 0.0f, 2.5f, 0.0f }, { 1.2f, 5.0f, 1.2f }, COR_MADEIRA_ESCURA, false },
    { { -6.0f, 0.6f, 5.0f }, { 2.5f, 1.2f, 2.5f }, COR_MADEIRA_ESCURA, false },
    { { 6.0f, 0.6f, 5.0f }, { 2.5f, 1.2f, 2.5f }, COR_MADEIRA_ESCURA, false }
};

static const ConfiguracaoItemFase ITENS_SEREIAS[] = {
    { { -6.0f, 0.45f, 1.5f }, false },
    { { 0.0f, 0.45f, -3.0f }, false },
    { { 6.0f, 0.45f, 1.5f }, false }
};

static const ConfiguracaoInimigo INIMIGOS_SEREIAS[] = {
    {
        { "Sereia de Ligeia", TIPO_INIMIGO, 36, 11, 3, 14 },
        { -3.0f, 0.0f, -4.0f },
        {
            "sereia_primitiva",
            { 70, 105, 160, 255 },
            { 205, 175, 150, 255 },
            0.85f
        }
    },
    {
        { "Sereia de Leucosia", TIPO_INIMIGO, 40, 12, 4, 11 },
        { 3.0f, 0.0f, -4.0f },
        {
            "sereia_primitiva",
            { 95, 75, 150, 255 },
            { 210, 180, 155, 255 },
            0.9f
        }
    }
};

/* ---- Fase 3: Estreito de Cila ---- */

static const ConfiguracaoCaixaCenario CAIXAS_CILA[] = {
    { { -10.5f, 2.0f, 0.0f }, { 1.0f, 4.0f, 26.0f }, COR_ROCHA, false },
    { { 10.5f, 2.0f, 0.0f }, { 1.0f, 4.0f, 26.0f }, COR_ROCHA, false },
    { { 0.0f, 2.0f, 12.5f }, { 22.0f, 4.0f, 1.0f }, COR_ROCHA_ESCURA, false },
    { { -7.0f, 2.0f, -12.5f }, { 8.0f, 4.0f, 1.0f }, COR_ROCHA, false },
    { { 7.0f, 2.0f, -12.5f }, { 8.0f, 4.0f, 1.0f }, COR_ROCHA, false },
    { { 0.0f, 2.0f, -12.5f }, { 6.0f, 4.0f, 1.0f }, COR_PORTAO, true },
    { { -7.0f, 1.5f, -3.0f }, { 3.0f, 3.0f, 3.0f }, COR_ROCHA_ESCURA, false },
    { { 7.0f, 1.5f, -3.0f }, { 3.0f, 3.0f, 3.0f }, COR_ROCHA_ESCURA, false },
    { { 0.0f, 0.6f, 5.5f }, { 5.0f, 1.2f, 2.0f }, COR_MADEIRA_ESCURA, false }
};

static const ConfiguracaoItemFase ITENS_CILA[] = {
    { { -7.0f, 0.55f, 4.0f }, false },
    { { 0.0f, 0.55f, -1.0f }, false },
    { { 7.0f, 0.55f, 4.0f }, false }
};

static const ConfiguracaoInimigo INIMIGOS_CILA[] = {
    {
        { "Tentaculo de Cila I", TIPO_INIMIGO, 28, 10, 2, 13 },
        { -4.5f, 0.0f, -5.0f },
        {
            "tentaculo_primitivo",
            { 85, 115, 90, 255 },
            { 115, 145, 105, 255 },
            0.9f
        }
    },
    {
        { "Tentaculo de Cila II", TIPO_INIMIGO, 32, 11, 3, 10 },
        { 0.0f, 0.0f, -6.0f },
        {
            "tentaculo_primitivo",
            { 75, 105, 82, 255 },
            { 105, 135, 98, 255 },
            1.0f
        }
    },
    {
        { "Tentaculo de Cila III", TIPO_INIMIGO, 28, 10, 2, 7 },
        { 4.5f, 0.0f, -5.0f },
        {
            "tentaculo_primitivo",
            { 90, 120, 95, 255 },
            { 120, 150, 110, 255 },
            0.9f
        }
    }
};

/* ---- Fase 4: Retorno a Itaca ---- */

static const ConfiguracaoCaixaCenario CAIXAS_ITACA[] = {
    { { -15.5f, 2.5f, 0.0f }, { 1.0f, 5.0f, 32.0f }, COR_MARMORE, false },
    { { 15.5f, 2.5f, 0.0f }, { 1.0f, 5.0f, 32.0f }, COR_MARMORE, false },
    { { 0.0f, 2.5f, 15.5f }, { 32.0f, 5.0f, 1.0f }, COR_MARMORE, false },
    { { -10.0f, 2.5f, -15.5f }, { 11.0f, 5.0f, 1.0f }, COR_MARMORE, false },
    { { 10.0f, 2.5f, -15.5f }, { 11.0f, 5.0f, 1.0f }, COR_MARMORE, false },
    { { 0.0f, 2.5f, -15.5f }, { 9.0f, 5.0f, 1.0f }, COR_PORTAO, true },
    { { -8.0f, 2.0f, 2.0f }, { 1.4f, 4.0f, 1.4f }, COR_MARMORE_ESCURO, false },
    { { 8.0f, 2.0f, 2.0f }, { 1.4f, 4.0f, 1.4f }, COR_MARMORE_ESCURO, false },
    { { -8.0f, 2.0f, -7.0f }, { 1.4f, 4.0f, 1.4f }, COR_MARMORE_ESCURO, false },
    { { 8.0f, 2.0f, -7.0f }, { 1.4f, 4.0f, 1.4f }, COR_MARMORE_ESCURO, false },
    { { 0.0f, 0.6f, 0.0f }, { 4.0f, 1.2f, 2.0f }, COR_MADEIRA, false }
};

static const ConfiguracaoItemFase ITENS_ITACA[] = {
    { { 0.0f, 1.45f, 0.0f }, true }
};

static const ConfiguracaoInimigo INIMIGOS_ITACA[] = {
    {
        { "Antinoo", TIPO_INIMIGO, 42, 13, 5, 12 },
        { -3.0f, 0.0f, -6.0f },
        {
            "pretendente_primitivo",
            { 145, 70, 55, 255 },
            { 205, 165, 130, 255 },
            0.9f
        }
    },
    {
        { "Eurimaco", TIPO_INIMIGO, 46, 14, 5, 9 },
        { 3.0f, 0.0f, -6.0f },
        {
            "pretendente_primitivo",
            { 125, 65, 95, 255 },
            { 200, 160, 125, 255 },
            0.95f
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
    },
    {
        .id = "canto_sereias",
        .titulo = "Fase 2 - O Canto das Sereias",
        .instrucaoInicial = "Proteja os pontos do conves contra o canto",
        .pontoInicial = { 0.0f, 0.0f, 8.0f },
        .saida = {
            { -2.5f, 0.0f, -11.0f },
            { 2.5f, 3.0f, -9.5f }
        },
        .caixas = CAIXAS_SEREIAS,
        .quantidadeCaixas = QUANTIDADE(CAIXAS_SEREIAS),
        .corChao = { 126, 83, 48, 255 },
        .tamanhoChao = { 18.0f, 22.0f },
        .divisoesGrade = 22,
        .itens = ITENS_SEREIAS,
        .quantidadeItens = QUANTIDADE(ITENS_SEREIAS),
        .tamanhoItem = { 0.8f, 0.8f, 0.8f },
        .corItem = { 230, 205, 125, 255 },
        .corContornoItem = { 105, 80, 35, 255 },
        .objetivo = {
            OBJETIVO_INTERACAO,
            "Proteja tres pontos do conves com cera",
            "ponto do conves",
            3
        },
        .heroi = { "Odisseu", TIPO_HEROI, 100, 20, 10, 12 },
        .encontro = {
            "Confronto com as Sereias",
            INIMIGOS_SEREIAS,
            QUANTIDADE(INIMIGOS_SEREIAS),
            { 0.0f, 0.0f, 5.0f },
            "O canto foi vencido. O leme esta livre novamente!"
        },
        .mensagemObjetivoConcluido =
            "O conves esta protegido. As Sereias se aproximam!",
        .mensagemFaseConcluida =
            "Navio em rota. O estreito de Cila se aproxima...",
        .textoObjetivoConfronto = "Objetivo: derrote as Sereias",
        .textoObjetivoSaida = "Objetivo: alcance o leme ao norte"
    },
    {
        .id = "estreito_cila",
        .titulo = "Fase 3 - O Estreito de Cila",
        .instrucaoInicial = "Reative os mecanismos antes do ataque",
        .pontoInicial = { 0.0f, 0.0f, 9.0f },
        .saida = {
            { -2.5f, 0.0f, -12.0f },
            { 2.5f, 3.0f, -10.0f }
        },
        .caixas = CAIXAS_CILA,
        .quantidadeCaixas = QUANTIDADE(CAIXAS_CILA),
        .corChao = { 70, 79, 88, 255 },
        .tamanhoChao = { 20.0f, 24.0f },
        .divisoesGrade = 24,
        .itens = ITENS_CILA,
        .quantidadeItens = QUANTIDADE(ITENS_CILA),
        .tamanhoItem = { 1.1f, 0.8f, 1.1f },
        .corItem = { 155, 125, 65, 255 },
        .corContornoItem = { 70, 55, 30, 255 },
        .objetivo = {
            OBJETIVO_INTERACAO,
            "Reative os tres mecanismos do navio",
            "mecanismo",
            3
        },
        .heroi = { "Odisseu", TIPO_HEROI, 100, 20, 10, 12 },
        .encontro = {
            "Confronto com Cila",
            INIMIGOS_CILA,
            QUANTIDADE(INIMIGOS_CILA),
            { 0.0f, 0.0f, 5.5f },
            "Os tentaculos recuaram. A passagem pelo estreito esta livre!"
        },
        .mensagemObjetivoConcluido =
            "Mecanismos ativos. Os tentaculos de Cila emergem!",
        .mensagemFaseConcluida =
            "O estreito ficou para tras. Itaca esta proxima...",
        .textoObjetivoConfronto =
            "Objetivo: derrote os tentaculos de Cila",
        .textoObjetivoSaida = "Objetivo: alcance a proa ao norte"
    },
    {
        .id = "retorno_itaca",
        .titulo = "Fase 4 - O Retorno a Itaca",
        .instrucaoInicial = "Recupere o arco no salao do palacio",
        .pontoInicial = { 0.0f, 0.0f, 11.0f },
        .saida = {
            { -3.5f, 0.0f, -15.0f },
            { 3.5f, 3.5f, -13.0f }
        },
        .caixas = CAIXAS_ITACA,
        .quantidadeCaixas = QUANTIDADE(CAIXAS_ITACA),
        .corChao = { 190, 180, 160, 255 },
        .tamanhoChao = { 30.0f, 30.0f },
        .divisoesGrade = 30,
        .itens = ITENS_ITACA,
        .quantidadeItens = QUANTIDADE(ITENS_ITACA),
        .tamanhoItem = { 1.6f, 0.35f, 0.5f },
        .corItem = { 115, 70, 35, 255 },
        .corContornoItem = { 55, 30, 15, 255 },
        .objetivo = {
            OBJETIVO_COLETA,
            "Recupere o arco de Odisseu",
            "arco",
            1
        },
        .heroi = { "Odisseu", TIPO_HEROI, 100, 20, 10, 12 },
        .encontro = {
            "Confronto final em Itaca",
            INIMIGOS_ITACA,
            QUANTIDADE(INIMIGOS_ITACA),
            { 0.0f, 0.0f, 5.0f },
            "Os pretendentes foram vencidos. O caminho ate Penelope se abriu!"
        },
        .mensagemObjetivoConcluido =
            "Arco recuperado. Os pretendentes bloqueiam o salao!",
        .mensagemFaseConcluida =
            "Odisseu finalmente reencontrou Penelope.",
        .textoObjetivoConfronto =
            "Objetivo: derrote os pretendentes",
        .textoObjetivoSaida = "Objetivo: alcance Penelope ao norte"
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

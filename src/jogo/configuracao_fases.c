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
        { "Polifemo", TIPO_INIMIGO, 84, 19, 6, 8 },
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
        { "Sereia de Ligeia", TIPO_INIMIGO, 36, 16, 4, 15 },
        { -3.0f, 0.0f, -4.0f },
        {
            "sereia_primitiva",
            { 70, 105, 160, 255 },
            { 205, 175, 150, 255 },
            0.85f
        }
    },
    {
        { "Sereia de Leucosia", TIPO_INIMIGO, 42, 15, 5, 11 },
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
    { { 6.0f, 0.6f, 8.5f }, { 3.0f, 1.2f, 2.0f }, COR_MADEIRA_ESCURA, false }
};

static const ConfiguracaoItemFase ITENS_CILA[] = {
    { { -7.0f, 0.55f, 4.0f }, false },
    { { 0.0f, 0.55f, -1.0f }, false },
    { { 7.0f, 0.55f, 4.0f }, false }
};

static const ConfiguracaoInimigo INIMIGOS_CILA[] = {
    {
        { "Tentaculo de Cila I", TIPO_INIMIGO, 28, 14, 3, 14 },
        { -4.5f, 0.0f, -5.0f },
        {
            "tentaculo_primitivo",
            { 85, 115, 90, 255 },
            { 115, 145, 105, 255 },
            0.9f
        }
    },
    {
        { "Tentaculo de Cila II", TIPO_INIMIGO, 34, 14, 4, 10 },
        { 0.0f, 0.0f, -6.0f },
        {
            "tentaculo_primitivo",
            { 75, 105, 82, 255 },
            { 105, 135, 98, 255 },
            1.0f
        }
    },
    {
        { "Tentaculo de Cila III", TIPO_INIMIGO, 28, 13, 3, 7 },
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
        { "Antinoo", TIPO_INIMIGO, 45, 15, 5, 13 },
        { -3.0f, 0.0f, -6.0f },
        {
            "pretendente_primitivo",
            { 145, 70, 55, 255 },
            { 205, 165, 130, 255 },
            0.9f
        }
    },
    {
        { "Eurimaco", TIPO_INIMIGO, 52, 16, 5, 14 },
        { 3.0f, 0.0f, -6.0f },
        {
            "pretendente_primitivo",
            { 125, 65, 95, 255 },
            { 200, 160, 125, 255 },
            0.95f
        }
    }
};

/* ---- Enfeites sem colisao (so desenho) ---- */

#define COR_CHAMA { 255, 170, 60, 254 }  /* alfa 254: brilha, sem sombra */
#define COR_HALO { 255, 190, 90, 60 }    /* alfa baixo: translucido */
#define COR_LA { 232, 228, 214, 255 }
#define COR_ESCURO { 42, 36, 34, 255 }
#define COR_PELE { 208, 172, 142, 255 }
#define COR_PANO { 236, 226, 200, 255 }
#define COR_METAL_VELHO { 120, 118, 112, 255 }
#define COR_MAR { 38, 92, 140, 255 }
#define COR_MAR_ESCURO { 30, 62, 78, 255 }
#define COR_GRAMA { 92, 124, 72, 255 }
#define COR_GRAMA_ESCURA { 62, 92, 58, 255 }
#define COR_OLHO_CILA { 255, 60, 40, 254 }
#define COR_OURO { 205, 170, 70, 255 }
#define COR_TAPETE { 130, 30, 40, 255 }
#define COR_VESTIDO { 70, 100, 150, 255 }

#define CIL(x, y, z, rb, h, rt, cor) { 'c', { x, y, z }, { rb, h, rt }, cor }
#define ESF(x, y, z, rx, ry, rz, cor) { 'e', { x, y, z }, { rx, ry, rz }, cor }
#define CAI(x, y, z, lx, ly, lz, cor) { 'b', { x, y, z }, { lx, ly, lz }, cor }

static const ConfiguracaoOrnamento ORN_CAVERNA[] = {
    CAI(0.0f, 6.9f, 0.0f, 44.0f, 0.6f, 44.0f, COR_ROCHA_ESCURA), /* teto alto */
    CIL(-14.0f, 4.2f, -6.0f, 0.0f, 2.4f, 0.7f, COR_PEDRA_ESCURA), /* estalactites */
    CIL(12.0f, 4.0f, -14.0f, 0.0f, 2.6f, 0.8f, COR_PEDRA_ESCURA),
    CIL(-6.0f, 4.4f, 12.0f, 0.0f, 2.2f, 0.6f, COR_PEDRA_ESCURA),
    CIL(15.0f, 4.2f, 8.0f, 0.0f, 2.4f, 0.7f, COR_PEDRA_ESCURA),
    CIL(-17.5f, 0.0f, -12.0f, 0.8f, 2.2f, 0.0f, COR_PEDRA), /* estalagmites */
    CIL(17.5f, 0.0f, -4.0f, 0.9f, 2.6f, 0.0f, COR_PEDRA),
    CIL(-17.0f, 0.0f, 9.0f, 0.8f, 1.8f, 0.0f, COR_PEDRA),
    CIL(16.5f, 0.0f, 16.5f, 0.9f, 2.0f, 0.0f, COR_PEDRA),
    CIL(-19.7f, 0.0f, -8.0f, 0.09f, 1.5f, 0.07f, COR_MADEIRA_ESCURA), /* tochas */
    CIL(19.7f, 0.0f, -8.0f, 0.09f, 1.5f, 0.07f, COR_MADEIRA_ESCURA),
    CIL(-19.7f, 0.0f, 10.0f, 0.09f, 1.5f, 0.07f, COR_MADEIRA_ESCURA),
    CIL(19.7f, 0.0f, 10.0f, 0.09f, 1.5f, 0.07f, COR_MADEIRA_ESCURA),
    ESF(-19.7f, 1.7f, -8.0f, 0.22f, 0.3f, 0.22f, COR_CHAMA),
    ESF(19.7f, 1.7f, -8.0f, 0.22f, 0.3f, 0.22f, COR_CHAMA),
    ESF(-19.7f, 1.7f, 10.0f, 0.22f, 0.3f, 0.22f, COR_CHAMA),
    ESF(19.7f, 1.7f, 10.0f, 0.22f, 0.3f, 0.22f, COR_CHAMA),
    ESF(-13.0f, 0.6f, 12.0f, 0.55f, 0.42f, 0.7f, COR_LA), /* ovelhas */
    ESF(-13.0f, 0.8f, 12.75f, 0.22f, 0.22f, 0.26f, COR_ESCURO),
    CAI(-13.0f, 0.15f, 12.0f, 0.5f, 0.3f, 0.5f, COR_ESCURO),
    ESF(14.0f, 0.6f, -11.0f, 0.55f, 0.42f, 0.7f, COR_LA),
    ESF(14.0f, 0.8f, -10.25f, 0.22f, 0.22f, 0.26f, COR_ESCURO),
    CAI(14.0f, 0.15f, -11.0f, 0.5f, 0.3f, 0.5f, COR_ESCURO),
    CIL(-11.0f, 0.0f, -14.0f, 0.9f, 0.3f, 0.8f, COR_ROCHA), /* fogueira */
    ESF(-11.0f, 0.7f, -14.0f, 0.4f, 0.6f, 0.4f, COR_CHAMA),
    ESF(-19.7f, 1.7f, -8.0f, 1.1f, 1.1f, 1.1f, COR_HALO), /* brilho das chamas */
    ESF(19.7f, 1.7f, -8.0f, 1.1f, 1.1f, 1.1f, COR_HALO),
    ESF(-19.7f, 1.7f, 10.0f, 1.1f, 1.1f, 1.1f, COR_HALO),
    ESF(19.7f, 1.7f, 10.0f, 1.1f, 1.1f, 1.1f, COR_HALO),
    ESF(-11.0f, 0.7f, -14.0f, 1.8f, 1.8f, 1.8f, COR_HALO)
};

static const ConfiguracaoOrnamento ORN_SEREIAS[] = {
    CAI(0.0f, -0.6f, 0.0f, 400.0f, 0.2f, 400.0f, COR_MAR), /* mar */
    CAI(0.0f, 4.2f, 0.7f, 6.0f, 1.6f, 0.12f, COR_PANO), /* vela alta e verga */
    CAI(0.0f, 5.0f, 0.7f, 7.0f, 0.18f, 0.18f, COR_MADEIRA_ESCURA),
    CIL(-8.4f, 0.0f, 8.0f, 0.5f, 1.0f, 0.45f, COR_MADEIRA), /* barris */
    CIL(-8.4f, 0.0f, 6.7f, 0.5f, 1.0f, 0.45f, COR_MADEIRA),
    CIL(8.4f, 0.0f, 8.0f, 0.5f, 1.0f, 0.45f, COR_MADEIRA),
    CIL(8.4f, 0.0f, 6.7f, 0.5f, 1.0f, 0.45f, COR_MADEIRA),
    CIL(-8.6f, 0.0f, 10.5f, 0.07f, 2.4f, 0.05f, COR_MADEIRA_ESCURA), /* lanternas */
    CIL(8.6f, 0.0f, 10.5f, 0.07f, 2.4f, 0.05f, COR_MADEIRA_ESCURA),
    ESF(-8.6f, 2.55f, 10.5f, 0.2f, 0.26f, 0.2f, COR_CHAMA),
    ESF(8.6f, 2.55f, 10.5f, 0.2f, 0.26f, 0.2f, COR_CHAMA),
    CIL(-60.0f, -0.6f, -70.0f, 14.0f, 12.0f, 0.0f, COR_ROCHA), /* ilhas */
    CIL(55.0f, -0.6f, -60.0f, 10.0f, 9.0f, 0.0f, COR_ROCHA),
    CIL(-28.0f, -0.6f, -38.0f, 5.0f, 6.0f, 0.5f, COR_ROCHA_ESCURA),
    ESF(-8.6f, 2.55f, 10.5f, 0.9f, 0.9f, 0.9f, COR_HALO),
    ESF(8.6f, 2.55f, 10.5f, 0.9f, 0.9f, 0.9f, COR_HALO)
};

static const ConfiguracaoOrnamento ORN_CILA[] = {
    CAI(0.0f, -0.6f, 0.0f, 400.0f, 0.2f, 400.0f, COR_MAR_ESCURO), /* mar */
    CIL(-17.0f, -0.6f, -8.0f, 7.0f, 16.0f, 1.0f, COR_ROCHA_ESCURA), /* penhascos */
    CIL(17.0f, -0.6f, -6.0f, 7.0f, 18.0f, 1.0f, COR_ROCHA_ESCURA),
    CIL(-16.0f, -0.6f, 10.0f, 6.0f, 12.0f, 0.5f, COR_ROCHA),
    CIL(16.0f, -0.6f, 12.0f, 6.0f, 13.0f, 0.5f, COR_ROCHA),
    ESF(-24.0f, -0.4f, -26.0f, 5.0f, 4.0f, 5.0f, COR_ROCHA),
    CAI(0.0f, 4.0f, -30.0f, 16.0f, 9.0f, 3.0f, COR_ESCURO), /* antro de Cila */
    ESF(-2.5f, 5.0f, -28.4f, 0.55f, 0.35f, 0.2f, COR_OLHO_CILA),
    ESF(2.5f, 5.0f, -28.4f, 0.55f, 0.35f, 0.2f, COR_OLHO_CILA),
    CIL(-7.0f, 0.0f, 4.0f, 0.75f, 0.15f, 0.65f, COR_METAL_VELHO), /* bases dos mecanismos */
    CIL(0.0f, 0.0f, -1.0f, 0.75f, 0.15f, 0.65f, COR_METAL_VELHO),
    CIL(7.0f, 0.0f, 4.0f, 0.75f, 0.15f, 0.65f, COR_METAL_VELHO),
    CIL(-9.9f, 0.0f, 10.5f, 0.07f, 2.4f, 0.05f, COR_MADEIRA_ESCURA), /* lanternas */
    CIL(9.9f, 0.0f, 10.5f, 0.07f, 2.4f, 0.05f, COR_MADEIRA_ESCURA),
    ESF(-9.9f, 2.55f, 10.5f, 0.2f, 0.26f, 0.2f, COR_CHAMA),
    ESF(9.9f, 2.55f, 10.5f, 0.2f, 0.26f, 0.2f, COR_CHAMA)
};

static const ConfiguracaoOrnamento ORN_ITACA[] = {
    CAI(0.0f, -0.1f, 0.0f, 300.0f, 0.1f, 300.0f, COR_GRAMA), /* terra ao redor */
    CIL(-70.0f, 0.0f, -60.0f, 26.0f, 14.0f, 0.0f, COR_GRAMA_ESCURA), /* colinas */
    CIL(60.0f, 0.0f, -70.0f, 30.0f, 16.0f, 0.0f, COR_GRAMA_ESCURA),
    CAI(0.0f, 0.015f, -1.0f, 5.0f, 0.03f, 27.6f, COR_OURO), /* tapete */
    CAI(0.0f, 0.02f, -1.0f, 4.2f, 0.04f, 27.0f, COR_TAPETE),
    CAI(-8.0f, 0.15f, 2.0f, 2.0f, 0.3f, 2.0f, COR_MARMORE_ESCURO), /* bases */
    CAI(8.0f, 0.15f, 2.0f, 2.0f, 0.3f, 2.0f, COR_MARMORE_ESCURO),
    CAI(-8.0f, 0.15f, -7.0f, 2.0f, 0.3f, 2.0f, COR_MARMORE_ESCURO),
    CAI(8.0f, 0.15f, -7.0f, 2.0f, 0.3f, 2.0f, COR_MARMORE_ESCURO),
    CIL(-14.2f, 0.0f, 6.0f, 0.6f, 4.6f, 0.5f, COR_MARMORE), /* colunas */
    CIL(14.2f, 0.0f, 6.0f, 0.6f, 4.6f, 0.5f, COR_MARMORE),
    CIL(-14.2f, 0.0f, -6.0f, 0.6f, 4.6f, 0.5f, COR_MARMORE),
    CIL(14.2f, 0.0f, -6.0f, 0.6f, 4.6f, 0.5f, COR_MARMORE),
    CIL(-3.0f, 0.0f, -13.0f, 0.4f, 0.9f, 0.6f, COR_METAL_VELHO), /* braseiros */
    CIL(3.0f, 0.0f, -13.0f, 0.4f, 0.9f, 0.6f, COR_METAL_VELHO),
    ESF(-3.0f, 1.15f, -13.0f, 0.35f, 0.5f, 0.35f, COR_CHAMA),
    ESF(3.0f, 1.15f, -13.0f, 0.35f, 0.5f, 0.35f, COR_CHAMA),
    CIL(0.0f, 0.0f, -14.3f, 0.7f, 1.6f, 0.35f, COR_VESTIDO), /* Penelope */
    CIL(0.0f, 1.6f, -14.3f, 0.35f, 0.75f, 0.28f, COR_VESTIDO),
    ESF(0.0f, 2.55f, -14.3f, 0.2f, 0.23f, 0.2f, COR_PELE),
    ESF(0.0f, 2.6f, -14.45f, 0.22f, 0.26f, 0.2f, COR_ESCURO),
    ESF(-3.0f, 1.15f, -13.0f, 1.3f, 1.3f, 1.3f, COR_HALO),
    ESF(3.0f, 1.15f, -13.0f, 1.3f, 1.3f, 1.3f, COR_HALO)
};

/* Balanceamento (commit 26), conferido por tests/teste_balanceamento.c:
 * - Odisseu: 170 de vida, 20 de ataque, 10 de defesa e 12 de iniciativa em
 *   todas as fases; a vida e carregada de uma fase para a seguinte.
 * - Jogador que so ataca termina a jornada sem derrota, com pouca vida no
 *   final; cada encontro dura cerca de 7 rodadas.
 * - Dano sofrido cresce ate o confronto final, o mais perigoso.
 * - A velocidade muda a ordem: Odisseu age primeiro contra Polifemo, no
 *   meio contra Sereias e Cila e por ultimo contra os pretendentes. */
static const ConfiguracaoFase FASES[] = {
    {
        .identidade = IDENTIDADE_POLIFEMO,
        .id = "caverna_polifemo",
        .titulo = "Fase 1 - A Caverna de Polifemo",
        .instrucaoInicial = "Colete madeira sem chamar a atencao de Polifemo",
        .textoNarrativo =
            "Depois da Guerra de Troia, Odisseu enfrenta um longo caminho\n"
            "de volta a Itaca. Uma tempestade o leva a uma ilha desconhecida,\n"
            "onde a caverna do ciclope Polifemo bloqueia sua jornada.",
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
        .ornamentos = ORN_CAVERNA,
        .quantidadeOrnamentos = QUANTIDADE(ORN_CAVERNA),
        .corCeu = { 20, 16, 14, 255 },
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
        .heroi = { "Odisseu", TIPO_HEROI, 170, 20, 10, 12 },
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
        .identidade = IDENTIDADE_SEREIAS,
        .id = "canto_sereias",
        .titulo = "Fase 2 - O Canto das Sereias",
        .instrucaoInicial = "Proteja o conves antes do encantamento chegar ao limite",
        .textoNarrativo =
            "Livre da caverna, Odisseu retorna ao mar. O vento conduz o navio\n"
            "ate as Sereias, cujo canto enfeitica todos que se aproximam.\n"
            "O conves precisa ser protegido antes que seja tarde.",
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
        .ornamentos = ORN_SEREIAS,
        .quantidadeOrnamentos = QUANTIDADE(ORN_SEREIAS),
        .corCeu = { 155, 200, 235, 255 },
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
        .heroi = { "Odisseu", TIPO_HEROI, 170, 20, 10, 12 },
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
        .identidade = IDENTIDADE_CILA,
        .id = "estreito_cila",
        .titulo = "Fase 3 - O Estreito de Cila",
        .instrucaoInicial = "Reative os mecanismos entre os ataques de Cila",
        .textoNarrativo =
            "O canto ficou para tras, mas o navio entra em um estreito rochoso.\n"
            "Nas sombras, Cila aguarda com seus tentaculos. Os mecanismos\n"
            "do navio precisam funcionar para atravessar a passagem.",
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
        .ornamentos = ORN_CILA,
        .quantidadeOrnamentos = QUANTIDADE(ORN_CILA),
        .corCeu = { 92, 110, 120, 255 },
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
        .heroi = { "Odisseu", TIPO_HEROI, 170, 20, 10, 12 },
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
        .identidade = IDENTIDADE_ITACA,
        .id = "retorno_itaca",
        .titulo = "Fase 4 - O Retorno a Itaca",
        .instrucaoInicial = "Escolha uma rota pelo palacio e recupere o arco",
        .textoNarrativo =
            "Depois de anos no mar, Odisseu finalmente pisa em Itaca.\n"
            "Seu palacio foi tomado por pretendentes. Para recuperar o lar,\n"
            "ele precisa encontrar seu arco e atravessar o grande salao.",
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
        .ornamentos = ORN_ITACA,
        .quantidadeOrnamentos = QUANTIDADE(ORN_ITACA),
        .corCeu = { 160, 200, 235, 255 },
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
        .heroi = { "Odisseu", TIPO_HEROI, 170, 20, 10, 12 },
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

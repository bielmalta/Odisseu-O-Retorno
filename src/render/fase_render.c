#include "fase_render.h"

#include <stdio.h>

/* Texto com sombra para ler bem sobre qualquer fundo. */
static void desenharTexto(const char *texto, int x, int y, int tamanho, Color cor)
{
    DrawText(texto, x + 1, y + 1, tamanho, (Color){ 0, 0, 0, 200 });
    DrawText(texto, x, y, tamanho, cor);
}

static void desenharItens(const Fase *fase)
{
    const Vector3 tamanho = { 0.9f, 0.3f, 0.3f };

    for (int i = 0; i < fase->quantidadeItens; i++) {
        if (fase->itens[i].coletado) {
            continue;
        }
        const Vector3 p = fase->itens[i].posicao;
        const bool mirado = (i == fase->itemAlvo);

        DrawCubeV(p, tamanho, (Color){ 130, 85, 40, 255 });
        DrawCubeWiresV(p, tamanho, mirado ? YELLOW : (Color){ 60, 35, 15, 255 });
    }
}

/* Representacao provisoria de Polifemo (primitivas), no ponto do inimigo. */
static void desenharInimigos(const Fase *fase)
{
    if (fase->etapa != ETAPA_CONFRONTO) {
        return;
    }

    for (int i = 0; i < fase->quantidadeInimigos; i++) {
        const Vector3 base = fase->pontosInimigos[i];
        const Vector3 cabeca = { base.x, base.y + 4.2f, base.z };
        const Vector3 olho = { base.x, base.y + 4.3f, base.z + 0.75f };

        DrawCylinder(base, 1.0f, 1.5f, 3.6f, 16, (Color){ 120, 80, 70, 255 });
        DrawSphere(cabeca, 0.95f, (Color){ 150, 105, 90, 255 });
        DrawSphere(olho, 0.28f, WHITE);
        DrawSphere((Vector3){ olho.x, olho.y, olho.z + 0.18f }, 0.12f, BLACK);
    }
}

static void desenharMarcaDeSaida(const Fase *fase)
{
    if (fase->etapa != ETAPA_SAIDA_LIBERADA && fase->etapa != ETAPA_CONCLUIDA) {
        return;
    }

    const BoundingBox s = fase->saida;
    const Vector3 centro = { (s.min.x + s.max.x) / 2, 0.02f, (s.min.z + s.max.z) / 2 };
    DrawCubeV(centro, (Vector3){ s.max.x - s.min.x, 0.04f, s.max.z - s.min.z },
              (Color){ 80, 200, 120, 255 });
}

void desenharFase(const Fase *fase)
{
    desenharCenario(&fase->cenario);
    desenharMarcaDeSaida(fase);
    desenharItens(fase);
    desenharInimigos(fase);
}

static const char *textoObjetivoAtual(const Fase *fase, char *buffer, int tamanho)
{
    switch (fase->etapa) {
    case ETAPA_EXPLORACAO:
        snprintf(
            buffer, (size_t)tamanho, "Objetivo: %s (%d/%d)",
            fase->objetivo.descricao, fase->objetivo.progresso,
            fase->objetivo.necessario
        );
        return buffer;
    case ETAPA_CONFRONTO:
        return "Objetivo: derrote Polifemo";
    case ETAPA_SAIDA_LIBERADA:
        return "Objetivo: ache a saida da caverna, ao norte";
    default:
        return "Fase concluida!";
    }
}

void desenharHudFase(const Fase *fase)
{
    char buffer[FASE_TAMANHO_MENSAGEM + 64];
    const int largura = GetScreenWidth();
    const int altura = GetScreenHeight();
    const int centroX = largura / 2;
    const int centroY = altura / 2;

    /* Mira central: so na exploracao; no confronto o cursor e livre. */
    if (fase->etapa != ETAPA_CONFRONTO) {
        DrawLine(centroX - 8, centroY, centroX + 8, centroY, WHITE);
        DrawLine(centroX, centroY - 8, centroX, centroY + 8, WHITE);
    }

    /* Titulo e objetivo */
    desenharTexto(fase->titulo, 12, 36, 20, WHITE);
    desenharTexto(
        textoObjetivoAtual(fase, buffer, (int)sizeof buffer), 12, 62, 20, GOLD
    );

    /* Aviso de interacao: so com objeto valido no centro da mira */
    if (fase->itemAlvo >= 0) {
        const char *aviso = TextFormat("[E] Coletar %s", fase->objetivo.nomeItem);
        const int larguraAviso = MeasureText(aviso, 22);
        desenharTexto(aviso, centroX - larguraAviso / 2, centroY + 30, 22, YELLOW);
    }

    /* Mensagem temporaria */
    if (fase->tempoMensagem > 0.0f && fase->mensagem[0] != '\0') {
        const int larguraMsg = MeasureText(fase->mensagem, 22);
        desenharTexto(fase->mensagem, centroX - larguraMsg / 2, altura - 90, 22, WHITE);
    }
}

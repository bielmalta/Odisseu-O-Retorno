#include "telas_jogo.h"

enum {
    LARGURA_BOTAO_TELA = 320,
    ALTURA_BOTAO_TELA = 54,
    ESPACO_BOTAO_TELA = 14
};

static Rectangle botaoCentral(
    int largura,
    int altura,
    int indice,
    int quantidade
)
{
    const float alturaTotal =
        (float)(quantidade * ALTURA_BOTAO_TELA +
                (quantidade - 1) * ESPACO_BOTAO_TELA);
    const float topo = (float)altura / 2.0f + 30.0f - alturaTotal / 2.0f;

    return (Rectangle){
        ((float)largura - LARGURA_BOTAO_TELA) / 2.0f,
        topo + indice * (ALTURA_BOTAO_TELA + ESPACO_BOTAO_TELA),
        LARGURA_BOTAO_TELA,
        ALTURA_BOTAO_TELA
    };
}

static bool pontoNoRetangulo(Vector2 ponto, Rectangle area)
{
    return ponto.x >= area.x && ponto.x <= area.x + area.width &&
           ponto.y >= area.y && ponto.y <= area.y + area.height;
}

static bool clicou(
    EntradaTelaJogo entrada,
    Rectangle area
)
{
    return entrada.clique && pontoNoRetangulo(entrada.mouse, area);
}

EntradaTelaJogo lerEntradaTelaJogo(void)
{
    EntradaTelaJogo entrada = {0};

    entrada.confirmar = IsKeyPressed(KEY_ENTER);
    entrada.cancelar = IsKeyPressed(KEY_ESCAPE);
    entrada.reiniciar = IsKeyPressed(KEY_R);
    entrada.menu = IsKeyPressed(KEY_M);
    entrada.sair = IsKeyPressed(KEY_S);
    entrada.clique = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
    entrada.mouse = GetMousePosition();

    return entrada;
}

AcaoTelaJogo interpretarEntradaMenu(
    EntradaTelaJogo entrada,
    int largura,
    int altura
)
{
    const Rectangle jogar = botaoCentral(largura, altura, 0, 2);
    const Rectangle sair = botaoCentral(largura, altura, 1, 2);

    if (entrada.confirmar || clicou(entrada, jogar)) {
        return ACAO_TELA_JOGAR;
    }
    if (entrada.cancelar || entrada.sair || clicou(entrada, sair)) {
        return ACAO_TELA_SAIR;
    }
    return ACAO_TELA_NENHUMA;
}

AcaoTelaJogo interpretarEntradaPausa(
    EntradaTelaJogo entrada,
    bool combateEmAndamento,
    int largura,
    int altura
)
{
    const int quantidade = combateEmAndamento ? 3 : 2;
    const Rectangle retomar = botaoCentral(largura, altura, 0, quantidade);
    const Rectangle reiniciar = botaoCentral(largura, altura, 1, quantidade);
    const Rectangle menu = botaoCentral(
        largura, altura, combateEmAndamento ? 2 : 1, quantidade
    );

    if (entrada.cancelar || entrada.confirmar || clicou(entrada, retomar)) {
        return ACAO_TELA_RETOMAR;
    }
    if (combateEmAndamento &&
        (entrada.reiniciar || clicou(entrada, reiniciar))) {
        return ACAO_TELA_REINICIAR;
    }
    if (entrada.menu || clicou(entrada, menu)) {
        return ACAO_TELA_MENU;
    }
    return ACAO_TELA_NENHUMA;
}

static void textoCentralizado(
    const char *texto,
    int centroX,
    int y,
    int tamanho,
    Color cor
)
{
    DrawText(texto, centroX - MeasureText(texto, tamanho) / 2, y, tamanho, cor);
}

static void desenharBotaoTela(Rectangle area, const char *texto, Color cor)
{
    const bool sobre = pontoNoRetangulo(GetMousePosition(), area);
    const Color fundo = sobre ? ColorBrightness(cor, 0.18f) : cor;
    const int tamanho = 22;
    const int x = (int)(area.x +
                        (area.width - MeasureText(texto, tamanho)) / 2.0f);
    const int y = (int)(area.y + (area.height - tamanho) / 2.0f);

    DrawRectangleRec(area, fundo);
    DrawRectangleLinesEx(area, 2.0f, sobre ? GOLD : RAYWHITE);
    DrawText(texto, x, y, tamanho, WHITE);
}

void desenharTelaMenu(int largura, int altura)
{
    const int centroX = largura / 2;
    const Rectangle jogar = botaoCentral(largura, altura, 0, 2);
    const Rectangle sair = botaoCentral(largura, altura, 1, 2);

    ClearBackground((Color){ 10, 20, 35, 255 });
    DrawRectangleGradientV(
        0, 0, largura, altura,
        (Color){ 18, 45, 68, 255 },
        (Color){ 8, 12, 24, 255 }
    );

    textoCentralizado(
        "ODISSEU", centroX, altura / 2 - 190, 72,
        (Color){ 226, 190, 104, 255 }
    );
    textoCentralizado(
        "O ULTIMO RETORNO", centroX, altura / 2 - 105, 30, RAYWHITE
    );
    textoCentralizado(
        "Uma jornada em primeira pessoa ate Itaca",
        centroX, altura / 2 - 58, 20, LIGHTGRAY
    );

    desenharBotaoTela(jogar, "[ENTER] Jogar", (Color){ 55, 125, 80, 255 });
    desenharBotaoTela(sair, "[S] Sair", (Color){ 120, 55, 55, 255 });
}

void desenharTelaPausa(int largura, int altura, bool combateEmAndamento)
{
    const int quantidade = combateEmAndamento ? 3 : 2;
    const Rectangle retomar = botaoCentral(largura, altura, 0, quantidade);
    const Rectangle reiniciar = botaoCentral(largura, altura, 1, quantidade);
    const Rectangle menu = botaoCentral(
        largura, altura, combateEmAndamento ? 2 : 1, quantidade
    );

    DrawRectangle(0, 0, largura, altura, (Color){ 0, 0, 0, 185 });
    textoCentralizado("JOGO PAUSADO", largura / 2, altura / 2 - 155, 48, GOLD);

    desenharBotaoTela(
        retomar, "[ESC/ENTER] Retomar", (Color){ 50, 110, 160, 255 }
    );
    if (combateEmAndamento) {
        desenharBotaoTela(
            reiniciar, "[R] Reiniciar combate", (Color){ 170, 105, 45, 255 }
        );
    }
    desenharBotaoTela(menu, "[M] Menu principal", (Color){ 120, 55, 55, 255 });
}

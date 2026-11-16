#include "tela_narrativa.h"

#include <stddef.h>
#include <string.h>

#include "raylib.h"

static void desenharLinhaCentralizada(
    const char *texto,
    int centroX,
    int y,
    int tamanho,
    Color cor
)
{
    DrawText(
        texto,
        centroX - MeasureText(texto, tamanho) / 2,
        y,
        tamanho,
        cor
    );
}

static int desenharTextoMultilinha(
    const char *texto,
    int centroX,
    int y,
    int tamanho,
    int espacamento,
    Color cor
)
{
    const char *inicio = texto;
    char linha[256];
    int linhaAtual = 0;

    while (inicio != NULL && *inicio != '\0') {
        const char *fim = strchr(inicio, '\n');
        size_t comprimento = fim != NULL
                                 ? (size_t)(fim - inicio)
                                 : strlen(inicio);

        if (comprimento >= sizeof linha) {
            comprimento = sizeof linha - 1;
        }
        memcpy(linha, inicio, comprimento);
        linha[comprimento] = '\0';

        desenharLinhaCentralizada(
            linha,
            centroX,
            y + linhaAtual * (tamanho + espacamento),
            tamanho,
            cor
        );
        linhaAtual++;
        inicio = fim != NULL ? fim + 1 : NULL;
    }

    return y + linhaAtual * (tamanho + espacamento);
}

static void desenharFundo(const Narrativa *narrativa, int largura, int altura)
{
    Color topo = { 18, 43, 65, 255 };
    Color base = { 8, 14, 28, 255 };

    if (narrativa->tipo == NARRATIVA_TRANSICAO) {
        topo = (Color){ 35, 65, 88, 255 };
        base = (Color){ 12, 25, 45, 255 };
    } else if (narrativa->tipo == NARRATIVA_ENCERRAMENTO) {
        topo = (Color){ 92, 63, 45, 255 };
        base = (Color){ 24, 25, 38, 255 };
    }

    DrawRectangleGradientV(0, 0, largura, altura, topo, base);
    DrawCircle(largura - 120, 100, 48.0f, Fade(GOLD, 0.45f));
    DrawRectangle(
        0,
        altura - 115,
        largura,
        115,
        (Color){ 9, 30, 52, 220 }
    );
    DrawTriangle(
        (Vector2){ largura * 0.15f, (float)altura - 115.0f },
        (Vector2){ largura * 0.35f, (float)altura - 210.0f },
        (Vector2){ largura * 0.48f, (float)altura - 115.0f },
        (Color){ 20, 32, 42, 255 }
    );
}

void desenharTelaNarrativa(
    const Narrativa *narrativa,
    int largura,
    int altura
)
{
    const int centroX = largura / 2;
    int proximaLinha;

    if (!narrativaValida(narrativa)) {
        return;
    }

    desenharFundo(narrativa, largura, altura);
    desenharLinhaCentralizada(
        narrativa->titulo,
        centroX,
        altura / 2 - 210,
        42,
        GOLD
    );

    proximaLinha = desenharTextoMultilinha(
        narrativa->texto,
        centroX,
        altura / 2 - 130,
        22,
        8,
        RAYWHITE
    );

    if (narrativa->objetivo[0] != '\0') {
        const char *objetivo = narrativa->tipo == NARRATIVA_ENCERRAMENTO
                                   ? narrativa->objetivo
                                   : TextFormat(
                                         "Proximo objetivo: %s",
                                         narrativa->objetivo
                                     );
        desenharLinhaCentralizada(
            objetivo,
            centroX,
            proximaLinha + 16,
            22,
            (Color){ 240, 205, 105, 255 }
        );
    }

    if (narrativa->mostrarControles) {
        desenharLinhaCentralizada(
            "WASD mover | Mouse olhar | E interagir | ESC pausar",
            centroX,
            altura - 92,
            18,
            LIGHTGRAY
        );
    }

    desenharLinhaCentralizada(
        narrativa->tipo == NARRATIVA_ENCERRAMENTO
            ? "ENTER ou ESC para voltar ao menu"
            : "ENTER ou ESPACO para continuar",
        centroX,
        altura - 52,
        20,
        WHITE
    );
}

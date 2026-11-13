#include "raylib.h"

enum {
    LARGURA_INICIAL = 1280,
    ALTURA_INICIAL = 720,
    FPS_ALVO = 60
};

static void inicializarJogo(void)
{
    SetTargetFPS(FPS_ALVO);
}

static void atualizarJogo(float deltaTempo)
{
    (void)deltaTempo;
}

static void desenharJogo(void)
{
    const char *titulo = "Odisseu: O Ultimo Retorno";
    const char *estado = "Projeto C11 + Raylib inicializado";
    const int tamanhoTitulo = 40;
    const int tamanhoEstado = 20;
    const int larguraTitulo = MeasureText(titulo, tamanhoTitulo);
    const int larguraEstado = MeasureText(estado, tamanhoEstado);

    ClearBackground((Color){ 12, 20, 32, 255 });
    DrawText(
        titulo,
        (GetScreenWidth() - larguraTitulo) / 2,
        GetScreenHeight() / 2 - 45,
        tamanhoTitulo,
        (Color){ 226, 190, 104, 255 }
    );
    DrawText(
        estado,
        (GetScreenWidth() - larguraEstado) / 2,
        GetScreenHeight() / 2 + 20,
        tamanhoEstado,
        RAYWHITE
    );
    DrawFPS(12, 12);
}

static void finalizarJogo(void)
{
    /* Os recursos carregados pelos proximos commits serao liberados aqui. */
}

int main(void)
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(
        LARGURA_INICIAL,
        ALTURA_INICIAL,
        "Odisseu: O Ultimo Retorno"
    );

    inicializarJogo();

    while (!WindowShouldClose()) {
        atualizarJogo(GetFrameTime());

        BeginDrawing();
        desenharJogo();
        EndDrawing();
    }

    finalizarJogo();
    CloseWindow();

    return 0;
}


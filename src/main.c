#include "raylib.h"

#include "cenario.h"
#include "estado_jogo.h"
#include "jogador.h"

enum {
    LARGURA_INICIAL = 1280,
    ALTURA_INICIAL = 720,
    FPS_ALVO = 60
};

static Jogador jogador;
static Cenario cenario;
static EstadoJogo estadoAtual = ESTADO_EXPLORACAO;

/* Unico ponto que liga o estado do jogo ao controle do jogador:
 * so a exploracao libera movimento e giro da camera. */
static void aplicarEstado(void)
{
    definirControleJogador(&jogador, estadoAtual == ESTADO_EXPLORACAO);
}

static void definirEstado(EstadoJogo novoEstado)
{
    estadoAtual = novoEstado;
    aplicarEstado();
}

static void inicializarJogo(void)
{
    SetTargetFPS(FPS_ALVO);
    /* ESC pausa/retoma (README); a janela fecha pelo botao X. */
    SetExitKey(KEY_NULL);

    criarCenarioTeste(&cenario);
    inicializarJogador(&jogador, (Vector3){ 0.0f, 0.0f, 14.0f }, 0.0f);
    definirEstado(ESTADO_EXPLORACAO);
}

static void atualizarJogo(float deltaTempo)
{
    if (IsKeyPressed(KEY_ESCAPE) && estadoAtual != ESTADO_COMBATE) {
        definirEstado(
            estadoAtual == ESTADO_PAUSA ? ESTADO_EXPLORACAO : ESTADO_PAUSA
        );
    }

    /* Tecla de depuracao ate o combate existir (commit 12). */
    if (IsKeyPressed(KEY_F2) && estadoAtual != ESTADO_PAUSA) {
        definirEstado(
            estadoAtual == ESTADO_COMBATE ? ESTADO_EXPLORACAO : ESTADO_COMBATE
        );
    }

    atualizarJogador(
        &jogador,
        lerComandosJogador(),
        cenario.caixas,
        cenario.quantidade,
        deltaTempo
    );
}

static void desenharJogo(void)
{
    const char *nomeEstado = "EXPLORACAO";

    if (estadoAtual == ESTADO_COMBATE) {
        nomeEstado = "COMBATE (movimento bloqueado)";
    } else if (estadoAtual == ESTADO_PAUSA) {
        nomeEstado = "PAUSA";
    }

    ClearBackground((Color){ 135, 180, 220, 255 });

    BeginMode3D(jogador.camera);
    desenharCenario(&cenario);
    EndMode3D();

    const int centroX = GetScreenWidth() / 2;
    const int centroY = GetScreenHeight() / 2;
    DrawLine(centroX - 8, centroY, centroX + 8, centroY, WHITE);
    DrawLine(centroX, centroY - 8, centroX, centroY + 8, WHITE);

    DrawText(TextFormat("Estado: %s", nomeEstado), 12, 36, 20, BLACK);
    DrawText(
        "WASD mover | Mouse olhar | Shift correr | Espaco pular",
        12, 62, 18, DARKGRAY
    );
    DrawText("ESC pausa | F2 alterna combate (teste)", 12, 84, 18, DARKGRAY);
    DrawFPS(12, 12);
}

static void finalizarJogo(void)
{
    definirControleJogador(&jogador, false); /* devolve o cursor ao sistema */
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

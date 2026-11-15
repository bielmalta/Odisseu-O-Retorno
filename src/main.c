#include "raylib.h"

#include "estado_jogo.h"
#include "fase.h"
#include "fase_render.h"
#include "jogador.h"

enum {
    LARGURA_INICIAL = 1280,
    ALTURA_INICIAL = 720,
    FPS_ALVO = 60
};

static Jogador jogador;
static Fase fase;
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

/* O objetivo foi cumprido: coloca Odisseu de frente para o inimigo e
 * entra em combate (movimento bloqueado). A transicao suave da camera
 * fica para o commit 19. */
static void iniciarConfronto(void)
{
    Vector3 alvo = fase.pontosInimigos[0];
    alvo.y += 2.5f;

    posicionarJogadorOlhando(&jogador, fase.pontoJogadorConfronto, alvo);
    definirEstado(ESTADO_COMBATE);
}

static void inicializarJogo(void)
{
    SetTargetFPS(FPS_ALVO);
    /* ESC pausa/retoma (README); a janela fecha pelo botao X. */
    SetExitKey(KEY_NULL);

    criarFaseBase(&fase);
    inicializarJogador(&jogador, fase.pontoInicial, 0.0f);
    definirEstado(ESTADO_EXPLORACAO);
}

static void atualizarJogo(float deltaTempo)
{
    if (IsKeyPressed(KEY_ESCAPE) && estadoAtual != ESTADO_COMBATE) {
        definirEstado(
            estadoAtual == ESTADO_PAUSA ? ESTADO_EXPLORACAO : ESTADO_PAUSA
        );
    }

    /* Tecla de teste ate o combate existir (commit 12): encerra o confronto. */
    if (IsKeyPressed(KEY_F2) && estadoAtual == ESTADO_COMBATE) {
        encerrarConfronto(&fase);
        definirEstado(ESTADO_EXPLORACAO);
    }

    if (estadoAtual == ESTADO_EXPLORACAO && IsKeyPressed(KEY_E)) {
        interagir(&fase);
    }

    atualizarJogador(
        &jogador,
        lerComandosJogador(),
        fase.cenario.caixas,
        fase.cenario.quantidade,
        deltaTempo
    );

    /* A mira so vale enquanto o jogador pode explorar. */
    if (estadoAtual == ESTADO_EXPLORACAO) {
        const Ray raio = {
            jogador.camera.position, direcaoOlharJogador(&jogador)
        };
        atualizarMiraFase(&fase, raio);
    } else {
        fase.itemAlvo = -1; /* sem exploracao nao ha objeto mirado */
    }

    atualizarFase(&fase, jogador.posicao, deltaTempo);

    if (fase.etapa == ETAPA_CONFRONTO && estadoAtual == ESTADO_EXPLORACAO) {
        iniciarConfronto();
    }
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
    desenharFase(&fase);
    EndMode3D();

    desenharHudFase(&fase);

    DrawText(TextFormat("Estado: %s", nomeEstado), 12, 92, 18, BLACK);
    DrawText(
        "WASD mover | Mouse olhar | Shift correr | Espaco pular | E interagir",
        12, GetScreenHeight() - 50, 18, BLACK
    );
    DrawText(
        "ESC pausa | F2 encerra o combate (teste)",
        12, GetScreenHeight() - 28, 18, BLACK
    );
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

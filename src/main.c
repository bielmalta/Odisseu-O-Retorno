#include <stddef.h>

#include "raylib.h"

#include "estado_jogo.h"
#include "fase.h"
#include "fase_render.h"
#include "jogador.h"
#include "acoes_combate.h"
#include "combate.h"
#include "hud_combate.h"
#include "ia_inimigo.h"
#include "painel_combate.h"
#include "personagem.h"

enum {
    LARGURA_INICIAL = 1280,
    ALTURA_INICIAL = 720,
    FPS_ALVO = 60
};

static Jogador jogador;
static Fase fase;
static EstadoJogo estadoAtual = ESTADO_EXPLORACAO;


/* Controlador de combate e participantes do encontro inicial. */
static Combate combate;
static EsperaTurnoInimigo esperaInimigo;
static PainelCombate painel; /* dados mostrados pelo HUD de combate */

static Personagem odisseu;
static Personagem polifemo;

static Personagem *inimigosCombate[1];

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

    /* Prepara a lista circular com os participantes. */
    if (!prepararCombate(
            &combate,
            &odisseu,
            inimigosCombate,
            1)) {
        TraceLog(LOG_ERROR, "Falha ao preparar combate.");
        return;
    }

    /* O HUD le os inimigos agora, antes de a lista ser ordenada. */
    iniciarPainelCombate(&painel, &combate);
    iniciarEsperaTurnoInimigo(&esperaInimigo);

    /* Posiciona a camera diante do inimigo. */
    posicionarJogadorOlhando(
        &jogador,
        fase.pontoJogadorConfronto,
        alvo
    );

    definirEstado(ESTADO_COMBATE);

    TraceLog(LOG_INFO, "Combate contra Polifemo iniciado.");
}

/* Cria os participantes do primeiro encontro, com a vida cheia. */
static void criarParticipantes(void)
{
    odisseu = criarPersonagem(
        "Odisseu", TIPO_HEROI, 100, 20, 10, 12
    );

    polifemo = criarPersonagem(
        "Polifemo", TIPO_INIMIGO, 120, 25, 8, 8
    );

    inimigosCombate[0] = &polifemo;
}

/* Vitoria confirmada: libera a saida e volta a explorar. */
static void concluirConfronto(void)
{
    finalizarCombate(&combate);
    encerrarConfronto(&fase);
    definirEstado(ESTADO_EXPLORACAO);
}

/* Derrota confirmada: refaz o mesmo encontro, com a vida cheia. */
static void reiniciarConfronto(void)
{
    finalizarCombate(&combate);
    criarParticipantes();
    iniciarConfronto();
}

static void inicializarJogo(void)
{
    SetTargetFPS(FPS_ALVO);
    SetExitKey(KEY_NULL);

    criarFaseBase(&fase);
    inicializarJogador(&jogador, fase.pontoInicial, 0.0f);

    /* Inicializa o controlador de combate. */
    inicializarCombate(&combate);
    iniciarEsperaTurnoInimigo(&esperaInimigo);
    criarParticipantes();

    /* O HUD recebe cada ataque e defesa como mensagem. */
    registrarObservadorCombate(observarEventoPainel, &painel);

    definirEstado(ESTADO_EXPLORACAO);
}

/* Um quadro de combate: turnos, entrada do jogador e resultado. */
static void atualizarConfronto(float deltaTempo)
{
    /* No turno do inimigo, espera um instante para a acao ficar visivel. */
    if (aguardarTurnoInimigo(&esperaInimigo, &combate, deltaTempo)) {
        atualizarCombate(&combate);
    }

    atualizarAlvoPainel(&painel);

    switch (interpretarEntradaCombate(
        &painel, &combate, lerEntradaCombate(),
        GetScreenWidth(), GetScreenHeight()
    )) {
    case COMANDO_ATACAR:
        atacarNoCombate(&combate, painel.alvo);
        break;
    case COMANDO_DEFENDER:
        defenderNoCombate(&combate);
        break;
    case COMANDO_CONTINUAR:
        if (combate.estado == COMBATE_VICTORY) {
            concluirConfronto();
        } else {
            reiniciarConfronto();
        }
        break;
    case COMANDO_NENHUM:
        break;
    }
}


static void atualizarJogo(float deltaTempo)
{
    if (IsKeyPressed(KEY_ESCAPE) && estadoAtual != ESTADO_COMBATE) {
        definirEstado(
            estadoAtual == ESTADO_PAUSA ? ESTADO_EXPLORACAO : ESTADO_PAUSA
        );
    }

    if (estadoAtual == ESTADO_COMBATE) {
        atualizarConfronto(deltaTempo);
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
        nomeEstado = "COMBATE";
    } else if (estadoAtual == ESTADO_PAUSA) {
        nomeEstado = "PAUSA";
    }

    ClearBackground((Color){ 135, 180, 220, 255 });

    BeginMode3D(jogador.camera);
    desenharFase(&fase);
    EndMode3D();

    desenharHudFase(&fase);

    DrawText(TextFormat("Estado: %s", nomeEstado), 12, 92, 18, BLACK);

    if (estadoAtual == ESTADO_COMBATE) {
        desenharHudCombate(&painel, &combate, &fase, jogador.camera);
    } else {
        DrawText(
            "WASD mover | Mouse olhar | Shift correr | Espaco pular | E interagir",
            12, GetScreenHeight() - 50, 18, BLACK
        );
        DrawText("ESC pausa e retoma", 12, GetScreenHeight() - 28, 18, BLACK);
    }

    DrawFPS(12, 12);
}


static void finalizarJogo(void)
{
    definirControleJogador(&jogador, false);
    registrarObservadorCombate(NULL, NULL);

    /* Libera os nós da lista circular ao fechar o jogo. */
    finalizarCombate(&combate);
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

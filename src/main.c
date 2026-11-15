#include <stdbool.h>
#include <stddef.h>

#include "raylib.h"

#include "estado_jogo.h"
#include "fase.h"
#include "fase_render.h"
#include "jogador.h"
#include "acoes_combate.h"
#include "animacao_combate.h"
#include "audio_combate.h"
#include "combate.h"
#include "efeitos_combate.h"
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
static AnimacaoCombate animacao; /* tempo das acoes vistas na tela */
static EfeitosSonoros sons;

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

/* O objetivo foi cumprido: entra em combate (movimento bloqueado) e a camera
 * vai suavemente ate o foco, de frente para o inimigo. */
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
    iniciarAnimacaoCombate(&animacao);

    /* Da camera livre ao foco diante do inimigo, sem corte. */
    iniciarTransicaoJogador(
        &jogador,
        fase.pontoJogadorConfronto,
        alvo,
        JOGADOR_TRANSICAO_COMBATE
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

    /* Cada ataque e defesa vira uma animacao; sons e mensagens saem nos
     * marcos dela. O dano ja foi resolvido pela logica nesse instante. */
    iniciarAnimacaoCombate(&animacao);
    registrarObservadorCombate(observarEventoAnimacao, &animacao);
    iniciarAudioCombate(&sons);

    definirEstado(ESTADO_EXPLORACAO);
}

/* Avanca a animacao e reage aos marcos: toca o som e, no impacto ou na
 * defesa, publica a mensagem da acao no mesmo instante. */
static void processarMarcosDaAnimacao(float deltaTempo)
{
    CueAnimacao cues[8];
    const int quantidade = atualizarAnimacaoCombate(
        &animacao, deltaTempo, cues, 8
    );

    for (int i = 0; i < quantidade; i++) {
        tocarCue(&sons, &cues[i]);

        if (cues[i].tipo == CUE_IMPACTO || cues[i].tipo == CUE_DEFESA_ATIVADA) {
            observarEventoPainel(&cues[i].evento, &painel);
        }
    }
}

/* Um quadro de combate: animacao, turnos, entrada do jogador e resultado. */
static void atualizarConfronto(float deltaTempo)
{
    bool animando;

    processarMarcosDaAnimacao(deltaTempo);

    /* A camera ainda vai para o foco: o combate so comeca quando ela chega. */
    if (transicaoJogadorAtiva(&jogador)) {
        return;
    }

    animando = animacaoCombateOcupada(&animacao);

    /* O turno seguinte espera a acao terminar de aparecer na tela. O dano ja
     * foi resolvido; a animacao e curta, entao os turnos nunca travam. No
     * turno do inimigo ainda ha uma pausa antes de ele agir. */
    if (!animando && aguardarTurnoInimigo(&esperaInimigo, &combate, deltaTempo)) {
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
        if (animando) {
            break; /* o golpe final ainda esta acontecendo */
        }
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
    const bool combatendo = estadoAtual == ESTADO_COMBATE;
    PoseJogador poseJogador = {0};
    PoseInimigo posesInimigos[FASE_MAX_INIMIGOS] = {0};
    Camera3D camera = jogador.camera;

    if (combatendo) {
        nomeEstado = "COMBATE";
        poseJogador = poseDoJogador(&animacao, &odisseu);
        for (int i = 0; i < fase.quantidadeInimigos &&
                        i < painel.quantidadeInimigos; i++) {
            posesInimigos[i] = poseDoInimigo(&animacao, painel.inimigos[i]);
        }
        camera = camaraComEfeitos(jogador.camera, &poseJogador, (float)GetTime());
    } else if (estadoAtual == ESTADO_PAUSA) {
        nomeEstado = "PAUSA";
    }

    ClearBackground((Color){ 135, 180, 220, 255 });

    BeginMode3D(camera);
    desenharFase(&fase, combatendo ? posesInimigos : NULL);
    if (combatendo) {
        desenharArmasJogador(camera, &poseJogador);
    }
    EndMode3D();

    desenharHudFase(&fase);

    DrawText(TextFormat("Estado: %s", nomeEstado), 12, 92, 18, BLACK);

    if (combatendo) {
        desenharEfeitosTela(&poseJogador, GetScreenWidth(), GetScreenHeight());

        /* O HUD de combate aparece quando a camera chega ao foco. */
        if (!transicaoJogadorAtiva(&jogador)) {
            desenharHudCombate(&painel, &combate, &fase, camera, &animacao);
        }
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
    encerrarAudioCombate(&sons);
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
        /* Um quadro muito longo (janela arrastada) nao pode pular animacoes. */
        float deltaTempo = GetFrameTime();

        if (deltaTempo > 0.1f) {
            deltaTempo = 0.1f;
        }
        atualizarJogo(deltaTempo);

        BeginDrawing();
        desenharJogo();
        EndDrawing();
    }

    finalizarJogo();
    CloseWindow();

    return 0;
}

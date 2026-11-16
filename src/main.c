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
#include "jornada.h"
#include "narrativa.h"
#include "painel_combate.h"
#include "personagem.h"
#include "tela_narrativa.h"
#include "telas_jogo.h"

enum {
    LARGURA_INICIAL = 1280,
    ALTURA_INICIAL = 720,
    FPS_ALVO = 60
};

static Jogador jogador;
static Fase fase;
static Jornada jornada;
static Narrativa narrativa;
static EstadoJogo estadoAtual = ESTADO_MENU;
static EstadoJogo estadoAntesDaPausa = ESTADO_EXPLORACAO;
static bool sairSolicitado;


/* Controlador de combate e participantes do encontro inicial. */
static Combate combate;
static EsperaTurnoInimigo esperaInimigo;
static PainelCombate painel; /* dados mostrados pelo HUD de combate */
static AnimacaoCombate animacao; /* tempo das acoes vistas na tela */
static EfeitosSonoros sons;

static Personagem inimigosFase[FASE_MAX_INIMIGOS];
static Personagem *inimigosCombate[FASE_MAX_INIMIGOS];

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

    if (fase.configuracao == NULL || fase.quantidadeInimigos <= 0) {
        TraceLog(LOG_ERROR, "Fase sem encontro configurado.");
        return;
    }

    /* Prepara a lista circular com os participantes. */
    if (!prepararCombate(
            &combate,
            &jornada.odisseu,
            inimigosCombate,
            fase.quantidadeInimigos)) {
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

    TraceLog(
        LOG_INFO,
        "%s iniciado.",
        fase.configuracao->encontro.nome
    );
}

/* Cria os inimigos da etapa sem reconstruir o Odisseu persistente. */
static bool criarInimigosDaFase(void)
{
    const ConfiguracaoFase *configuracao = fase.configuracao;

    if (configuracao == NULL ||
        configuracao->encontro.quantidadeInimigos <= 0 ||
        configuracao->encontro.quantidadeInimigos > FASE_MAX_INIMIGOS) {
        return false;
    }

    for (int i = 0; i < configuracao->encontro.quantidadeInimigos; i++) {
        inimigosFase[i] = criarPersonagemConfigurada(
            &configuracao->encontro.inimigos[i].personagem
        );
        inimigosCombate[i] = &inimigosFase[i];
    }
    return true;
}

/* Vitoria confirmada: libera a saida e volta a explorar. */
static void concluirConfronto(void)
{
    finalizarCombate(&combate);
    registrarVitoriaJornada(&jornada);
    encerrarConfronto(&fase);
    definirEstado(ESTADO_EXPLORACAO);
}

/* Derrota confirmada: refaz o mesmo encontro, com a vida cheia. */
static void reiniciarConfronto(void)
{
    finalizarCombate(&combate);
    jornada.odisseu.vida = jornada.odisseu.vidaMaxima;
    jornada.odisseu.bonusDefesa = 0;
    if (criarInimigosDaFase()) {
        iniciarConfronto();
    }
}

/* Reconstroi toda a fase e garante que nenhuma lista ou animacao da partida
 * anterior sobreviva ao reinicio ou ao retorno para o menu. */
static void prepararNovaPartida(void)
{
    definirControleJogador(&jogador, false);
    finalizarCombate(&combate);
    if (!iniciarJornada(&jornada, &fase)) {
        TraceLog(LOG_ERROR, "Falha ao iniciar a jornada.");
        return;
    }
    inicializarJogador(&jogador, fase.pontoInicial, 0.0f);
    iniciarEsperaTurnoInimigo(&esperaInimigo);
    iniciarAnimacaoCombate(&animacao);
    if (!criarInimigosDaFase()) {
        TraceLog(LOG_ERROR, "Falha ao criar participantes configurados.");
    }
    estadoAntesDaPausa = ESTADO_EXPLORACAO;
}

static void iniciarNovaPartida(void)
{
    prepararNovaPartida();
    if (fase.configuracao == NULL) {
        definirEstado(ESTADO_MENU);
        return;
    }
    narrativa = criarNarrativaIntroducao(
        fase.titulo,
        fase.configuracao->textoNarrativo,
        fase.objetivo.descricao
    );
    definirEstado(ESTADO_NARRATIVA);
}

static void voltarAoMenu(void)
{
    prepararNovaPartida();
    definirEstado(ESTADO_MENU);
}

static void pausarJogo(void)
{
    if (estadoAtual != ESTADO_EXPLORACAO &&
        estadoAtual != ESTADO_COMBATE) {
        return;
    }

    estadoAntesDaPausa = estadoAtual;
    definirEstado(ESTADO_PAUSA);
}

static void retomarJogo(void)
{
    if (estadoAtual == ESTADO_PAUSA) {
        definirEstado(estadoAntesDaPausa);
    }
}

static void inicializarJogo(void)
{
    SetTargetFPS(FPS_ALVO);
    SetExitKey(KEY_NULL);

    /* Inicializa o controlador de combate. */
    inicializarCombate(&combate);

    /* Cada ataque e defesa vira uma animacao; sons e mensagens saem nos
     * marcos dela. O dano ja foi resolvido pela logica nesse instante. */
    iniciarAnimacaoCombate(&animacao);
    registrarObservadorCombate(observarEventoAnimacao, &animacao);
    iniciarAudioCombate(&sons);

    sairSolicitado = false;
    prepararNovaPartida();
    definirEstado(ESTADO_MENU);
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
    case COMANDO_MENU:
        if (!animando) {
            voltarAoMenu();
        }
        break;
    case COMANDO_NENHUM:
        break;
    }
}


static void atualizarJogo(float deltaTempo)
{
    if (estadoAtual == ESTADO_MENU) {
        const AcaoTelaJogo acao = interpretarEntradaMenu(
            lerEntradaTelaJogo(), GetScreenWidth(), GetScreenHeight()
        );

        if (acao == ACAO_TELA_JOGAR) {
            iniciarNovaPartida();
        } else if (acao == ACAO_TELA_SAIR) {
            sairSolicitado = true;
        }
        return;
    }

    if (estadoAtual == ESTADO_RETORNO_AO_LAR) {
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_ESCAPE)) {
            voltarAoMenu();
        }
        return;
    }

    if (estadoAtual == ESTADO_NARRATIVA) {
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
            definirEstado(ESTADO_EXPLORACAO);
        }
        return;
    }

    if (estadoAtual == ESTADO_PAUSA) {
        const bool pausouCombate = estadoAntesDaPausa == ESTADO_COMBATE;
        const AcaoTelaJogo acao = interpretarEntradaPausa(
            lerEntradaTelaJogo(), pausouCombate,
            GetScreenWidth(), GetScreenHeight()
        );

        switch (acao) {
        case ACAO_TELA_RETOMAR:
            retomarJogo();
            break;
        case ACAO_TELA_REINICIAR:
            if (pausouCombate) {
                reiniciarConfronto();
            }
            break;
        case ACAO_TELA_MENU:
            voltarAoMenu();
            break;
        case ACAO_TELA_NENHUMA:
        case ACAO_TELA_JOGAR:
        case ACAO_TELA_SAIR:
            break;
        }
        return;
    }

    if (IsKeyPressed(KEY_ESCAPE)) {
        pausarJogo();
        return;
    }

    if (estadoAtual == ESTADO_COMBATE) {
        atualizarConfronto(deltaTempo);
        if (estadoAtual == ESTADO_MENU) {
            return;
        }
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

    if (fase.etapa == ETAPA_CONCLUIDA) {
        const ResultadoAvancoJornada resultado =
            avancarJornada(&jornada, &fase);

        if (resultado == JORNADA_PROXIMA_FASE) {
            definirControleJogador(&jogador, false);
            inicializarJogador(&jogador, fase.pontoInicial, 0.0f);
            iniciarEsperaTurnoInimigo(&esperaInimigo);
            iniciarAnimacaoCombate(&animacao);
            if (!criarInimigosDaFase()) {
                TraceLog(LOG_ERROR, "Falha ao carregar a proxima etapa.");
                voltarAoMenu();
                return;
            }
            narrativa = criarNarrativaTransicao(
                fase.titulo,
                fase.configuracao->textoNarrativo,
                fase.objetivo.descricao
            );
            definirEstado(ESTADO_NARRATIVA);
            TraceLog(LOG_INFO, "Jornada avancou para: %s", fase.titulo);
        } else if (resultado == JORNADA_RETORNO_AO_LAR) {
            narrativa = criarNarrativaEncerramento();
            definirEstado(ESTADO_RETORNO_AO_LAR);
        }
    }

    if (fase.etapa == ETAPA_CONFRONTO && estadoAtual == ESTADO_EXPLORACAO) {
        iniciarConfronto();
    }
}

static void desenharJogo(void)
{
    const char *nomeEstado = "EXPLORACAO";
    const bool pausado = estadoAtual == ESTADO_PAUSA;
    const bool combatendo = estadoAtual == ESTADO_COMBATE ||
        (pausado && estadoAntesDaPausa == ESTADO_COMBATE);
    PoseJogador poseJogador = {0};
    PoseInimigo posesInimigos[FASE_MAX_INIMIGOS] = {0};
    Camera3D camera = jogador.camera;

    if (estadoAtual == ESTADO_NARRATIVA ||
        estadoAtual == ESTADO_RETORNO_AO_LAR) {
        desenharTelaNarrativa(
            &narrativa,
            GetScreenWidth(),
            GetScreenHeight()
        );
        return;
    }

    if (estadoAtual == ESTADO_MENU) {
        desenharTelaMenu(GetScreenWidth(), GetScreenHeight());
        DrawFPS(12, 12);
        return;
    }

    if (combatendo) {
        nomeEstado = "COMBATE";
        poseJogador = poseDoJogador(&animacao, &jornada.odisseu);
        for (int i = 0; i < fase.quantidadeInimigos &&
                        i < painel.quantidadeInimigos; i++) {
            posesInimigos[i] = poseDoInimigo(&animacao, painel.inimigos[i]);
        }
        camera = camaraComEfeitos(jogador.camera, &poseJogador, (float)GetTime());
    } else if (pausado) {
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

    if (pausado) {
        desenharTelaPausa(
            GetScreenWidth(), GetScreenHeight(),
            estadoAntesDaPausa == ESTADO_COMBATE
        );
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

    while (!WindowShouldClose() && !sairSolicitado) {
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

#include "hud_combate.h"

#include <math.h>
#include <stddef.h>

#include "fase_render.h"

enum {
    MARGEM = 12,
    LARGURA_BOTAO = 170,
    ALTURA_BOTAO = 56,
    ESPACO_BOTOES = 16,
    DISTANCIA_BOTOES_BASE = 130,
    LARGURA_PAINEL_INIMIGOS = 300,
    ALTURA_LINHA_INIMIGO = 52,
    ESPACO_LINHAS = 6,
    TOPO_PAINEL_INIMIGOS = 62,
    LARGURA_PAINEL_JOGADOR = 320,
    ALTURA_PAINEL_JOGADOR = 72,
    LARGURA_BOTAO_RESULTADO = 300,
    TOPO_ORDEM_TURNOS = 88,
    LARGURA_FICHA_TURNO = 118,
    ALTURA_FICHA_TURNO = 40,
    ESPACO_FICHAS_TURNO = 6
};

#define ALTURA_MARCADOR_ALVO 5.4f /* acima dos pes do inimigo */

/* ---- Posicoes (usadas tanto para desenhar quanto para clicar) ---- */

static Rectangle retanguloBotao(int indice, int largura, int altura)
{
    const float x = (float)(largura - MARGEM - 2 * LARGURA_BOTAO - ESPACO_BOTOES)
                    + (float)(indice * (LARGURA_BOTAO + ESPACO_BOTOES));

    return (Rectangle){
        x, (float)(altura - DISTANCIA_BOTOES_BASE),
        (float)LARGURA_BOTAO, (float)ALTURA_BOTAO
    };
}

static Rectangle retanguloInimigo(int indice, int largura)
{
    return (Rectangle){
        (float)(largura - MARGEM - LARGURA_PAINEL_INIMIGOS),
        (float)(TOPO_PAINEL_INIMIGOS +
                indice * (ALTURA_LINHA_INIMIGO + ESPACO_LINHAS)),
        (float)LARGURA_PAINEL_INIMIGOS, (float)ALTURA_LINHA_INIMIGO
    };
}

static Rectangle retanguloPainelJogador(int altura)
{
    return (Rectangle){
        (float)MARGEM, (float)(altura - DISTANCIA_BOTOES_BASE),
        (float)LARGURA_PAINEL_JOGADOR, (float)ALTURA_PAINEL_JOGADOR
    };
}

static Rectangle retanguloContinuar(int largura, int altura)
{
    return (Rectangle){
        (float)(largura - LARGURA_BOTAO_RESULTADO) / 2.0f,
        (float)altura / 2.0f + 25.0f,
        (float)LARGURA_BOTAO_RESULTADO, (float)ALTURA_BOTAO
    };
}

static Rectangle retanguloMenuResultado(int largura, int altura)
{
    return (Rectangle){
        (float)(largura - LARGURA_BOTAO_RESULTADO) / 2.0f,
        (float)altura / 2.0f + 95.0f,
        (float)LARGURA_BOTAO_RESULTADO, (float)ALTURA_BOTAO
    };
}

static bool combateAcabou(const Combate *combate)
{
    return combate->estado == COMBATE_VICTORY ||
           combate->estado == COMBATE_DEFEAT;
}

/* ---- Entrada ---- */

EntradaCombate lerEntradaCombate(void)
{
    EntradaCombate entrada = {0};

    entrada.teclaAtacar = IsKeyPressed(KEY_A);
    entrada.teclaDefender = IsKeyPressed(KEY_D);
    entrada.teclaProximo = IsKeyPressed(KEY_TAB) ||
                           IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_DOWN);
    entrada.teclaAnterior = IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_UP);
    entrada.teclaContinuar = IsKeyPressed(KEY_ENTER);
    entrada.teclaMenu = IsKeyPressed(KEY_M);
    entrada.clique = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
    entrada.mouse = GetMousePosition();

    return entrada;
}

ComandoCombate interpretarEntradaCombate(
    PainelCombate *painel,
    const Combate *combate,
    EntradaCombate entrada,
    int largura,
    int altura
)
{
    if (painel == NULL || combate == NULL) {
        return COMANDO_NENHUM;
    }

    if (combateAcabou(combate)) {
        const bool clicouContinuar =
            entrada.clique &&
            CheckCollisionPointRec(entrada.mouse, retanguloContinuar(largura, altura));
        const bool clicouMenu =
            entrada.clique &&
            CheckCollisionPointRec(
                entrada.mouse, retanguloMenuResultado(largura, altura)
            );

        if (entrada.teclaMenu || clicouMenu) {
            return COMANDO_MENU;
        }

        return (entrada.teclaContinuar || clicouContinuar)
                   ? COMANDO_CONTINUAR : COMANDO_NENHUM;
    }

    /* Escolher o alvo vale durante todo o combate, mesmo no turno inimigo. */
    if (entrada.teclaProximo) {
        mudarAlvoPainel(painel, 1);
    }
    if (entrada.teclaAnterior) {
        mudarAlvoPainel(painel, -1);
    }
    if (entrada.clique) {
        for (int i = 0; i < painel->quantidadeInimigos; i++) {
            if (CheckCollisionPointRec(entrada.mouse, retanguloInimigo(i, largura))) {
                selecionarAlvoPainel(painel, i);
            }
        }
    }

    /* Fora da vez de Odisseu os comandos ficam desabilitados. */
    if (combate->estado != COMBATE_WAITING_PLAYER) {
        return COMANDO_NENHUM;
    }

    if (painel->alvo != NULL &&
        (entrada.teclaAtacar ||
         (entrada.clique && CheckCollisionPointRec(
             entrada.mouse, retanguloBotao(0, largura, altura))))) {
        return COMANDO_ATACAR;
    }

    if (entrada.teclaDefender ||
        (entrada.clique && CheckCollisionPointRec(
            entrada.mouse, retanguloBotao(1, largura, altura)))) {
        return COMANDO_DEFENDER;
    }

    return COMANDO_NENHUM;
}

/* ---- Desenho ---- */

/* Texto com sombra para ler bem sobre qualquer fundo. */
static void desenharTexto(const char *texto, int x, int y, int tamanho, Color cor)
{
    DrawText(texto, x + 1, y + 1, tamanho, (Color){ 0, 0, 0, 200 });
    DrawText(texto, x, y, tamanho, cor);
}

static void desenharTextoCentralizado(
    const char *texto, Rectangle area, int tamanho, Color cor
)
{
    const int largura = MeasureText(texto, tamanho);
    const int x = (int)(area.x + (area.width - (float)largura) / 2.0f);
    const int y = (int)(area.y + (area.height - (float)tamanho) / 2.0f);

    DrawText(texto, x, y, tamanho, cor);
}

static void desenharBarraVida(Rectangle area, int vida, int vidaMaxima)
{
    float fracao = vidaMaxima > 0 ? (float)vida / (float)vidaMaxima : 0.0f;
    Color cor = { 70, 190, 90, 255 };

    if (fracao < 0.0f) {
        fracao = 0.0f;
    } else if (fracao > 1.0f) {
        fracao = 1.0f;
    }

    if (fracao <= 0.25f) {
        cor = (Color){ 215, 60, 50, 255 };
    } else if (fracao <= 0.5f) {
        cor = (Color){ 230, 190, 50, 255 };
    }

    DrawRectangleRec(area, (Color){ 30, 30, 30, 230 });
    DrawRectangleRec(
        (Rectangle){ area.x, area.y, area.width * fracao, area.height }, cor
    );
    DrawRectangleLinesEx(area, 1.0f, BLACK);
    desenharTextoCentralizado(
        TextFormat("%d/%d", vida, vidaMaxima), area, 14, WHITE
    );
}

static void desenharPersonagem(
    const Personagem *personagem, int vida, Rectangle area, bool selecionado
)
{
    const bool derrotado = vida <= 0;
    const Color fundo = derrotado ? (Color){ 50, 50, 50, 170 }
                                  : (Color){ 0, 0, 0, 140 };

    DrawRectangleRec(area, fundo);
    if (selecionado) {
        DrawRectangleLinesEx(area, 3.0f, GOLD);
    }

    desenharTexto(
        derrotado ? TextFormat("%s (derrotado)", personagem->nome)
                  : personagem->nome,
        (int)area.x + 8, (int)area.y + 6, 18, derrotado ? LIGHTGRAY : WHITE
    );

    if (personagem->bonusDefesa > 0) {
        const char *guarda = TextFormat("GUARDA +%d", personagem->bonusDefesa);

        desenharTexto(
            guarda,
            (int)(area.x + area.width) - MeasureText(guarda, 14) - 8,
            (int)area.y + 8, 14, SKYBLUE
        );
    }

    desenharBarraVida(
        (Rectangle){ area.x + 8, area.y + area.height - 24, area.width - 16, 18 },
        vida, personagem->vidaMaxima
    );
}

static void desenharInimigos(
    const PainelCombate *painel, const AnimacaoCombate *animacao, int largura
)
{
    desenharTexto(
        "Inimigos", largura - MARGEM - LARGURA_PAINEL_INIMIGOS,
        TOPO_PAINEL_INIMIGOS - 24, 18, WHITE
    );

    for (int i = 0; i < painel->quantidadeInimigos; i++) {
        desenharPersonagem(
            painel->inimigos[i], vidaExibida(animacao, painel->inimigos[i]),
            retanguloInimigo(i, largura), painel->inimigos[i] == painel->alvo
        );
    }
}

static void desenharBotao(
    Rectangle area, const char *texto, Color cor, bool habilitado
)
{
    const bool sobre = habilitado && CheckCollisionPointRec(GetMousePosition(), area);
    Color fundo = habilitado ? cor : (Color){ 90, 90, 90, 200 };

    if (sobre) {
        fundo = ColorBrightness(fundo, 0.25f);
    }

    DrawRectangleRec(area, fundo);
    DrawRectangleLinesEx(area, 2.0f, habilitado ? WHITE : GRAY);
    desenharTextoCentralizado(texto, area, 22, habilitado ? WHITE : LIGHTGRAY);
}

static void desenharBotoes(
    const PainelCombate *painel, const Combate *combate, int largura, int altura
)
{
    const bool suaVez = combate->estado == COMBATE_WAITING_PLAYER;

    desenharBotao(
        retanguloBotao(0, largura, altura), "[A] Atacar",
        (Color){ 190, 60, 50, 255 }, suaVez && painel->alvo != NULL
    );
    desenharBotao(
        retanguloBotao(1, largura, altura), "[D] Defender",
        (Color){ 50, 110, 190, 255 }, suaVez
    );
}

static void desenharMensagens(const PainelCombate *painel, int altura)
{
    const int linha = 22;
    const int topo = altura - DISTANCIA_BOTOES_BASE - 12 -
                     painel->quantidadeMensagens * linha;

    for (int i = 0; i < painel->quantidadeMensagens; i++) {
        const bool maisNova = i == painel->quantidadeMensagens - 1;

        desenharTexto(
            painel->mensagens[i], MARGEM, topo + i * linha, 20,
            maisNova ? YELLOW : WHITE
        );
    }
}

static void desenharTurno(const Combate *combate, int largura)
{
    const char *situacao = NULL;
    Color cor = WHITE;

    if (combate->estado == COMBATE_WAITING_PLAYER) {
        situacao = "SUA VEZ: escolha uma acao";
        cor = GOLD;
    } else if (combate->estado == COMBATE_ENEMY_TURN &&
               combate->participanteAtual != NULL) {
        situacao = TextFormat("Vez de %s...", combate->participanteAtual->nome);
    }

    if (combate->rodada > 0) {
        const char *rodada = TextFormat("Rodada %u", combate->rodada);

        desenharTexto(rodada, (largura - MeasureText(rodada, 20)) / 2, 12, 20, WHITE);
    }

    if (situacao != NULL) {
        desenharTexto(
            situacao, (largura - MeasureText(situacao, 24)) / 2, 38, 24, cor
        );
    }
}

static void desenharFichaTurno(const TurnoPainel *turno, Rectangle ficha)
{
    const Personagem *personagem = turno->personagem;
    const bool atual = turno->situacao == TURNO_ATUAL;
    const bool jaAgiu = turno->situacao == TURNO_JA_AGIU;
    Color fundo = personagem->tipo == TIPO_HEROI ? (Color){ 50, 90, 150, 235 }
                                                 : (Color){ 140, 60, 50, 235 };
    const char *marca = atual ? "AGORA" : (jaAgiu ? "agiu" : NULL);

    if (jaAgiu) {
        fundo = Fade(fundo, 0.45f);
    }

    DrawRectangleRec(ficha, fundo);
    DrawRectangleLinesEx(ficha, atual ? 3.0f : 1.0f, atual ? GOLD : BLACK);

    desenharTexto(
        personagem->nome, (int)ficha.x + 8, (int)ficha.y + 4, 16,
        jaAgiu ? LIGHTGRAY : WHITE
    );
    desenharTexto(
        TextFormat("ini %d", personagem->iniciativa),
        (int)ficha.x + 8, (int)ficha.y + 23, 12, jaAgiu ? GRAY : LIGHTGRAY
    );

    if (marca != NULL) {
        desenharTexto(
            marca, (int)(ficha.x + ficha.width) - MeasureText(marca, 12) - 8,
            (int)ficha.y + 23, 12, atual ? GOLD : GRAY
        );
    }
}

/* Ordem da rodada, lida da lista circular a cada quadro: acompanha o
 * QuickSort e as remocoes sozinha. */
static void desenharOrdemTurnos(const Combate *combate, int largura)
{
    TurnoPainel ordem[PAINEL_MAX_TURNOS];
    const int quantidade = montarOrdemTurnos(combate, ordem, PAINEL_MAX_TURNOS);
    const char *titulo = "Ordem dos turnos";
    const int total = quantidade * LARGURA_FICHA_TURNO +
                      (quantidade - 1) * ESPACO_FICHAS_TURNO;

    if (quantidade == 0) {
        return;
    }

    desenharTexto(
        titulo, (largura - MeasureText(titulo, 14)) / 2,
        TOPO_ORDEM_TURNOS - 18, 14, WHITE
    );

    for (int i = 0; i < quantidade; i++) {
        desenharFichaTurno(
            &ordem[i],
            (Rectangle){
                (float)((largura - total) / 2 +
                        i * (LARGURA_FICHA_TURNO + ESPACO_FICHAS_TURNO)),
                (float)TOPO_ORDEM_TURNOS,
                (float)LARGURA_FICHA_TURNO, (float)ALTURA_FICHA_TURNO
            }
        );
    }
}

/* Seta sobre a cabeca do inimigo selecionado, no mundo 3D. */
static void desenharMarcadorAlvo(
    const PainelCombate *painel, const Fase *fase, Camera3D camera,
    const AnimacaoCombate *animacao
)
{
    for (int i = 0; i < painel->quantidadeInimigos &&
                    i < fase->quantidadeInimigos; i++) {
        /* Acompanha o inimigo quando ele investe ou recua. */
        const PoseInimigo pose = poseDoInimigo(animacao, painel->inimigos[i]);
        Vector3 ancora = posicaoInimigoComPose(fase, i, &pose);
        const Vector3 frente = {
            camera.target.x - camera.position.x,
            camera.target.y - camera.position.y,
            camera.target.z - camera.position.z
        };
        const Vector3 ate = {
            ancora.x - camera.position.x,
            ancora.y + ALTURA_MARCADOR_ALVO - camera.position.y,
            ancora.z - camera.position.z
        };

        if (painel->inimigos[i] != painel->alvo) {
            continue;
        }

        /* Alvo atras da camera nao tem posicao na tela. */
        if (frente.x * ate.x + frente.y * ate.y + frente.z * ate.z <= 0.0f) {
            return;
        }

        ancora.y += ALTURA_MARCADOR_ALVO;
        const Vector2 p = GetWorldToScreen(ancora, camera);
        const float balanco = sinf((float)GetTime() * 4.0f) * 4.0f;
        const Vector2 a = { p.x - 14.0f, p.y - 26.0f + balanco };
        const Vector2 b = { p.x, p.y + balanco };
        const Vector2 c = { p.x + 14.0f, p.y - 26.0f + balanco };

        DrawTriangle(a, b, c, GOLD);
        DrawTriangleLines(a, b, c, BLACK);
        return;
    }
}

static void desenharResultado(const Combate *combate, int largura, int altura)
{
    const bool vitoria = combate->estado == COMBATE_VICTORY;
    const char *titulo = vitoria ? "VITORIA!" : "DERROTA";
    const char *detalhe = vitoria ? "Os inimigos foram vencidos."
                                  : "Odisseu foi derrotado.";
    const char *botao = vitoria ? "[ENTER] Continuar"
                                : "[ENTER] Tentar de novo";

    DrawRectangle(0, 0, largura, altura, (Color){ 0, 0, 0, 120 });

    desenharTexto(
        titulo, (largura - MeasureText(titulo, 64)) / 2, altura / 2 - 90, 64,
        vitoria ? GOLD : (Color){ 230, 70, 60, 255 }
    );
    desenharTexto(
        detalhe, (largura - MeasureText(detalhe, 24)) / 2, altura / 2 - 10, 24, WHITE
    );

    desenharBotao(
        retanguloContinuar(largura, altura), botao,
        vitoria ? (Color){ 60, 140, 70, 255 } : (Color){ 190, 60, 50, 255 }, true
    );
    desenharBotao(
        retanguloMenuResultado(largura, altura), "[M] Menu principal",
        (Color){ 70, 80, 100, 255 }, true
    );
}

void desenharHudCombate(
    const PainelCombate *painel,
    const Combate *combate,
    const Fase *fase,
    Camera3D camera,
    const AnimacaoCombate *animacao
)
{
    const int largura = GetScreenWidth();
    const int altura = GetScreenHeight();

    if (painel == NULL || combate == NULL || combate->jogador == NULL) {
        return;
    }

    if (!combateAcabou(combate)) {
        desenharMarcadorAlvo(painel, fase, camera, animacao);
    }

    desenharTurno(combate, largura);
    desenharOrdemTurnos(combate, largura);
    desenharInimigos(painel, animacao, largura);
    desenharPersonagem(
        combate->jogador, vidaExibida(animacao, combate->jogador),
        retanguloPainelJogador(altura), false
    );
    desenharMensagens(painel, altura);
    desenharBotoes(painel, combate, largura, altura);

    desenharTexto(
        "A atacar | D defender | TAB ou setas trocam o alvo | clique nos botoes",
        MARGEM, altura - 28, 18, WHITE
    );

    /* O resultado espera o golpe final acertar e o derrotado tombar. */
    if (combateAcabou(combate) && !animacaoCombateOcupada(animacao)) {
        desenharResultado(combate, largura, altura);
    }
}

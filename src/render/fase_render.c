#include "fase_render.h"

#include <math.h>
#include <stdio.h>

#include "interpolacao.h"
#include "rlgl.h"

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

/* Mistura `cor` com branco: 0 mantem a cor, 1 deixa quase branca. */
static Color clarear(Color cor, float quanto)
{
    const float t = limitar01(quanto) * 0.7f;

    return (Color){
        (unsigned char)interpolar((float)cor.r, 255.0f, t),
        (unsigned char)interpolar((float)cor.g, 255.0f, t),
        (unsigned char)interpolar((float)cor.b, 255.0f, t),
        cor.a
    };
}

/* Direcao horizontal do inimigo para o ponto onde Odisseu fica no confronto. */
static Vector3 direcaoParaJogador(const Fase *fase, Vector3 base)
{
    Vector3 dir = {
        fase->pontoJogadorConfronto.x - base.x, 0.0f,
        fase->pontoJogadorConfronto.z - base.z
    };
    const float tamanho = sqrtf(dir.x * dir.x + dir.z * dir.z);

    if (tamanho > 0.001f) {
        dir.x /= tamanho;
        dir.z /= tamanho;
    } else {
        dir = (Vector3){ 0.0f, 0.0f, 1.0f };
    }

    return dir;
}

Vector3 posicaoInimigoComPose(const Fase *fase, int indice, const PoseInimigo *pose)
{
    const Vector3 base = fase->pontosInimigos[indice];
    const Vector3 dir = direcaoParaJogador(fase, base);
    const float deslocamento = pose->avanco * 1.6f - pose->preparo * 0.5f -
                               pose->recuo * 0.8f;

    return (Vector3){
        base.x + dir.x * deslocamento,
        base.y + pose->queda * 1.2f, /* deitado, apoiado no chao */
        base.z + dir.z * deslocamento
    };
}

/* Representacao provisoria de Polifemo (primitivas), no ponto do inimigo,
 * deslocada e inclinada pela pose: investe contra Odisseu, recua ao ser
 * atingido, abaixa na defesa e tomba ao ser derrotado. */
static void desenharInimigo(const Fase *fase, int indice, const PoseInimigo *pose)
{
    const Vector3 posicao = posicaoInimigoComPose(fase, indice, pose);
    const Vector3 dir = direcaoParaJogador(fase, fase->pontosInimigos[indice]);
    /* Inclina para o jogador ao investir, para tras ao se preparar, e tomba. */
    const float inclinacao = pose->avanco * 12.0f - pose->preparo * 10.0f -
                             pose->queda * 90.0f;
    const float altura = 1.0f - 0.18f * pose->agachar;
    const Color corpo = clarear((Color){ 120, 80, 70, 255 }, pose->brilho);
    const Color cabecaCor = clarear((Color){ 150, 105, 90, 255 }, pose->brilho);

    rlPushMatrix();
    rlTranslatef(posicao.x, posicao.y, posicao.z);
    /* Eixo do tombo: horizontal, perpendicular a direcao do jogador. */
    rlRotatef(inclinacao, dir.z, 0.0f, -dir.x);
    rlScalef(1.0f, altura, 1.0f);

    DrawCylinder((Vector3){ 0.0f, 0.0f, 0.0f }, 1.0f, 1.5f, 3.6f, 16, corpo);
    DrawSphere((Vector3){ 0.0f, 4.2f, 0.0f }, 0.95f, cabecaCor);
    DrawSphere((Vector3){ 0.0f, 4.3f, 0.75f }, 0.28f, WHITE);
    DrawSphere((Vector3){ 0.0f, 4.3f, 0.93f }, 0.12f, BLACK);

    rlPopMatrix();
}

static void desenharInimigos(const Fase *fase, const PoseInimigo *poses)
{
    const PoseInimigo repouso = {0};

    if (fase->etapa != ETAPA_CONFRONTO) {
        return;
    }

    for (int i = 0; i < fase->quantidadeInimigos; i++) {
        desenharInimigo(fase, i, poses != NULL ? &poses[i] : &repouso);
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

void desenharFase(const Fase *fase, const PoseInimigo *poses)
{
    desenharCenario(&fase->cenario);
    desenharMarcaDeSaida(fase);
    desenharItens(fase);
    desenharInimigos(fase, poses);
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

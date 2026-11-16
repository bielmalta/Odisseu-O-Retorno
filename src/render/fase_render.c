#include "fase_render.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

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
    const ConfiguracaoFase *configuracao = fase->configuracao;
    const Vector3 tamanho = configuracao->tamanhoItem;

    for (int i = 0; i < fase->quantidadeItens; i++) {
        const bool concluido = fase->itens[i].concluido;
        const bool remover = configuracao->itens[i].removerAoInteragir;

        if (concluido && remover) {
            continue;
        }
        const Vector3 p = fase->itens[i].posicao;
        const bool mirado = (i == fase->itemAlvo);
        const Color cor = concluido ? (Color){ 70, 150, 90, 255 }
                                    : configuracao->corItem;

        DrawCubeV(p, tamanho, cor);
        DrawCubeWiresV(
            p,
            tamanho,
            mirado ? YELLOW : configuracao->corContornoItem
        );
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

/* Representacao provisoria configurada para o inimigo, deslocada e inclinada
 * pela pose: investe, recua, abaixa na defesa e tomba ao ser derrotado. */
static void desenharInimigo(const Fase *fase, int indice, const PoseInimigo *pose)
{
    const ConfiguracaoVisualInimigo *visual =
        &fase->configuracao->encontro.inimigos[indice].visual;
    const Vector3 posicao = posicaoInimigoComPose(fase, indice, pose);
    const Vector3 dir = direcaoParaJogador(fase, fase->pontosInimigos[indice]);
    /* Inclina para o jogador ao investir, para tras ao se preparar, e tomba. */
    const float inclinacao = pose->avanco * 12.0f - pose->preparo * 10.0f -
                             pose->queda * 90.0f;
    const float altura = 1.0f - 0.18f * pose->agachar;
    const float escala = visual->escala > 0.0f ? visual->escala : 1.0f;
    const Color corpo = clarear(visual->corCorpo, pose->brilho);
    const Color cabecaCor = clarear(visual->corCabeca, pose->brilho);

    rlPushMatrix();
    rlTranslatef(posicao.x, posicao.y, posicao.z);
    /* Eixo do tombo: horizontal, perpendicular a direcao do jogador. */
    rlRotatef(inclinacao, dir.z, 0.0f, -dir.x);
    rlScalef(escala, altura * escala, escala);

    if (visual->modelo != NULL &&
        strcmp(visual->modelo, "ciclope_primitivo") == 0) {
        DrawCylinder(
            (Vector3){ 0.0f, 0.0f, 0.0f },
            1.0f, 1.5f, 3.6f, 16, corpo
        );
        DrawSphere((Vector3){ 0.0f, 4.2f, 0.0f }, 0.95f, cabecaCor);
        DrawSphere((Vector3){ 0.0f, 4.3f, 0.75f }, 0.28f, WHITE);
        DrawSphere((Vector3){ 0.0f, 4.3f, 0.93f }, 0.12f, BLACK);
    } else if (visual->modelo != NULL &&
               strcmp(visual->modelo, "sereia_primitiva") == 0) {
        DrawCylinder(
            (Vector3){ 0.0f, 0.0f, 0.0f },
            0.8f, 0.55f, 3.0f, 12, corpo
        );
        DrawSphere((Vector3){ 0.0f, 3.55f, 0.0f }, 0.65f, cabecaCor);
        DrawCubeV(
            (Vector3){ -1.15f, 2.1f, 0.0f },
            (Vector3){ 1.3f, 1.8f, 0.18f },
            clarear(corpo, 0.15f)
        );
        DrawCubeV(
            (Vector3){ 1.15f, 2.1f, 0.0f },
            (Vector3){ 1.3f, 1.8f, 0.18f },
            clarear(corpo, 0.15f)
        );
    } else if (visual->modelo != NULL &&
               strcmp(visual->modelo, "tentaculo_primitivo") == 0) {
        DrawCylinder(
            (Vector3){ 0.0f, 0.0f, 0.0f },
            0.8f, 0.35f, 4.2f, 14, corpo
        );
        DrawSphere((Vector3){ 0.0f, 4.2f, 0.0f }, 0.38f, cabecaCor);
        DrawSphere((Vector3){ 0.0f, 1.0f, 0.65f }, 0.16f, cabecaCor);
        DrawSphere((Vector3){ 0.0f, 2.0f, 0.48f }, 0.14f, cabecaCor);
        DrawSphere((Vector3){ 0.0f, 3.0f, 0.30f }, 0.12f, cabecaCor);
    } else if (visual->modelo != NULL &&
               strcmp(visual->modelo, "pretendente_primitivo") == 0) {
        DrawCubeV(
            (Vector3){ 0.0f, 1.7f, 0.0f },
            (Vector3){ 1.6f, 3.4f, 1.0f },
            corpo
        );
        DrawSphere((Vector3){ 0.0f, 3.9f, 0.0f }, 0.7f, cabecaCor);
        DrawCubeV(
            (Vector3){ 1.0f, 1.5f, 0.35f },
            (Vector3){ 0.16f, 3.0f, 0.16f },
            LIGHTGRAY
        );
    } else {
        /* Modelo generico usado por uma configuracao ainda sem asset proprio. */
        DrawCubeV(
            (Vector3){ 0.0f, 1.8f, 0.0f },
            (Vector3){ 1.8f, 3.6f, 1.2f },
            corpo
        );
        DrawSphere((Vector3){ 0.0f, 4.1f, 0.0f }, 0.75f, cabecaCor);
    }

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
        return fase->configuracao->textoObjetivoConfronto;
    case ETAPA_SAIDA_LIBERADA:
        return fase->configuracao->textoObjetivoSaida;
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
        const char *verbo = fase->objetivo.tipo == OBJETIVO_COLETA
                                ? "Coletar"
                                : "Ativar";
        const char *aviso = TextFormat(
            "[E] %s %s", verbo, fase->objetivo.nomeAlvo
        );
        const int larguraAviso = MeasureText(aviso, 22);
        desenharTexto(aviso, centroX - larguraAviso / 2, centroY + 30, 22, YELLOW);
    }

    /* Mensagem temporaria */
    if (fase->tempoMensagem > 0.0f && fase->mensagem[0] != '\0') {
        const int larguraMsg = MeasureText(fase->mensagem, 22);
        desenharTexto(fase->mensagem, centroX - larguraMsg / 2, altura - 90, 22, WHITE);
    }
}

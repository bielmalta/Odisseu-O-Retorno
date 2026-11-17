#include "fase_render.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

#include "cenario_render.h"
#include "interpolacao.h"
#include "rlgl.h"

/* Texto com sombra para ler bem sobre qualquer fundo. */
static void desenharTexto(const char *texto, int x, int y, int tamanho, Color cor)
{
    DrawText(texto, x + 1, y + 1, tamanho, (Color){ 0, 0, 0, 200 });
    DrawText(texto, x, y, tamanho, cor);
}

/* Itens flutuam e giram, com um brilho em volta, ate serem usados. */
static void desenharItens(const Fase *fase)
{
    const ConfiguracaoFase *configuracao = fase->configuracao;
    const Vector3 tamanho = configuracao->tamanhoItem;
    const float maior = fmaxf(tamanho.x, fmaxf(tamanho.y, tamanho.z));
    const float tempo = (float)GetTime();

    for (int i = 0; i < fase->quantidadeItens; i++) {
        const bool concluido = fase->itens[i].concluido;

        if (concluido && configuracao->itens[i].removerAoInteragir) {
            continue;
        }

        Vector3 p = fase->itens[i].posicao;
        const Color cor = concluido ? (Color){ 70, 150, 90, 255 }
                                    : configuracao->corItem;

        if (!concluido) {
            p.y += sinf(tempo * 2.0f + (float)i) * 0.08f;
        }

        rlPushMatrix();
        rlTranslatef(p.x, p.y, p.z);
        rlRotatef(concluido ? 0.0f : tempo * 50.0f, 0.0f, 1.0f, 0.0f);
        DrawCubeV((Vector3){ 0.0f, 0.0f, 0.0f }, tamanho, cor);
        DrawCubeWiresV(
            (Vector3){ 0.0f, 0.0f, 0.0f },
            tamanho,
            i == fase->itemAlvo ? YELLOW : configuracao->corContornoItem
        );
        rlPopMatrix();

        if (!concluido) {
            DrawSphere(p, maior * 0.9f, (Color){ 255, 230, 140, 45 });
        }
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

/* ---- Modelos dos inimigos: listas de pecas (o corpo olha para +z) ---- */

enum { P_CORPO, P_CABECA, P_BRANCO, P_PRETO, P_METAL, P_MADEIRA, P_OURO, P_CAPA };

typedef struct {
    char forma; /* 'c', 'e' ou 'b', como nos enfeites da fase */
    Vector3 posicao;
    Vector3 tamanho;
    int cor; /* P_CORPO e P_CABECA vem da configuracao do inimigo */
} Peca;

#define PC(x, y, z, rb, h, rt, cor) { 'c', { x, y, z }, { rb, h, rt }, cor }
#define PE(x, y, z, rx, ry, rz, cor) { 'e', { x, y, z }, { rx, ry, rz }, cor }
#define PB(x, y, z, lx, ly, lz, cor) { 'b', { x, y, z }, { lx, ly, lz }, cor }

static const Peca CICLOPE[] = {
    PC(-0.5f, 0.0f, 0.0f, 0.42f, 1.7f, 0.36f, P_CORPO), /* pernas */
    PC(0.5f, 0.0f, 0.0f, 0.42f, 1.7f, 0.36f, P_CORPO),
    PC(0.0f, 1.3f, 0.0f, 1.15f, 0.8f, 1.0f, P_MADEIRA), /* tanga */
    PC(0.0f, 1.9f, 0.0f, 1.0f, 1.9f, 1.3f, P_CORPO), /* tronco */
    PE(0.0f, 2.5f, 0.45f, 1.0f, 0.75f, 0.7f, P_CORPO),
    PC(-1.65f, 1.7f, 0.0f, 0.4f, 2.0f, 0.5f, P_CORPO), /* bracos e maos */
    PC(1.65f, 1.7f, 0.0f, 0.4f, 2.0f, 0.5f, P_CORPO),
    PE(-1.65f, 1.65f, 0.0f, 0.42f, 0.42f, 0.42f, P_CABECA),
    PE(1.65f, 1.65f, 0.0f, 0.42f, 0.42f, 0.42f, P_CABECA),
    PC(1.65f, 1.4f, 0.5f, 0.22f, 2.8f, 0.55f, P_MADEIRA), /* clava */
    PE(0.0f, 4.25f, 0.0f, 1.0f, 0.95f, 0.95f, P_CABECA),
    PE(0.0f, 4.75f, -0.15f, 0.95f, 0.5f, 0.95f, P_PRETO), /* cabelo */
    PB(0.0f, 4.55f, 0.85f, 1.5f, 0.2f, 0.35f, P_PRETO), /* sobrancelha */
    PE(0.0f, 4.25f, 0.88f, 0.34f, 0.34f, 0.18f, P_BRANCO), /* o olho unico */
    PE(0.0f, 4.25f, 1.02f, 0.15f, 0.15f, 0.08f, P_PRETO),
    PB(0.0f, 3.7f, 0.9f, 0.7f, 0.14f, 0.2f, P_PRETO) /* boca */
};

static const Peca SEREIA[] = {
    PC(0.0f, 0.0f, 0.0f, 0.2f, 1.9f, 0.62f, P_CORPO), /* cauda e nadadeira */
    PB(0.0f, 0.1f, 0.0f, 1.5f, 0.12f, 0.45f, P_CORPO),
    PC(0.0f, 1.9f, 0.0f, 0.62f, 0.7f, 0.5f, P_CORPO),
    PC(0.0f, 2.55f, 0.0f, 0.5f, 0.9f, 0.58f, P_CABECA), /* busto */
    PC(-0.75f, 2.3f, 0.0f, 0.13f, 1.1f, 0.16f, P_CABECA), /* bracos */
    PC(0.75f, 2.3f, 0.0f, 0.13f, 1.1f, 0.16f, P_CABECA),
    PE(0.0f, 3.75f, 0.0f, 0.48f, 0.5f, 0.48f, P_CABECA),
    PE(0.0f, 3.9f, -0.12f, 0.56f, 0.6f, 0.52f, P_CORPO), /* cabelo */
    PB(0.0f, 3.1f, -0.38f, 0.7f, 1.6f, 0.14f, P_CORPO),
    PB(-1.2f, 2.9f, -0.15f, 1.2f, 1.7f, 0.1f, P_CORPO), /* asas */
    PB(1.2f, 2.9f, -0.15f, 1.2f, 1.7f, 0.1f, P_CORPO),
    PE(-0.17f, 3.78f, 0.42f, 0.07f, 0.07f, 0.05f, P_PRETO), /* olhos */
    PE(0.17f, 3.78f, 0.42f, 0.07f, 0.07f, 0.05f, P_PRETO)
};

static const Peca TENTACULO[] = {
    PE(0.0f, 0.1f, 0.0f, 1.2f, 0.55f, 1.2f, P_CORPO), /* base */
    PE(0.0f, 1.0f, 0.0f, 0.78f, 0.95f, 0.78f, P_CORPO),
    PE(0.0f, 2.0f, 0.3f, 0.64f, 0.8f, 0.64f, P_CORPO),
    PE(0.0f, 2.95f, 0.75f, 0.52f, 0.7f, 0.52f, P_CORPO),
    PE(0.0f, 3.8f, 1.35f, 0.42f, 0.6f, 0.42f, P_CORPO),
    PE(0.0f, 4.5f, 2.05f, 0.32f, 0.5f, 0.32f, P_CORPO),
    PE(0.0f, 5.0f, 2.7f, 0.2f, 0.28f, 0.2f, P_CORPO), /* ponta */
    PE(0.0f, 1.0f, 0.72f, 0.2f, 0.2f, 0.2f, P_CABECA), /* ventosas */
    PE(0.0f, 2.0f, 1.0f, 0.18f, 0.18f, 0.18f, P_CABECA),
    PE(0.0f, 2.95f, 1.4f, 0.15f, 0.15f, 0.15f, P_CABECA),
    PE(0.0f, 3.8f, 1.9f, 0.13f, 0.13f, 0.13f, P_CABECA)
};

static const Peca PRETENDENTE[] = {
    PB(-0.32f, 1.0f, 0.0f, 0.45f, 2.0f, 0.5f, P_PRETO), /* pernas */
    PB(0.32f, 1.0f, 0.0f, 0.45f, 2.0f, 0.5f, P_PRETO),
    PB(0.0f, 2.8f, 0.0f, 1.4f, 1.6f, 0.85f, P_CORPO), /* tunica */
    PB(0.0f, 2.05f, 0.0f, 1.45f, 0.18f, 0.9f, P_OURO), /* cinto */
    PB(0.0f, 2.5f, -0.5f, 1.5f, 2.2f, 0.1f, P_CAPA),
    PB(-0.95f, 2.8f, 0.0f, 0.4f, 1.5f, 0.45f, P_CORPO), /* bracos */
    PB(0.95f, 3.0f, 0.35f, 0.4f, 0.4f, 1.1f, P_CORPO),
    PE(0.95f, 3.0f, 0.95f, 0.22f, 0.22f, 0.22f, P_CABECA),
    PB(0.95f, 3.55f, 0.95f, 0.1f, 1.6f, 0.05f, P_METAL), /* espada */
    PB(0.95f, 2.75f, 0.95f, 0.5f, 0.1f, 0.12f, P_OURO),
    PE(0.0f, 4.0f, 0.0f, 0.48f, 0.5f, 0.48f, P_CABECA),
    PE(0.0f, 4.2f, -0.08f, 0.5f, 0.32f, 0.5f, P_PRETO), /* cabelo */
    PE(-0.17f, 4.05f, 0.42f, 0.07f, 0.07f, 0.05f, P_PRETO), /* olhos */
    PE(0.17f, 4.05f, 0.42f, 0.07f, 0.07f, 0.05f, P_PRETO)
};

/* Para uma configuracao ainda sem modelo proprio. */
static const Peca GENERICO[] = {
    PB(0.0f, 1.8f, 0.0f, 1.8f, 3.6f, 1.2f, P_CORPO),
    PE(0.0f, 4.1f, 0.0f, 0.75f, 0.75f, 0.75f, P_CABECA)
};

typedef struct {
    const char *nome;
    const Peca *pecas;
    int quantidade;
} Modelo;

#define MODELO(nome, pecas) { nome, pecas, (int)(sizeof(pecas) / sizeof((pecas)[0])) }

static const Modelo MODELOS[] = {
    MODELO("ciclope_primitivo", CICLOPE),
    MODELO("sereia_primitiva", SEREIA),
    MODELO("tentaculo_primitivo", TENTACULO),
    MODELO("pretendente_primitivo", PRETENDENTE),
    MODELO("", GENERICO)
};

static const Modelo *modeloPeloNome(const char *nome)
{
    const int ultimo = (int)(sizeof MODELOS / sizeof MODELOS[0]) - 1;

    for (int i = 0; i < ultimo; i++) {
        if (nome != NULL && strcmp(nome, MODELOS[i].nome) == 0) {
            return &MODELOS[i];
        }
    }
    return &MODELOS[ultimo];
}

/* Desenha o inimigo de frente para Odisseu, deslocado e inclinado pela pose:
 * investe, recua, abaixa na defesa e tomba ao ser derrotado. */
static void desenharInimigo(const Fase *fase, int indice, const PoseInimigo *pose)
{
    const ConfiguracaoVisualInimigo *visual =
        &fase->configuracao->encontro.inimigos[indice].visual;
    const Modelo *modelo = modeloPeloNome(visual->modelo);
    const Vector3 posicao = posicaoInimigoComPose(fase, indice, pose);
    const Vector3 dir = direcaoParaJogador(fase, fase->pontosInimigos[indice]);
    /* Inclina para o jogador ao investir, para tras ao se preparar, e tomba. */
    const float inclinacao = pose->avanco * 12.0f - pose->preparo * 10.0f -
                             pose->queda * 90.0f;
    const float escala = visual->escala > 0.0f ? visual->escala : 1.0f;
    const float altura = (1.0f - 0.18f * pose->agachar) * escala;
    const float respiracao = (1.0f - pose->queda) *
                             sinf((float)GetTime() * 2.0f + (float)indice) * 0.05f;
    const Color paleta[] = {
        visual->corCorpo, visual->corCabeca, { 240, 238, 230, 255 },
        { 26, 24, 30, 255 }, { 176, 180, 190, 255 }, { 112, 76, 46, 255 },
        { 205, 170, 70, 255 }, { 120, 36, 44, 255 }
    };

    /* Sombra redonda no chao. */
    DrawCylinder(
        (Vector3){ posicao.x, 0.02f, posicao.z },
        1.3f * escala, 1.3f * escala, 0.01f, 16, (Color){ 0, 0, 0, 90 }
    );

    rlPushMatrix();
    rlTranslatef(posicao.x, posicao.y + respiracao, posicao.z);
    rlRotatef(atan2f(dir.x, dir.z) * RAD2DEG, 0.0f, 1.0f, 0.0f);
    rlRotatef(inclinacao, 1.0f, 0.0f, 0.0f);
    rlScalef(escala, altura, escala);

    for (int i = 0; i < modelo->quantidade; i++) {
        const Peca *peca = &modelo->pecas[i];

        desenharForma(
            peca->forma, peca->posicao, peca->tamanho,
            clarear(paleta[peca->cor], pose->brilho)
        );
    }

    rlPopMatrix();
}

static void desenharInimigos(const Fase *fase, const PoseInimigo *poses)
{
    const PoseInimigo repouso = {0};

    if (fase->etapa == ETAPA_EXPLORACAO &&
        fase->configuracao->identidade == IDENTIDADE_POLIFEMO) {
        Fase copia = *fase;
        copia.pontosInimigos[0] = fase->posicaoAmeaca;
        desenharInimigo(&copia, 0, &repouso);
        return;
    }

    if (fase->etapa == ETAPA_EXPLORACAO &&
        fase->configuracao->identidade == IDENTIDADE_ITACA) {
        Fase copia = *fase;
        for (int i = 0; i < 2 && i < fase->quantidadeInimigos; i++) {
            copia.pontosInimigos[i] = fase->patrulhasItaca[i];
            desenharInimigo(&copia, i, &repouso);
        }
        return;
    }

    if (fase->etapa != ETAPA_CONFRONTO) {
        return;
    }

    for (int i = 0; i < fase->quantidadeInimigos; i++) {
        desenharInimigo(fase, i, poses != NULL ? &poses[i] : &repouso);
    }
}

static void desenharMecanicaExploracao3D(const Fase *fase)
{
    if (fase->etapa != ETAPA_EXPLORACAO) {
        return;
    }

    if (fase->configuracao->identidade == IDENTIDADE_CILA &&
        fase->tempoImpactoVisual > 0.0f && fase->indiceEvento >= 0 &&
        fase->indiceEvento < fase->quantidadeItens) {
        const Vector3 p = fase->itens[fase->indiceEvento].posicao;
        const float pulso = 0.65f + sinf((float)GetTime() * 16.0f) * 0.25f;

        DrawCylinder(
            (Vector3){ p.x, 0.03f, p.z }, 2.2f * pulso, 2.2f * pulso,
            0.06f, 24, (Color){ 220, 45, 35, 150 }
        );
        DrawCylinder(
            (Vector3){ p.x, 0.0f, p.z }, 0.22f, 0.08f, 6.0f,
            12, (Color){ 90, 125, 95, 220 }
        );
    }

    if (fase->configuracao->identidade == IDENTIDADE_ITACA &&
        fase->rotaItaca == ROTA_ITACA_NENHUMA) {
        DrawCubeV(
            (Vector3){ -5.0f, 0.04f, 7.0f },
            (Vector3){ 5.0f, 0.08f, 2.0f }, (Color){ 55, 135, 85, 180 }
        );
        DrawCubeV(
            (Vector3){ 5.0f, 0.04f, 7.0f },
            (Vector3){ 5.0f, 0.08f, 2.0f }, (Color){ 170, 75, 55, 180 }
        );
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
    const ConfiguracaoFase *configuracao = fase->configuracao;

    desenharCenario(&fase->cenario);
    for (int i = 0; i < configuracao->quantidadeOrnamentos; i++) {
        const ConfiguracaoOrnamento *o = &configuracao->ornamentos[i];

        desenharForma(o->forma, o->posicao, o->tamanho, o->cor);
    }
    desenharMarcaDeSaida(fase);
    desenharMecanicaExploracao3D(fase);
    desenharInimigos(fase, poses);
    desenharItens(fase); /* por ultimo: o brilho dos itens e translucido */
}

static void desenharBarraMecanica(
    const char *rotulo,
    float valor,
    float maximo,
    Color cor,
    int largura
)
{
    const Rectangle fundo = { 12.0f, 92.0f, 300.0f, 24.0f };
    const float fracao = maximo > 0.0f ? limitar01(valor / maximo) : 0.0f;

    DrawRectangleRec(fundo, (Color){ 10, 12, 18, 210 });
    DrawRectangle(
        (int)fundo.x + 2, (int)fundo.y + 2,
        (int)((fundo.width - 4.0f) * fracao), (int)fundo.height - 4, cor
    );
    DrawRectangleLinesEx(fundo, 2.0f, WHITE);
    desenharTexto(rotulo, 18, 95, 16, WHITE);
    (void)largura;
}

static void desenharHudMecanica(const Fase *fase, int largura)
{
    if (fase->etapa != ETAPA_EXPLORACAO) {
        return;
    }

    switch (fase->configuracao->identidade) {
    case IDENTIDADE_POLIFEMO:
        desenharBarraMecanica(
            "BARULHO: afaste-se de Polifemo",
            fase->medidorPerigo, 100.0f, ORANGE, largura
        );
        break;
    case IDENTIDADE_SEREIAS:
        desenharBarraMecanica(
            "ENCANTAMENTO",
            fase->medidorPerigo, SEREIAS_ENCANTO_MAXIMO,
            (Color){ 180, 75, 210, 255 }, largura
        );
        break;
    case IDENTIDADE_CILA:
        desenharBarraMecanica(
            TextFormat("PROXIMO ATAQUE: %.1fs", fase->tempoEvento),
            fase->tempoEvento, CILA_INTERVALO_ATAQUE,
            (Color){ 65, 170, 115, 255 }, largura
        );
        break;
    case IDENTIDADE_ITACA:
        if (fase->rotaItaca == ROTA_ITACA_NENHUMA) {
            desenharTexto(
                "Escolha: ESQUERDA furtiva | DIREITA direta",
                12, 94, 18, GOLD
            );
        } else {
            desenharBarraMecanica(
                fase->detectadoEmItaca ? "ALERTA: DESCOBERTO" : "SUSPEITA",
                fase->medidorPerigo, ITACA_SUSPEITA_MAXIMA,
                fase->detectadoEmItaca ? RED : YELLOW, largura
            );
        }
        break;
    case IDENTIDADE_NENHUMA:
        break;
    }
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
    desenharHudMecanica(fase, largura);

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

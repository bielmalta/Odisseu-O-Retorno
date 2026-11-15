#include "efeitos_combate.h"

#include <math.h>

#include "raymath.h"
#include "rlgl.h"

#define FOV_NORMAL 70.0f
#define FOV_IMPULSO 4.0f           /* graus a menos no impacto do golpe */
#define AMPLITUDE_TREMOR 0.08f     /* metros */
#define QUEDA_ALTURA 0.9f          /* metros que a camera desce na derrota */
#define QUEDA_INCLINACAO 0.55f     /* radianos de giro lateral na derrota */

/* Base da camera: para onde olha, para a direita e para cima. */
typedef struct {
    Vector3 frente;
    Vector3 direita;
    Vector3 cima;
} BaseCamera;

static BaseCamera baseDaCamera(Camera3D camera)
{
    BaseCamera base;

    base.frente = Vector3Normalize(Vector3Subtract(camera.target, camera.position));
    base.direita = Vector3Normalize(Vector3CrossProduct(base.frente, camera.up));
    base.cima = Vector3CrossProduct(base.direita, base.frente);
    return base;
}

/* Ponto a `f` a frente, `d` a direita e `c` acima da camera. */
static Vector3 pontoNaCamera(Camera3D camera, BaseCamera base, float f, float d, float c)
{
    Vector3 ponto = camera.position;

    ponto = Vector3Add(ponto, Vector3Scale(base.frente, f));
    ponto = Vector3Add(ponto, Vector3Scale(base.direita, d));
    ponto = Vector3Add(ponto, Vector3Scale(base.cima, c));
    return ponto;
}

Camera3D camaraComEfeitos(Camera3D camera, const PoseJogador *pose, float tempo)
{
    const BaseCamera base = baseDaCamera(camera);
    const float amplitude = pose->tremor * AMPLITUDE_TREMOR;
    Vector3 deslocamento = { 0.0f, 0.0f, 0.0f };

    /* Tremor: dois senos de frequencias diferentes, sem aleatoriedade. */
    deslocamento = Vector3Add(
        deslocamento, Vector3Scale(base.direita, sinf(tempo * 57.0f) * amplitude)
    );
    deslocamento = Vector3Add(
        deslocamento, Vector3Scale(base.cima, sinf(tempo * 43.0f + 1.3f) * amplitude * 0.8f)
    );

    /* Derrota: a camera cai e o mundo gira de lado. */
    deslocamento.y -= pose->queda * QUEDA_ALTURA;

    camera.position = Vector3Add(camera.position, deslocamento);
    camera.target = Vector3Add(camera.target, deslocamento);
    camera.target.y -= pose->queda * 0.5f; /* o olhar cai junto */
    camera.up = Vector3RotateByAxisAngle(
        camera.up, base.frente, pose->queda * QUEDA_INCLINACAO
    );

    /* O golpe aperta um pouco o campo de visao no impacto. */
    camera.fovy = FOV_NORMAL - FOV_IMPULSO * (pose->estocada > 0.0f ? pose->estocada : 0.0f);

    return camera;
}

/* ---- Maos, lanca e escudo ---- */

static const Color PELE = { 214, 160, 120, 255 };
static const Color PELE_SOMBRA = { 176, 128, 94, 255 };
static const Color CABO = { 128, 88, 48, 255 };
static const Color PONTA = { 205, 208, 218, 255 };
static const Color ESCUDO_BORDA = { 120, 78, 36, 255 };
static const Color ESCUDO_FACE = { 178, 124, 62, 255 };
static const Color ESCUDO_CENTRO = { 232, 196, 96, 255 };

static void desenharEscudo(Camera3D camera, BaseCamera base, float levantado)
{
    /* Em repouso fica no canto de baixo; levantado, vai para o meio-esquerda
     * sem tapar o inimigo. */
    const Vector3 centro = pontoNaCamera(
        camera, base,
        0.70f + 0.10f * levantado,
        -0.36f + 0.18f * levantado,
        -0.32f + 0.20f * levantado
    );
    const Vector3 frente = Vector3Scale(base.frente, 0.04f);
    const Vector3 punho = pontoNaCamera(camera, base, 0.45f, -0.50f, -0.65f);

    /* Braco que segura o escudo, saindo de baixo da tela. */
    DrawCylinderEx(punho, Vector3Subtract(centro, Vector3Scale(base.frente, 0.08f)),
                   0.07f, 0.05f, 10, PELE_SOMBRA);

    DrawCylinderEx(centro, Vector3Add(centro, frente), 0.21f, 0.21f, 28, ESCUDO_BORDA);
    DrawCylinderEx(Vector3Add(centro, Vector3Scale(base.frente, 0.025f)),
                   Vector3Add(centro, Vector3Scale(base.frente, 0.07f)),
                   0.165f, 0.165f, 28, ESCUDO_FACE);
    DrawSphere(Vector3Add(centro, Vector3Scale(base.frente, 0.08f)), 0.05f, ESCUDO_CENTRO);
}

static void desenharLanca(Camera3D camera, BaseCamera base, float estocada)
{
    const float avanco = estocada > 0.0f ? estocada * 0.55f : estocada * 0.16f;
    const float elevar = estocada < 0.0f ? -estocada * 0.14f : 0.0f;
    const float centrar = estocada > 0.0f ? estocada * 0.22f : 0.0f;
    const float empinar = estocada < 0.0f ? -estocada * 0.25f : 0.0f;
    const Vector3 punho = pontoNaCamera(
        camera, base, 0.62f + avanco, 0.32f - centrar, -0.32f + elevar
    );
    /* Direcao da lanca: para a frente, apontando para baixo do centro da
     * tela em repouso (nao tapa o inimigo) e subindo ao recuar. */
    const Vector3 direcao = Vector3Normalize(Vector3Add(
        Vector3Add(base.frente, Vector3Scale(base.direita, -0.12f)),
        Vector3Scale(base.cima, 0.14f + empinar)
    ));
    const Vector3 ponta = Vector3Add(punho, Vector3Scale(direcao, 0.95f));
    const Vector3 fundo = Vector3Subtract(punho, Vector3Scale(direcao, 0.45f));
    const Vector3 cotovelo = pontoNaCamera(camera, base, 0.30f + avanco * 0.5f, 0.52f, -0.70f);

    /* Antebraco da mao direita, saindo de baixo da tela. */
    DrawCylinderEx(cotovelo, punho, 0.075f, 0.05f, 10, PELE_SOMBRA);

    DrawCylinderEx(fundo, ponta, 0.026f, 0.022f, 10, CABO);
    DrawCylinderEx(ponta, Vector3Add(ponta, Vector3Scale(direcao, 0.30f)),
                   0.06f, 0.0f, 10, PONTA);
    DrawSphere(punho, 0.055f, PELE);
}

void desenharArmasJogador(Camera3D camera, const PoseJogador *pose)
{
    const BaseCamera base = baseDaCamera(camera);

    /* As armas estao sempre na frente de tudo: sem teste de profundidade, para
     * nao atravessarem uma parede quando Odisseu esta encostado nela. */
    rlDrawRenderBatchActive();
    rlDisableDepthTest();

    desenharEscudo(camera, base, pose->escudo);
    desenharLanca(camera, base, pose->estocada);

    rlDrawRenderBatchActive();
    rlEnableDepthTest();
}

/* ---- Efeitos de tela ---- */

/* Escurece as bordas da tela com `cor`, com a opacidade `forca` em [0, 1]. */
static void desenharVinheta(Color cor, float forca, int largura, int altura)
{
    const int menor = largura < altura ? largura : altura;
    const int borda = (int)((float)menor * 0.28f);
    const Color forte = Fade(cor, forca);
    const Color nada = Fade(cor, 0.0f);

    DrawRectangle(0, 0, largura, altura, Fade(cor, forca * 0.12f));
    DrawRectangleGradientV(0, 0, largura, borda, forte, nada);
    DrawRectangleGradientV(0, altura - borda, largura, borda, nada, forte);
    DrawRectangleGradientH(0, 0, borda, altura, forte, nada);
    DrawRectangleGradientH(largura - borda, 0, borda, altura, nada, forte);
}

void desenharEfeitosTela(const PoseJogador *pose, int largura, int altura)
{
    if (pose->lampejoDano > 0.001f) {
        desenharVinheta((Color){ 220, 30, 30, 255 }, pose->lampejoDano * 0.8f, largura, altura);
    }

    if (pose->lampejoDefesa > 0.001f) {
        desenharVinheta((Color){ 70, 140, 255, 255 }, pose->lampejoDefesa * 0.65f, largura, altura);
    }

    if (pose->queda > 0.001f) {
        DrawRectangle(0, 0, largura, altura, Fade((Color){ 50, 0, 0, 255 }, pose->queda * 0.65f));
    }
}

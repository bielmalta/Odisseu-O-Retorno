#include "jogador.h"

#include <math.h>

static Vector3 frenteHorizontal(float yaw) {
    return (Vector3){sinf(yaw), 0.0f, -cosf(yaw)};
}

static Vector3 direitaHorizontal(float yaw) {
    return (Vector3){cosf(yaw), 0.0f, sinf(yaw)};
}

Vector3 direcaoOlharJogador(const Jogador *jogador) {
    float cp = cosf(jogador->pitch);
    return (Vector3){sinf(jogador->yaw) * cp, sinf(jogador->pitch),
                     -cosf(jogador->yaw) * cp};
}

/* Sobreposicao estrita: encostar na face de uma caixa nao conta como colisao. */
static bool sobrepoe(Vector3 pos, BoundingBox caixa) {
    return (pos.x + JOGADOR_RAIO > caixa.min.x) &&
           (pos.x - JOGADOR_RAIO < caixa.max.x) &&
           (pos.z + JOGADOR_RAIO > caixa.min.z) &&
           (pos.z - JOGADOR_RAIO < caixa.max.z) &&
           (pos.y + JOGADOR_ALTURA_CORPO > caixa.min.y) &&
           (pos.y < caixa.max.y);
}

static bool colide(Vector3 pos, const BoundingBox *caixas, int n) {
    for (int i = 0; i < n; i++) {
        if (sobrepoe(pos, caixas[i])) return true;
    }
    return false;
}

/* Altura do chao sob o jogador: o piso (y = 0) ou o topo de uma caixa que
 * estava ao alcance dos pes antes do movimento vertical. */
static float alturaDoChao(Vector3 pos, float yAnterior, const BoundingBox *caixas,
                          int n) {
    float chao = 0.0f;
    for (int i = 0; i < n; i++) {
        const BoundingBox *c = &caixas[i];
        bool sobAreaX = pos.x + JOGADOR_RAIO > c->min.x && pos.x - JOGADOR_RAIO < c->max.x;
        bool sobAreaZ = pos.z + JOGADOR_RAIO > c->min.z && pos.z - JOGADOR_RAIO < c->max.z;
        if (sobAreaX && sobAreaZ && c->max.y <= yAnterior + 0.05f && c->max.y > chao) {
            chao = c->max.y;
        }
    }
    return chao;
}

static void sincronizarCamera(Jogador *jogador) {
    Vector3 olhos = {jogador->posicao.x, jogador->posicao.y + JOGADOR_ALTURA_OLHOS,
                     jogador->posicao.z};
    Vector3 dir = direcaoOlharJogador(jogador);
    jogador->camera.position = olhos;
    jogador->camera.target = (Vector3){olhos.x + dir.x, olhos.y + dir.y, olhos.z + dir.z};
    jogador->camera.up = (Vector3){0.0f, 1.0f, 0.0f};
    jogador->camera.fovy = 70.0f;
    jogador->camera.projection = CAMERA_PERSPECTIVE;
}

void inicializarJogador(Jogador *jogador, Vector3 posicaoInicial, float yaw) {
    jogador->posicao = posicaoInicial;
    jogador->velocidadeVertical = 0.0f;
    jogador->yaw = yaw;
    jogador->pitch = 0.0f;
    jogador->noChao = false;
    jogador->controleAtivo = false;
    jogador->descartarMouse = false;
    sincronizarCamera(jogador);
}

void posicionarJogadorOlhando(Jogador *jogador, Vector3 posicao, Vector3 alvo) {
    jogador->posicao = posicao;
    jogador->velocidadeVertical = 0.0f;

    float dx = alvo.x - posicao.x;
    float dy = alvo.y - (posicao.y + JOGADOR_ALTURA_OLHOS);
    float dz = alvo.z - posicao.z;
    jogador->yaw = atan2f(dx, -dz);
    jogador->pitch = atan2f(dy, sqrtf(dx * dx + dz * dz));
    sincronizarCamera(jogador);
}

void definirControleJogador(Jogador *jogador, bool ativo) {
    if (jogador->controleAtivo == ativo) return;
    jogador->controleAtivo = ativo;
    if (ativo) {
        DisableCursor();
        jogador->descartarMouse = true;
    } else {
        EnableCursor();
    }
}

ComandosJogador lerComandosJogador(void) {
    ComandosJogador c = {0};
    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) c.frente += 1.0f;
    if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) c.frente -= 1.0f;
    if (IsKeyDown(KEY_D)) c.lateral += 1.0f;
    if (IsKeyDown(KEY_A)) c.lateral -= 1.0f;
    Vector2 mouse = GetMouseDelta();
    c.giroX = mouse.x;
    c.giroY = mouse.y;
    c.pular = IsKeyPressed(KEY_SPACE);
    c.correr = IsKeyDown(KEY_LEFT_SHIFT);
    return c;
}

void atualizarJogador(Jogador *jogador, ComandosJogador comandos,
                      const BoundingBox *obstaculos, int quantidade, float dt) {
    /* Controle bloqueado (menu/combate): descarta toda a entrada, mas a
     * gravidade continua valendo para nao deixar o jogador flutuando. */
    if (!jogador->controleAtivo) comandos = (ComandosJogador){0};

    if (jogador->descartarMouse) {
        comandos.giroX = 0.0f;
        comandos.giroY = 0.0f;
        jogador->descartarMouse = false;
    }

    /* Olhar */
    jogador->yaw += comandos.giroX * JOGADOR_SENSIBILIDADE;
    jogador->pitch -= comandos.giroY * JOGADOR_SENSIBILIDADE;
    if (jogador->pitch > JOGADOR_PITCH_MAX) jogador->pitch = JOGADOR_PITCH_MAX;
    if (jogador->pitch < -JOGADOR_PITCH_MAX) jogador->pitch = -JOGADOR_PITCH_MAX;

    /* Movimento horizontal, resolvido por eixo para permitir deslizar nas paredes */
    Vector3 f = frenteHorizontal(jogador->yaw);
    Vector3 d = direitaHorizontal(jogador->yaw);
    Vector3 desejo = {f.x * comandos.frente + d.x * comandos.lateral, 0.0f,
                      f.z * comandos.frente + d.z * comandos.lateral};
    float comp = sqrtf(desejo.x * desejo.x + desejo.z * desejo.z);
    if (comp > 1.0f) {
        desejo.x /= comp;
        desejo.z /= comp;
    }
    float vel = comandos.correr ? JOGADOR_VEL_CORRER : JOGADOR_VEL_ANDAR;

    Vector3 tentativa = jogador->posicao;
    tentativa.x += desejo.x * vel * dt;
    if (!colide(tentativa, obstaculos, quantidade)) jogador->posicao.x = tentativa.x;

    tentativa = jogador->posicao;
    tentativa.z += desejo.z * vel * dt;
    if (!colide(tentativa, obstaculos, quantidade)) jogador->posicao.z = tentativa.z;

    /* Vertical: pulo, gravidade, chao e batida de cabeca */
    if (comandos.pular && jogador->noChao) {
        jogador->velocidadeVertical = JOGADOR_FORCA_PULO;
        jogador->noChao = false;
    }
    jogador->velocidadeVertical -= JOGADOR_GRAVIDADE * dt;

    float yAnterior = jogador->posicao.y;
    Vector3 novo = jogador->posicao;
    novo.y += jogador->velocidadeVertical * dt;

    if (jogador->velocidadeVertical > 0.0f && colide(novo, obstaculos, quantidade)) {
        jogador->velocidadeVertical = 0.0f; /* bateu a cabeca */
        novo.y = yAnterior;
    }

    float chao = alturaDoChao(novo, yAnterior, obstaculos, quantidade);
    if (jogador->velocidadeVertical <= 0.0f && novo.y <= chao) {
        novo.y = chao;
        jogador->velocidadeVertical = 0.0f;
        jogador->noChao = true;
    } else {
        jogador->noChao = false;
    }
    jogador->posicao = novo;

    sincronizarCamera(jogador);
}

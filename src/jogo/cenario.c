#include "cenario.h"

static void adicionarCaixa(Cenario *c, Vector3 centro, Vector3 tamanho) {
    if (c->quantidade >= CENARIO_MAX_CAIXAS) return;
    c->caixas[c->quantidade++] = (BoundingBox){
        {centro.x - tamanho.x / 2, centro.y - tamanho.y / 2, centro.z - tamanho.z / 2},
        {centro.x + tamanho.x / 2, centro.y + tamanho.y / 2, centro.z + tamanho.z / 2}};
}

void criarCenarioTeste(Cenario *c) {
    c->quantidade = 0;
    /* Paredes externas da arena 40x40 */
    adicionarCaixa(c, (Vector3){0, 2, -20.5f}, (Vector3){42, 4, 1});
    adicionarCaixa(c, (Vector3){0, 2, 20.5f}, (Vector3){42, 4, 1});
    adicionarCaixa(c, (Vector3){-20.5f, 2, 0}, (Vector3){1, 4, 42});
    adicionarCaixa(c, (Vector3){20.5f, 2, 0}, (Vector3){1, 4, 42});
    /* Obstaculos */
    adicionarCaixa(c, (Vector3){0, 2, -8}, (Vector3){6, 4, 1});   /* parede central */
    adicionarCaixa(c, (Vector3){-8, 2, 4}, (Vector3){2, 4, 2});   /* pilar */
    adicionarCaixa(c, (Vector3){8, 2, 4}, (Vector3){2, 4, 2});    /* pilar */
    adicionarCaixa(c, (Vector3){6, 0.4f, -2}, (Vector3){2, 0.8f, 2}); /* degrau baixo */
    adicionarCaixa(c, (Vector3){8.5f, 0.8f, -2}, (Vector3){3, 1.6f, 2}); /* plataforma */
}

void desenharCenario(const Cenario *c) {
    DrawPlane((Vector3){0, 0, 0}, (Vector2){40, 40}, (Color){90, 80, 60, 255});
    DrawGrid(40, 1.0f);
    for (int i = 0; i < c->quantidade; i++) {
        BoundingBox b = c->caixas[i];
        Vector3 centro = {(b.min.x + b.max.x) / 2, (b.min.y + b.max.y) / 2,
                          (b.min.z + b.max.z) / 2};
        Vector3 tam = {b.max.x - b.min.x, b.max.y - b.min.y, b.max.z - b.min.z};
        DrawCubeV(centro, tam, (Color){170, 150, 120, 255});
        DrawCubeWiresV(centro, tam, (Color){60, 50, 40, 255});
    }
}

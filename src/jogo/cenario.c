#include "cenario.h"

void limparCenario(Cenario *cenario)
{
    cenario->quantidade = 0;
}

int adicionarCaixaCenario(
    Cenario *cenario, Vector3 centro, Vector3 tamanho, Color cor
)
{
    if (cenario->quantidade >= CENARIO_MAX_CAIXAS) {
        return -1;
    }

    const int indice = cenario->quantidade++;
    cenario->caixas[indice] = (BoundingBox){
        { centro.x - tamanho.x / 2, centro.y - tamanho.y / 2, centro.z - tamanho.z / 2 },
        { centro.x + tamanho.x / 2, centro.y + tamanho.y / 2, centro.z + tamanho.z / 2 }
    };
    cenario->cores[indice] = cor;
    return indice;
}

void removerCaixaCenario(Cenario *cenario, int indice)
{
    if (indice < 0 || indice >= cenario->quantidade) {
        return;
    }

    cenario->caixas[indice] = (BoundingBox){
        { 0.0f, CENARIO_Y_REMOVIDA - 1.0f, 0.0f },
        { 0.0f, CENARIO_Y_REMOVIDA, 0.0f }
    };
}

void desenharCenario(const Cenario *cenario)
{
    DrawPlane((Vector3){ 0, 0, 0 }, (Vector2){ 40, 40 }, (Color){ 90, 80, 60, 255 });
    DrawGrid(40, 1.0f);

    for (int i = 0; i < cenario->quantidade; i++) {
        const BoundingBox b = cenario->caixas[i];
        if (b.max.y <= CENARIO_Y_REMOVIDA) {
            continue; /* caixa removida */
        }

        const Vector3 centro = {
            (b.min.x + b.max.x) / 2, (b.min.y + b.max.y) / 2, (b.min.z + b.max.z) / 2
        };
        const Vector3 tamanho = {
            b.max.x - b.min.x, b.max.y - b.min.y, b.max.z - b.min.z
        };
        DrawCubeV(centro, tamanho, cenario->cores[i]);
        DrawCubeWiresV(centro, tamanho, (Color){ 60, 50, 40, 255 });
    }
}

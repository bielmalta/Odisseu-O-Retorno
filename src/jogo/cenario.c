#include "cenario.h"

#include <stddef.h>

void limparCenario(Cenario *cenario)
{
    cenario->quantidade = 0;
    cenario->corChao = (Color){ 90, 80, 60, 255 };
    cenario->tamanhoChao = (Vector2){ 40.0f, 40.0f };
    cenario->divisoesGrade = 40;
}

void configurarChaoCenario(
    Cenario *cenario,
    Vector2 tamanho,
    Color cor,
    int divisoesGrade
)
{
    if (cenario == NULL) {
        return;
    }

    cenario->tamanhoChao = tamanho;
    cenario->corChao = cor;
    cenario->divisoesGrade = divisoesGrade > 0 ? divisoesGrade : 1;
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

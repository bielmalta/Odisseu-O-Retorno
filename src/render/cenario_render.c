#include "cenario_render.h"

void desenharCenario(const Cenario *cenario)
{
    const float passoGrade = cenario->tamanhoChao.x /
                             (float)cenario->divisoesGrade;

    DrawPlane(
        (Vector3){ 0, 0, 0 }, cenario->tamanhoChao, cenario->corChao
    );
    DrawGrid(cenario->divisoesGrade, passoGrade);

    for (int i = 0; i < cenario->quantidade; i++) {
        const BoundingBox caixa = cenario->caixas[i];
        Vector3 centro;
        Vector3 tamanho;

        if (caixa.max.y <= CENARIO_Y_REMOVIDA) {
            continue;
        }

        centro = (Vector3){
            (caixa.min.x + caixa.max.x) / 2.0f,
            (caixa.min.y + caixa.max.y) / 2.0f,
            (caixa.min.z + caixa.max.z) / 2.0f
        };
        tamanho = (Vector3){
            caixa.max.x - caixa.min.x,
            caixa.max.y - caixa.min.y,
            caixa.max.z - caixa.min.z
        };

        DrawCubeV(centro, tamanho, cenario->cores[i]);
        DrawCubeWiresV(
            centro,
            tamanho,
            (Color){ 60, 50, 40, 255 }
        );
    }
}

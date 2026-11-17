#include "cenario_render.h"

#include "iluminacao.h"
#include "rlgl.h"

void desenharForma(char forma, Vector3 posicao, Vector3 tamanho, Color cor)
{
    if (forma == 'c') {
        DrawCylinder(posicao, tamanho.z, tamanho.x, tamanho.y, 12, cor);
    } else if (forma == 'e') {
        rlPushMatrix();
        rlTranslatef(posicao.x, posicao.y, posicao.z);
        rlScalef(tamanho.x, tamanho.y, tamanho.z);
        DrawSphereEx((Vector3){ 0.0f, 0.0f, 0.0f }, 1.0f, 10, 12, cor);
        rlPopMatrix();
    } else {
        DrawCubeV(posicao, tamanho, cor);
    }
}

void desenharCenario(const Cenario *cenario)
{
    /* Chao e paredes ganham o padrao de blocos de pedra da luz. */
    definirBlocosLuz(true);
    DrawPlane((Vector3){ 0, 0, 0 }, cenario->tamanhoChao, cenario->corChao);

    for (int i = 0; i < cenario->quantidade; i++) {
        const BoundingBox caixa = cenario->caixas[i];

        if (caixa.max.y <= CENARIO_Y_REMOVIDA) {
            continue;
        }

        DrawCubeV(
            (Vector3){
                (caixa.min.x + caixa.max.x) / 2.0f,
                (caixa.min.y + caixa.max.y) / 2.0f,
                (caixa.min.z + caixa.max.z) / 2.0f
            },
            (Vector3){
                caixa.max.x - caixa.min.x,
                caixa.max.y - caixa.min.y,
                caixa.max.z - caixa.min.z
            },
            cenario->cores[i]
        );
    }
    definirBlocosLuz(false);
}

#include "interpolacao.h"

#define PI_F 3.14159265358979323846f

float limitar01(float t)
{
    if (t < 0.0f) {
        return 0.0f;
    }
    if (t > 1.0f) {
        return 1.0f;
    }

    return t;
}

float suavizar(float t)
{
    t = limitar01(t);

    return t * t * (3.0f - 2.0f * t);
}

float interpolar(float a, float b, float t)
{
    return a + (b - a) * t;
}

float diferencaAngulo(float de, float para)
{
    float diferenca = para - de;

    /* Sem libm: o laco roda poucas vezes, pois os angulos ja vem perto. */
    while (diferenca > PI_F) {
        diferenca -= 2.0f * PI_F;
    }
    while (diferenca <= -PI_F) {
        diferenca += 2.0f * PI_F;
    }

    return diferenca;
}

float interpolarAngulo(float de, float para, float t)
{
    return de + diferencaAngulo(de, para) * limitar01(t);
}

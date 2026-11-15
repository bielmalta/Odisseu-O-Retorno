#ifndef FASE_RENDER_H
#define FASE_RENDER_H

#include "animacao_combate.h"
#include "fase.h"

/* Desenho 3D da fase (chamar dentro de BeginMode3D/EndMode3D).
 * `poses` traz a pose de cada inimigo (posicao i = pontosInimigos[i]) durante
 * o combate; NULL desenha todos em repouso. */
void desenharFase(const Fase *fase, const PoseInimigo *poses);

/* Onde o inimigo `indice` esta de fato com a pose aplicada (investida, recuo
 * e queda), para a seta do alvo acompanha-lo. */
Vector3 posicaoInimigoComPose(const Fase *fase, int indice, const PoseInimigo *pose);

/* Mira, objetivo, progresso e aviso de interacao (2D). */
void desenharHudFase(const Fase *fase);

#endif

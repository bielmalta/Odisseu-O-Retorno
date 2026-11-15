#ifndef FASE_RENDER_H
#define FASE_RENDER_H

#include "fase.h"

/* Desenho 3D da fase (chamar dentro de BeginMode3D/EndMode3D). */
void desenharFase(const Fase *fase);

/* Mira, objetivo, progresso e aviso de interacao (2D). */
void desenharHudFase(const Fase *fase);

#endif

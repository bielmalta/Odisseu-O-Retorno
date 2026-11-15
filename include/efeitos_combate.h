#ifndef EFEITOS_COMBATE_H
#define EFEITOS_COMBATE_H

#include "raylib.h"

#include "animacao_combate.h"

/* Efeitos visuais do combate em primeira pessoa (Raylib).
 * Tudo aqui so le a pose calculada por animacao_combate.h: nada decide vida,
 * dano ou turno. */

/* Camera para desenhar o mundo: a camera do jogador com o tremor de quem foi
 * atingido, o tombo da derrota e o impulso do golpe. `tempo` e o relogio do
 * jogo (GetTime()), usado so para o tremor oscilar. A camera da logica nao
 * e alterada. */
Camera3D camaraComEfeitos(Camera3D camera, const PoseJogador *pose, float tempo);

/* Maos, lanca e escudo de Odisseu. Chamar dentro de BeginMode3D() com a
 * camera devolvida por camaraComEfeitos(), depois do cenario. */
void desenharArmasJogador(Camera3D camera, const PoseJogador *pose);

/* Vinheta vermelha (dano), azul (golpe bloqueado) e escurecimento da derrota.
 * Desenho 2D, antes do HUD. */
void desenharEfeitosTela(const PoseJogador *pose, int largura, int altura);

#endif

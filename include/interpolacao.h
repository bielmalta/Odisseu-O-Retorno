#ifndef INTERPOLACAO_H
#define INTERPOLACAO_H

/* Funcoes de suavizacao usadas pelas animacoes e pela camera.
 * Modulo de C puro: nao depende da Raylib. */

/* Limita t ao intervalo [0, 1]. */
float limitar01(float t);

/* Curva "smoothstep": 0 em t <= 0, 1 em t >= 1, com partida e chegada
 * suaves (sem salto de velocidade). */
float suavizar(float t);

/* a + (b - a) * t, sem limitar t. */
float interpolar(float a, float b, float t);

/* Menor diferenca entre dois angulos, em radianos, no intervalo (-pi, pi]. */
float diferencaAngulo(float de, float para);

/* Interpola dois angulos pelo caminho mais curto (t limitado a [0, 1]). */
float interpolarAngulo(float de, float para, float t);

#endif

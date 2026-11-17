#ifndef CENARIO_RENDER_H
#define CENARIO_RENDER_H

#include "cenario.h"

/* Desenha o estado do cenario sem alterar seus dados ou colisoes. */
void desenharCenario(const Cenario *cenario);

/* Uma forma simples: 'c' cilindro ou cone (posicao = centro da base; tamanho =
 * raio da base, altura, raio do topo), 'e' elipsoide (tamanho = os tres raios)
 * ou 'b' caixa (posicao = centro; tamanho = dimensoes). */
void desenharForma(char forma, Vector3 posicao, Vector3 tamanho, Color cor);

#endif

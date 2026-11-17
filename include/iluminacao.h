#ifndef ILUMINACAO_H
#define ILUMINACAO_H

#include <stdbool.h>

#include "raylib.h"

/* Luz e nevoa do mundo 3D (Raylib). Um shader pequeno sombreia cada face pela
 * direcao dela, sem malhas nem texturas, e esmaece o fundo na cor do ceu.
 * Cor com alfa 254 vira brasa (sem sombra); alfa abaixo de 230, vidro ou
 * sombra (sem sombra, translucido). */

void iniciarIluminacao(void); /* depois de InitWindow() */
void encerrarIluminacao(void);

/* Dentro de BeginMode3D(): tudo desenhado ate terminarLuz() recebe a luz. */
void comecarLuz(Camera3D camera, Color nevoa);
void terminarLuz(void);

/* Liga ou desliga o padrao de blocos de pedra (cenario sim, personagens nao). */
void definirBlocosLuz(bool ativo);

#endif

#ifndef PERSONAGEM_H
#define PERSONAGEM_H
#include <stdbool.h>
#define TAMANHO_NOME_PERSONAGEM 32

typedef enum{
    TIPO_HEROI,
    TIPO_INIMIGO
} TipoPersonagem;

typedef struct {
    char nome[TAMANHO_NOME_PERSONAGEM];
    TipoPersonagem tipo;
    int vida;
    int vidaMaxima;
    int ataque;
    int defesa;
    int iniciativa;
    int bonusDefesa; /* reducao temporaria do proximo dano (ver acoes_combate.h) */
    int energia;     /* furia acumulada em combate: paga o golpe giratorio */
    int bonusIniciativa; /* soma a iniciativa na proxima ordenacao */
} Personagem;

Personagem criarPersonagem(
    const char *nome,
    TipoPersonagem tipo,
    int vidaMaxima,
    int ataque,
    int defesa,
    int iniciativa
);

bool personagemVivo(const Personagem *personagem);

int aplicarDano(Personagem *personagem, int dano);

#endif
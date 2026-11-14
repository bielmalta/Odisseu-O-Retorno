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
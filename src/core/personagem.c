#include "personagem.h"
#include <stdio.h>

static int naoNegativo(int valor){
    if (valor < 0){
        return 0;
    }
    return valor;
}

Personagem criarPersonagem(
    const char *nome,
    TipoPersonagem tipo,
    int vidaMaxima,
    int ataque,
    int defesa,
    int iniciativa
){
    Personagem personagem;

    if (nome == NULL){
        nome = "Sem nome";
    }

    snprintf(personagem.nome, sizeof personagem.nome, "%s", nome);
    personagem.tipo = tipo;

    if (vidaMaxima < 1){
        vidaMaxima = 1;
    }

    personagem.vidaMaxima = vidaMaxima;
    personagem.vida = personagem.vidaMaxima;
    personagem.ataque = naoNegativo(ataque);
    personagem.defesa = naoNegativo(defesa);
    personagem.iniciativa = naoNegativo(iniciativa);
    personagem.bonusDefesa = 0;

    return personagem;
}

bool personagemVivo(const Personagem *personagem){
    return personagem != NULL && personagem->vida > 0;
}

int aplicarDano(Personagem *personagem, int dano){
    if (personagem == NULL || dano <= 0){
        return 0;
    }

    if (dano>personagem->vida){
        dano = personagem->vida;
    }

    personagem->vida -= dano;
    return dano;
}
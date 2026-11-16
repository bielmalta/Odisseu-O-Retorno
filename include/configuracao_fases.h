#ifndef CONFIGURACAO_FASES_H
#define CONFIGURACAO_FASES_H

#include <stdbool.h>

#include "objetivo.h"
#include "personagem.h"
#include "raylib.h"

typedef struct ConfiguracaoPersonagem {
    const char *nome;
    TipoPersonagem tipo;
    int vidaMaxima;
    int ataque;
    int defesa;
    int iniciativa;
} ConfiguracaoPersonagem;

typedef struct ConfiguracaoCaixaCenario {
    Vector3 centro;
    Vector3 tamanho;
    Color cor;
    bool portao;
} ConfiguracaoCaixaCenario;

typedef struct ConfiguracaoItemFase {
    Vector3 posicao;
} ConfiguracaoItemFase;

typedef struct ConfiguracaoVisualInimigo {
    const char *modelo;
    Color corCorpo;
    Color corCabeca;
    float escala;
} ConfiguracaoVisualInimigo;

typedef struct ConfiguracaoInimigo {
    ConfiguracaoPersonagem personagem;
    Vector3 posicao;
    ConfiguracaoVisualInimigo visual;
} ConfiguracaoInimigo;

typedef struct ConfiguracaoEncontro {
    const char *nome;
    const ConfiguracaoInimigo *inimigos;
    int quantidadeInimigos;
    Vector3 pontoJogador;
    const char *recompensaNarrativa;
} ConfiguracaoEncontro;

typedef struct ConfiguracaoFase {
    const char *id;
    const char *titulo;
    const char *instrucaoInicial;

    Vector3 pontoInicial;
    BoundingBox saida;

    const ConfiguracaoCaixaCenario *caixas;
    int quantidadeCaixas;
    Color corChao;
    Vector2 tamanhoChao;
    int divisoesGrade;

    const ConfiguracaoItemFase *itens;
    int quantidadeItens;
    Vector3 tamanhoItem;
    Color corItem;
    Color corContornoItem;

    ConfiguracaoObjetivo objetivo;
    ConfiguracaoPersonagem heroi;
    ConfiguracaoEncontro encontro;

    const char *mensagemObjetivoConcluido;
    const char *mensagemFaseConcluida;
    const char *textoObjetivoConfronto;
    const char *textoObjetivoSaida;
} ConfiguracaoFase;

int quantidadeConfiguracoesFase(void);
const ConfiguracaoFase *obterConfiguracaoFase(int indice);

Personagem criarPersonagemConfigurada(
    const ConfiguracaoPersonagem *configuracao
);

#endif

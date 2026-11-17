#ifndef CONFIGURACAO_FASES_H
#define CONFIGURACAO_FASES_H

#include <stdbool.h>

#include "identidade_fase.h"
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

/* Enfeite sem colisao. forma: 'c' cilindro (posicao = centro da base, tamanho =
 * raio da base, altura, raio do topo), 'e' esfera (tamanho = os tres raios) ou
 * 'b' caixa (posicao = centro, tamanho = dimensoes). */
typedef struct ConfiguracaoOrnamento {
    char forma;
    Vector3 posicao;
    Vector3 tamanho;
    Color cor;
} ConfiguracaoOrnamento;

typedef struct ConfiguracaoItemFase {
    Vector3 posicao;
    bool removerAoInteragir;
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
    IdentidadeFase identidade;
    const char *id;
    const char *titulo;
    const char *instrucaoInicial;
    const char *textoNarrativo;

    Vector3 pontoInicial;
    BoundingBox saida;

    const ConfiguracaoCaixaCenario *caixas;
    int quantidadeCaixas;
    Color corChao;
    Vector2 tamanhoChao;
    int divisoesGrade;

    const ConfiguracaoOrnamento *ornamentos;
    int quantidadeOrnamentos;
    Color corCeu; /* fundo e cor da nevoa */

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

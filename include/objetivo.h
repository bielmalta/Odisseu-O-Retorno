#ifndef OBJETIVO_H
#define OBJETIVO_H

#include <stdbool.h>

typedef enum TipoObjetivo {
    OBJETIVO_COLETA,
    OBJETIVO_INTERACAO,
    OBJETIVO_CHEGADA,
    OBJETIVO_COMBATE
} TipoObjetivo;

typedef struct ConfiguracaoObjetivo {
    TipoObjetivo tipo;
    const char *descricao;
    const char *nomeAlvo;
    int quantidadeNecessaria;
} ConfiguracaoObjetivo;

typedef struct Objetivo {
    TipoObjetivo tipo;
    const char *descricao;
    const char *nomeAlvo;
    int progresso;
    int necessario;
    bool iniciado;
    bool cumprido;
} Objetivo;

/* Cria o estado do objetivo a partir de uma configuracao imutavel. */
Objetivo criarObjetivo(const ConfiguracaoObjetivo *configuracao);

/* Reinicia o progresso e ativa o objetivo. */
void iniciarObjetivo(Objetivo *objetivo);

/* Soma progresso sem ultrapassar o necessario. Retorna true somente quando
 * esta chamada acaba de concluir o objetivo. */
bool registrarProgresso(Objetivo *objetivo, int quantidade);

bool verificarObjetivo(const Objetivo *objetivo);

/* Marca o objetivo como cumprido e completa seu contador. */
void concluirObjetivo(Objetivo *objetivo);

#endif

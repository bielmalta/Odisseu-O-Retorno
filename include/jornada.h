#ifndef JORNADA_H
#define JORNADA_H

#include <stdbool.h>

#include "personagem.h"

typedef enum ResultadoAvancoJornada {
    JORNADA_AVANCO_BLOQUEADO,
    JORNADA_PROXIMA_FASE,
    JORNADA_RETORNO_AO_LAR
} ResultadoAvancoJornada;

typedef struct Jornada {
    int indiceEtapaAtual;
    int quantidadeEtapas;
    bool combateVencido;
    bool concluida;
    Personagem odisseu;
} Jornada;

/* Inicia a sequencia e copia o estado persistente de Odisseu. */
bool iniciarJornada(
    Jornada *jornada,
    Personagem odisseu,
    int quantidadeEtapas
);

/* Registra a vitoria exigida para liberar o avanco da etapa atual. */
void registrarVitoriaJornada(Jornada *jornada);

/* Avanca o indice somente depois de objetivo, combate e saida. O carregamento
 * do mapa correspondente fica com a camada que conhece Fase e Raylib. */
ResultadoAvancoJornada avancarJornada(
    Jornada *jornada,
    bool objetivoCumprido,
    bool saidaAlcancada
);

#endif

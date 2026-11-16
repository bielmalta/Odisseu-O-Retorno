#ifndef JORNADA_H
#define JORNADA_H

#include <stdbool.h>

#include "fase.h"
#include "personagem.h"

typedef enum ResultadoAvancoJornada {
    JORNADA_AVANCO_BLOQUEADO,
    JORNADA_PROXIMA_FASE,
    JORNADA_RETORNO_AO_LAR
} ResultadoAvancoJornada;

typedef struct Jornada {
    int indiceEtapaAtual;
    bool combateVencido;
    bool concluida;
    Personagem odisseu;
} Jornada;

/* Inicia a primeira fase e cria o estado persistente de Odisseu. */
bool iniciarJornada(Jornada *jornada, Fase *fase);

/* Registra a vitoria exigida para liberar o avanco da etapa atual. */
void registrarVitoriaJornada(Jornada *jornada);

/* Carrega a proxima fase somente depois de objetivo, combate e saida. */
ResultadoAvancoJornada avancarJornada(Jornada *jornada, Fase *fase);

#endif

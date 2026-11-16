#ifndef NARRATIVA_H
#define NARRATIVA_H

#include <stdbool.h>

typedef enum TipoNarrativa {
    NARRATIVA_INTRODUCAO,
    NARRATIVA_TRANSICAO,
    NARRATIVA_ENCERRAMENTO
} TipoNarrativa;

typedef struct Narrativa {
    TipoNarrativa tipo;
    const char *titulo;
    const char *texto;
    const char *objetivo;
    bool mostrarControles;
} Narrativa;

Narrativa criarNarrativaIntroducao(
    const char *titulo,
    const char *texto,
    const char *objetivo
);

Narrativa criarNarrativaTransicao(
    const char *titulo,
    const char *texto,
    const char *objetivo
);

Narrativa criarNarrativaEncerramento(void);

bool narrativaValida(const Narrativa *narrativa);

#endif

#ifndef IDENTIDADE_FASE_H
#define IDENTIDADE_FASE_H

/* Identifica as regras exclusivas de cada etapa sem acoplar as regras de
 * combate a Raylib ou aos dados de renderizacao. */
typedef enum {
    IDENTIDADE_NENHUMA,
    IDENTIDADE_POLIFEMO,
    IDENTIDADE_SEREIAS,
    IDENTIDADE_CILA,
    IDENTIDADE_ITACA
} IdentidadeFase;

#endif

#ifndef LISTA_TURNOS_H
#define LISTA_TURNOS_H

#include <stdbool.h>

#include "personagem.h"

typedef struct NoTurno {
    Personagem *personagem;
    struct NoTurno *anterior;
    struct NoTurno *proximo;
} NoTurno;

typedef struct {
    NoTurno *inicio;
    NoTurno *atual;
    int tamanho;
} ListaTurnos;

void iniciarLista(ListaTurnos *lista);
bool listaVazia(const ListaTurnos *lista);

bool inserirNaLista(ListaTurnos *lista, Personagem *personagem);

bool removerDaLista(ListaTurnos *lista, const Personagem *personagem);

Personagem *personagemDaVez(const ListaTurnos *lista);
Personagem *avancarTurno(ListaTurnos *lista);
Personagem *voltarTurno(ListaTurnos *lista);

void liberarLista(ListaTurnos *lista);

#endif
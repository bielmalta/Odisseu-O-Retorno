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

bool inserirPersonagem(ListaTurnos *lista, Personagem *personagem);

bool removerPersonagem(ListaTurnos *lista, const Personagem *personagem);

Personagem *buscarPersonagem(const ListaTurnos *lista, const char *nome);

int percorrerParticipantes(const ListaTurnos *lista, Personagem **saida, int capacidade);

Personagem *personagemDaVez(const ListaTurnos *lista);
Personagem *avancarTurno(ListaTurnos *lista);
Personagem *retrocederTurno(ListaTurnos *lista);

void limparListaCombate(ListaTurnos *lista);

bool listaConsistente(const ListaTurnos *lista);

#endif
#include "lista_turnos.h"
#include <stdlib.h>

void iniciarLista(ListaTurnos *lista){
    lista->inicio = NULL;
    lista->atual = NULL;
    lista->tamanho = 0;
}

bool listaVazia(const ListaTurnos *lista){
    return lista->inicio == NULL;
}

bool inserirNaLista(ListaTurnos *lista, Personagem *personagem){
    NoTurno *novo;

    if (personagem == NULL) {
        return false;
    }

    novo = malloc(sizeof *novo);
    if (novo == NULL) {
        return false;
    }
    novo->personagem = personagem;

    if (listaVazia(lista)) {
        novo->proximo = novo;
        novo->anterior = novo;
        lista->inicio = novo;
        lista->atual = novo;
    } else {
        NoTurno *ultimo = lista->inicio->anterior;

        novo->proximo = lista->inicio;
        novo->anterior = ultimo;
        ultimo->proximo = novo;
        lista->inicio->anterior = novo;
    }

    lista->tamanho++;
    return true;
}

void liberarLista(ListaTurnos *lista){
    NoTurno *no = lista->inicio;

    for (int i = 0; i < lista->tamanho; i++) {
        NoTurno *seguinte = no->proximo;
        free(no);
        no = seguinte;
    }

    iniciarLista(lista);
}
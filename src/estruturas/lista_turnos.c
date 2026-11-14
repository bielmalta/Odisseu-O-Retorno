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


static NoTurno *buscarNo(const ListaTurnos *lista, const Personagem *personagem)
{
    NoTurno *no = lista->inicio;

    for (int i = 0; i < lista->tamanho; i++) {
        if (no->personagem == personagem) {
            return no;
        }
        no = no->proximo;
    }

    return NULL;
}

bool removerDaLista(ListaTurnos *lista, const Personagem *personagem)
{
    NoTurno *no = buscarNo(lista, personagem);

    if (no == NULL) {
        return false;
    }

    if (lista->tamanho == 1) {
        free(no);
        iniciarLista(lista);
        return true;
    }

    no->anterior->proximo = no->proximo;
    no->proximo->anterior = no->anterior;

    if (lista->inicio == no) {
        lista->inicio = no->proximo;
    }
    if (lista->atual == no) {
        lista->atual = no->anterior;
    }

    free(no);
    lista->tamanho--;
    return true;
}

Personagem *personagemDaVez(const ListaTurnos *lista){
    return lista->atual != NULL ? lista->atual->personagem : NULL;
}

Personagem *avancarTurno(ListaTurnos *lista){
    if (lista->atual == NULL) {
        return NULL;
    }

    lista->atual = lista->atual->proximo;
    return lista->atual->personagem;
}

Personagem *voltarTurno(ListaTurnos *lista){
    if (lista->atual == NULL) {
        return NULL;
    }

    lista->atual = lista->atual->anterior;
    return lista->atual->personagem;
}
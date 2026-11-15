#include "lista_turnos.h"
#include <stdlib.h>
#include <string.h>

void iniciarLista(ListaTurnos *lista){
    lista->inicio = NULL;
    lista->atual = NULL;
    lista->tamanho = 0;
}

bool listaVazia(const ListaTurnos *lista){
    return lista->inicio == NULL;
}

bool inserirPersonagem(ListaTurnos *lista, Personagem *personagem){
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

void limparListaCombate(ListaTurnos *lista){
    NoTurno *no = lista->inicio;

    for (int i = 0; i < lista->tamanho; i++) {
        NoTurno *seguinte = no->proximo;
        free(no);
        no = seguinte;
    }

    iniciarLista(lista);
}

static NoTurno *buscarNo(const ListaTurnos *lista, const Personagem *personagem){
    NoTurno *no = lista->inicio;

    for (int i = 0; i < lista->tamanho; i++) {
        if (no->personagem == personagem) {
            return no;
        }
        no = no->proximo;
    }

    return NULL;
}

bool removerPersonagem(ListaTurnos *lista, const Personagem *personagem){
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

Personagem *buscarPersonagem(const ListaTurnos *lista, const char *nome){
    NoTurno *no = lista->inicio;

    if (nome == NULL) {
        return NULL;
    }

    for (int i = 0; i < lista->tamanho; i++) {
        if (strcmp(no->personagem->nome, nome) == 0) {
            return no->personagem;
        }
        no = no->proximo;
    }

    return NULL;
}

int percorrerParticipantes(const ListaTurnos *lista, Personagem **saida, int capacidade){
    NoTurno *no = lista->inicio;
    int quantidade = 0;

    for (int i = 0; i < lista->tamanho && quantidade < capacidade; i++) {
        saida[quantidade] = no->personagem;
        quantidade++;
        no = no->proximo;
    }

    return quantidade;
}

Personagem *personagemDaVez(const ListaTurnos *lista){
    if (lista->atual == NULL) {
        return NULL;
    }

    return lista->atual->personagem;
}

Personagem *avancarTurno(ListaTurnos *lista){
    if (lista->atual == NULL) {
        return NULL;
    }

    lista->atual = lista->atual->proximo;
    return lista->atual->personagem;
}

Personagem *retrocederTurno(ListaTurnos *lista){
    if (lista->atual == NULL) {
        return NULL;
    }

    lista->atual = lista->atual->anterior;
    return lista->atual->personagem;
}

bool listaConsistente(const ListaTurnos *lista){
    NoTurno *no = lista->inicio;
    bool achouAtual = false;

    if (listaVazia(lista)) {
        return lista->tamanho == 0 && lista->atual == NULL;
    }

    for (int i = 0; i < lista->tamanho; i++) {
        if (no->proximo->anterior != no || no->anterior->proximo != no) {
            return false;
        }
        if (no == lista->atual) {
            achouAtual = true;
        }
        no = no->proximo;
    }

    return no == lista->inicio && achouAtual;
}
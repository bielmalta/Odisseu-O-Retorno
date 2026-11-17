#include "quicksort.h"

#include <stdlib.h>
#include <string.h>

bool agePrimeiro(const NoTurno *a, const NoTurno *b)
{
    const Personagem *pa = a->personagem;
    const Personagem *pb = b->personagem;
    const int inicA = pa->iniciativa + pa->bonusIniciativa;
    const int inicB = pb->iniciativa + pb->bonusIniciativa;

    if (inicA != inicB) {
        return inicA > inicB;
    }

    return strcmp(pa->nome, pb->nome) < 0;
}

static void trocar(NoTurno **a, NoTurno **b)
{
    NoTurno *auxiliar = *a;

    *a = *b;
    *b = auxiliar;
}

int particionar(NoTurno **nos, int inicio, int fim)
{
    NoTurno *pivo = nos[fim];
    int fronteira = inicio;

    for (int i = inicio; i < fim; i++) {
        if (agePrimeiro(nos[i], pivo)) {
            trocar(&nos[i], &nos[fronteira]);
            fronteira++;
        }
    }

    trocar(&nos[fronteira], &nos[fim]);
    return fronteira;
}

void quickSort(NoTurno **nos, int inicio, int fim)
{
    if (nos == NULL) {
        return;
    }

    /* Recursao so no lado menor; o maior vira repeticao do laco. Assim a
     * pilha cresce no maximo com log2(n), mesmo em entradas ruins. */
    while (inicio < fim) {
        const int pivo = particionar(nos, inicio, fim);

        if (pivo - inicio < fim - pivo) {
            quickSort(nos, inicio, pivo - 1);
            inicio = pivo + 1;
        } else {
            quickSort(nos, pivo + 1, fim);
            fim = pivo - 1;
        }
    }
}

static void religarNaOrdem(ListaTurnos *lista, NoTurno **nos, int quantidade)
{
    for (int i = 0; i < quantidade; i++) {
        nos[i]->proximo = nos[(i + 1) % quantidade];
        nos[i]->anterior = nos[(i - 1 + quantidade) % quantidade];
    }

    lista->inicio = nos[0];
    lista->atual = nos[0];
}

bool ordenarPorVelocidade(ListaTurnos *lista)
{
    NoTurno **nos;
    NoTurno *no;

    if (lista == NULL) {
        return false;
    }

    if (lista->tamanho <= 1) {
        lista->atual = lista->inicio;
        return true;
    }

    nos = malloc((size_t)lista->tamanho * sizeof *nos);
    if (nos == NULL) {
        return false;
    }

    no = lista->inicio;
    for (int i = 0; i < lista->tamanho; i++) {
        nos[i] = no;
        no = no->proximo;
    }

    quickSort(nos, 0, lista->tamanho - 1);
    religarNaOrdem(lista, nos, lista->tamanho);

    free(nos);
    return true;
}

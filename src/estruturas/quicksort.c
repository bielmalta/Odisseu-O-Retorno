#include "quicksort.h"

#include <string.h>

bool agePrimeiro(const NoTurno *a, const NoTurno *b)
{
    const Personagem *pa = a->personagem;
    const Personagem *pb = b->personagem;

    if (pa->iniciativa != pb->iniciativa) {
        return pa->iniciativa > pb->iniciativa;
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


#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "combate.h"

int main(void)
{
    Combate combate;

    Personagem odisseu = criarPersonagem(
        "Odisseu", TIPO_HEROI, 100, 20, 10, 12
    );

    Personagem ciclope = criarPersonagem(
        "Ciclope", TIPO_INIMIGO, 120, 25, 8, 8
    );

    Personagem sereia = criarPersonagem(
        "Sereia", TIPO_INIMIGO, 60, 15, 5, 15
    );

    Personagem *inimigos[] = {&ciclope, &sereia};

    const char *ordemEsperada[] = {
        "Sereia", "Odisseu", "Ciclope"
    };

    int acoesRegistradas = 0;

    inicializarCombate(&combate);

    assert(prepararCombate(&combate, &odisseu, inimigos, 2));
    assert(combate.turnos.tamanho == 3);
    assert(listaConsistente(&combate.turnos));

    /* Executa exatamente duas rodadas completas. */
    for (int passo = 0;
         passo < 100 && acoesRegistradas < 6;
         passo++) {

        if (combate.estado == COMBATE_WAITING_PLAYER) {
            concluirAcaoCombate(&combate);
        } else {
            atualizarCombate(&combate);
        }

        /* Cada entrada em RESOLVING representa uma acao concluida. */
        if (combate.estado == COMBATE_RESOLVING) {
            int indice = acoesRegistradas % 3;

            assert(combate.participanteAtual != NULL);

            assert(strcmp(
                combate.participanteAtual->nome,
                ordemEsperada[indice]
            ) == 0);

            assert(combate.rodada ==
                   (unsigned int)(acoesRegistradas / 3 + 1));

            acoesRegistradas++;
        }
    }

    assert(acoesRegistradas == 6);
    assert(combate.rodada == 2);
    assert(combate.turnosConcluidos == 3);
    assert(listaConsistente(&combate.turnos));

    finalizarCombate(&combate);

    assert(listaVazia(&combate.turnos));
    assert(combate.turnos.tamanho == 0);

    printf("\nTESTE DO COMBATE PASSOU!\n");

    return 0;
}

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "acoes_combate.h"
#include "combate.h"

static void testarVitoriaERemocao(void)
{
    Combate combate;
    Personagem odisseu = criarPersonagem(
        "Odisseu", TIPO_HEROI, 100, 100, 10, 20
    );
    Personagem ciclope = criarPersonagem(
        "Ciclope", TIPO_INIMIGO, 30, 25, 8, 8
    );
    Personagem *inimigos[] = {&ciclope};

    inicializarCombate(&combate);
    assert(prepararCombate(&combate, &odisseu, inimigos, 1));
    atualizarCombate(&combate);
    assert(combate.estado == COMBATE_WAITING_PLAYER);

    assert(atacarNoCombate(&combate, &ciclope));

    assert(combate.estado == COMBATE_VICTORY);
    assert(combate.participanteAtual == NULL);
    assert(combate.turnos.tamanho == 1);
    assert(buscarPersonagem(&combate.turnos, "Ciclope") == NULL);
    assert(listaConsistente(&combate.turnos));

    finalizarCombate(&combate);
}

static void testarDerrotaImediata(void)
{
    Combate combate;
    Personagem odisseu = criarPersonagem(
        "Odisseu", TIPO_HEROI, 20, 20, 0, 8
    );
    Personagem ciclope = criarPersonagem(
        "Ciclope", TIPO_INIMIGO, 100, 50, 0, 20
    );
    Personagem *inimigos[] = {&ciclope};

    inicializarCombate(&combate);
    assert(prepararCombate(&combate, &odisseu, inimigos, 1));
    atualizarCombate(&combate);
    assert(combate.estado == COMBATE_ENEMY_TURN);

    assert(atacarNoCombate(&combate, &odisseu));

    assert(combate.estado == COMBATE_DEFEAT);
    assert(combate.participanteAtual == NULL);
    assert(combate.turnos.tamanho == 2);
    assert(listaConsistente(&combate.turnos));

    finalizarCombate(&combate);
}

static void testarRemocaoDoParticipanteAtual(void)
{
    Combate combate;
    Personagem odisseu = criarPersonagem(
        "Odisseu", TIPO_HEROI, 100, 20, 10, 15
    );
    Personagem sereia = criarPersonagem(
        "Sereia", TIPO_INIMIGO, 40, 15, 5, 20
    );
    Personagem ciclope = criarPersonagem(
        "Ciclope", TIPO_INIMIGO, 100, 25, 8, 8
    );
    Personagem *inimigos[] = {&sereia, &ciclope};

    inicializarCombate(&combate);
    assert(prepararCombate(&combate, &odisseu, inimigos, 2));
    atualizarCombate(&combate);
    assert(combate.participanteAtual == &sereia);

    sereia.vida = 0;
    atualizarCombate(&combate);

    assert(combate.turnos.tamanho == 2);
    assert(buscarPersonagem(&combate.turnos, "Sereia") == NULL);
    assert(listaConsistente(&combate.turnos));
    assert(combate.estado == COMBATE_WAITING_PLAYER);
    assert(combate.participanteAtual == &odisseu);

    finalizarCombate(&combate);
}

static void testarRemocaoSemPularTurno(void)
{
    Combate combate;
    Personagem odisseu = criarPersonagem(
        "Odisseu", TIPO_HEROI, 100, 100, 10, 15
    );
    Personagem sereia = criarPersonagem(
        "Sereia", TIPO_INIMIGO, 40, 15, 5, 20
    );
    Personagem ciclope = criarPersonagem(
        "Ciclope", TIPO_INIMIGO, 100, 25, 8, 8
    );
    Personagem *inimigos[] = {&sereia, &ciclope};

    inicializarCombate(&combate);
    assert(prepararCombate(&combate, &odisseu, inimigos, 2));

    atualizarCombate(&combate); /* Sereia inicia. */
    atualizarCombate(&combate); /* Sereia conclui automaticamente. */
    atualizarCombate(&combate); /* Avanca para Odisseu. */
    assert(combate.estado == COMBATE_WAITING_PLAYER);
    assert(combate.turnosConcluidos == 1);

    assert(atacarNoCombate(&combate, &sereia));
    assert(combate.turnos.tamanho == 2);
    assert(combate.turnosConcluidos == 1);

    atualizarCombate(&combate);
    assert(combate.estado == COMBATE_ENEMY_TURN);
    assert(combate.participanteAtual == &ciclope);

    finalizarCombate(&combate);
}

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

    testarVitoriaERemocao();
    testarDerrotaImediata();
    testarRemocaoDoParticipanteAtual();
    testarRemocaoSemPularTurno();

    printf("\nTESTE DO COMBATE PASSOU!\n");

    return 0;
}

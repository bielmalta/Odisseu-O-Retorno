#include <assert.h>
#include <stdio.h>

#include "jornada.h"

static void testarPreCondicoesDoAvanco(void)
{
    Jornada jornada;
    Personagem odisseu = criarPersonagem(
        "Odisseu", TIPO_HEROI, 100, 20, 10, 12
    );

    assert(iniciarJornada(&jornada, odisseu, 4));
    assert(jornada.indiceEtapaAtual == 0);
    assert(!jornada.combateVencido);
    assert(!jornada.concluida);
    assert(jornada.odisseu.vida == jornada.odisseu.vidaMaxima);

    assert(avancarJornada(&jornada, false, false) ==
           JORNADA_AVANCO_BLOQUEADO);
    assert(avancarJornada(&jornada, true, false) ==
           JORNADA_AVANCO_BLOQUEADO);

    registrarVitoriaJornada(&jornada);
    assert(avancarJornada(&jornada, false, true) ==
           JORNADA_AVANCO_BLOQUEADO);
    assert(avancarJornada(&jornada, true, false) ==
           JORNADA_AVANCO_BLOQUEADO);
}

static void testarConclusaoDaUltimaEtapa(void)
{
    Jornada jornada;
    Personagem odisseu = criarPersonagem(
        "Odisseu", TIPO_HEROI, 100, 20, 10, 12
    );
    int ataquePersistente;

    assert(iniciarJornada(&jornada, odisseu, 4));
    jornada.odisseu.vida = 37;
    jornada.odisseu.ataque += 4;
    ataquePersistente = jornada.odisseu.ataque;

    for (int indice = 0; indice < jornada.quantidadeEtapas; indice++) {
        const bool ultima = indice == jornada.quantidadeEtapas - 1;

        assert(jornada.indiceEtapaAtual == indice);

        registrarVitoriaJornada(&jornada);

        assert(avancarJornada(&jornada, true, true) ==
               (ultima ? JORNADA_RETORNO_AO_LAR
                       : JORNADA_PROXIMA_FASE));

        assert(jornada.odisseu.vida == 37);
        assert(jornada.odisseu.ataque == ataquePersistente);
        if (!ultima) {
            assert(!jornada.combateVencido);
        }
    }

    assert(jornada.concluida);
    assert(jornada.indiceEtapaAtual == jornada.quantidadeEtapas - 1);
    assert(jornada.odisseu.vida == 37);
    assert(jornada.odisseu.ataque == ataquePersistente);
    assert(avancarJornada(&jornada, true, true) ==
           JORNADA_AVANCO_BLOQUEADO);
}

static void testarEntradasInvalidas(void)
{
    Jornada jornada;
    Personagem odisseu = criarPersonagem(
        "Odisseu", TIPO_HEROI, 100, 20, 10, 12
    );
    Personagem inimigo = criarPersonagem(
        "Polifemo", TIPO_INIMIGO, 100, 20, 10, 8
    );

    assert(!iniciarJornada(NULL, odisseu, 4));
    assert(!iniciarJornada(&jornada, odisseu, 0));
    assert(!iniciarJornada(&jornada, inimigo, 4));
    assert(avancarJornada(NULL, true, true) == JORNADA_AVANCO_BLOQUEADO);
    registrarVitoriaJornada(NULL);
}

int main(void)
{
    testarPreCondicoesDoAvanco();
    testarConclusaoDaUltimaEtapa();
    testarEntradasInvalidas();

    puts("Testes da progressao da jornada passaram.");
    return 0;
}

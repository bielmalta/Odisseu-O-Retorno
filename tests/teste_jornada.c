#include <assert.h>
#include <stdio.h>

#include "jornada.h"

static void testarPreCondicoesDoAvanco(void)
{
    Jornada jornada;
    Fase fase;

    assert(iniciarJornada(&jornada, &fase));
    assert(jornada.indiceEtapaAtual == 0);
    assert(!jornada.combateVencido);
    assert(!jornada.concluida);
    assert(jornada.odisseu.vida == jornada.odisseu.vidaMaxima);

    assert(avancarJornada(&jornada, &fase) == JORNADA_AVANCO_BLOQUEADO);

    concluirObjetivo(&fase.objetivo);
    fase.etapa = ETAPA_CONFRONTO;
    assert(avancarJornada(&jornada, &fase) == JORNADA_AVANCO_BLOQUEADO);

    registrarVitoriaJornada(&jornada);
    assert(avancarJornada(&jornada, &fase) == JORNADA_AVANCO_BLOQUEADO);

    fase.etapa = ETAPA_SAIDA_LIBERADA;
    assert(avancarJornada(&jornada, &fase) == JORNADA_AVANCO_BLOQUEADO);
}

static void testarConclusaoDaUltimaEtapa(void)
{
    Jornada jornada;
    Fase fase;
    int ataquePersistente;

    assert(iniciarJornada(&jornada, &fase));
    jornada.odisseu.vida = 37;
    jornada.odisseu.ataque += 4;
    ataquePersistente = jornada.odisseu.ataque;

    for (int indice = 0; indice < quantidadeConfiguracoesFase(); indice++) {
        const bool ultima = indice == quantidadeConfiguracoesFase() - 1;

        assert(jornada.indiceEtapaAtual == indice);
        assert(fase.configuracao == obterConfiguracaoFase(indice));

        concluirObjetivo(&fase.objetivo);
        registrarVitoriaJornada(&jornada);
        fase.etapa = ETAPA_CONCLUIDA;

        assert(avancarJornada(&jornada, &fase) ==
               (ultima ? JORNADA_RETORNO_AO_LAR
                       : JORNADA_PROXIMA_FASE));

        assert(jornada.odisseu.vida == 37);
        assert(jornada.odisseu.ataque == ataquePersistente);
        if (!ultima) {
            assert(!jornada.combateVencido);
            assert(fase.etapa == ETAPA_EXPLORACAO);
            assert(!verificarObjetivo(&fase.objetivo));
        }
    }

    assert(jornada.concluida);
    assert(jornada.indiceEtapaAtual == quantidadeConfiguracoesFase() - 1);
    assert(jornada.odisseu.vida == 37);
    assert(jornada.odisseu.ataque == ataquePersistente);
    assert(avancarJornada(&jornada, &fase) == JORNADA_AVANCO_BLOQUEADO);
}

static void testarEntradasInvalidas(void)
{
    Jornada jornada;
    Fase fase;

    assert(!iniciarJornada(NULL, &fase));
    assert(!iniciarJornada(&jornada, NULL));
    assert(avancarJornada(NULL, &fase) == JORNADA_AVANCO_BLOQUEADO);
    assert(avancarJornada(&jornada, NULL) == JORNADA_AVANCO_BLOQUEADO);
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

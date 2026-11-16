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

    concluirObjetivo(&fase.objetivo);
    registrarVitoriaJornada(&jornada);
    fase.etapa = ETAPA_CONCLUIDA;

    assert(avancarJornada(&jornada, &fase) == JORNADA_RETORNO_AO_LAR);
    assert(jornada.concluida);
    assert(jornada.indiceEtapaAtual == 0);
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

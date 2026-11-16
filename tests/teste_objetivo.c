#include <assert.h>
#include <stdio.h>

#include "objetivo.h"

static void testarCicloDoObjetivo(void)
{
    const ConfiguracaoObjetivo configuracao = {
        OBJETIVO_COLETA,
        "Colete os fragmentos",
        "fragmento",
        3
    };
    Objetivo objetivo = criarObjetivo(&configuracao);

    assert(objetivo.tipo == OBJETIVO_COLETA);
    assert(objetivo.descricao == configuracao.descricao);
    assert(objetivo.nomeAlvo == configuracao.nomeAlvo);
    assert(objetivo.necessario == 3);
    assert(!objetivo.iniciado);
    assert(!verificarObjetivo(&objetivo));

    iniciarObjetivo(&objetivo);
    assert(objetivo.iniciado);
    assert(objetivo.progresso == 0);

    assert(!registrarProgresso(&objetivo, 1));
    assert(objetivo.progresso == 1);
    assert(!verificarObjetivo(&objetivo));

    assert(registrarProgresso(&objetivo, 5));
    assert(objetivo.progresso == 3);
    assert(verificarObjetivo(&objetivo));

    assert(!registrarProgresso(&objetivo, 1));
    assert(objetivo.progresso == 3);

    iniciarObjetivo(&objetivo);
    assert(objetivo.progresso == 0);
    assert(!objetivo.cumprido);

    concluirObjetivo(&objetivo);
    assert(verificarObjetivo(&objetivo));
    assert(objetivo.progresso == objetivo.necessario);
}

static void testarEntradasInvalidas(void)
{
    Objetivo vazio = criarObjetivo(NULL);
    const ConfiguracaoObjetivo semQuantidade = {
        OBJETIVO_INTERACAO,
        "Ative o mecanismo",
        "mecanismo",
        0
    };
    Objetivo objetivo = criarObjetivo(&semQuantidade);

    assert(!vazio.iniciado);
    assert(!verificarObjetivo(&vazio));
    assert(!registrarProgresso(&vazio, 1));

    iniciarObjetivo(&objetivo);
    assert(objetivo.necessario == 1);
    assert(!registrarProgresso(&objetivo, 0));
    assert(registrarProgresso(&objetivo, 1));

    iniciarObjetivo(NULL);
    concluirObjetivo(NULL);
    assert(!registrarProgresso(NULL, 1));
    assert(!verificarObjetivo(NULL));
}

int main(void)
{
    testarCicloDoObjetivo();
    testarEntradasInvalidas();

    puts("Testes do sistema de objetivos passaram.");
    return 0;
}

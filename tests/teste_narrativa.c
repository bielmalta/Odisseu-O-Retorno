#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "narrativa.h"

static void testarIntroducao(void)
{
    Narrativa narrativa = criarNarrativaIntroducao(
        "A Caverna de Polifemo",
        "Odisseu inicia sua jornada.",
        "Prepare a estaca"
    );

    assert(narrativaValida(&narrativa));
    assert(narrativa.tipo == NARRATIVA_INTRODUCAO);
    assert(narrativa.mostrarControles);
    assert(strcmp(narrativa.titulo, "A Caverna de Polifemo") == 0);
    assert(strcmp(narrativa.objetivo, "Prepare a estaca") == 0);
}

static void testarTransicao(void)
{
    Narrativa narrativa = criarNarrativaTransicao(
        "O Canto das Sereias",
        "O navio se aproxima das Sereias.",
        "Proteja o conves"
    );

    assert(narrativaValida(&narrativa));
    assert(narrativa.tipo == NARRATIVA_TRANSICAO);
    assert(!narrativa.mostrarControles);
    assert(strcmp(narrativa.objetivo, "Proteja o conves") == 0);
}

static void testarEncerramento(void)
{
    Narrativa narrativa = criarNarrativaEncerramento();

    assert(narrativaValida(&narrativa));
    assert(narrativa.tipo == NARRATIVA_ENCERRAMENTO);
    assert(!narrativa.mostrarControles);
    assert(strstr(narrativa.texto, "Penelope") != NULL);
    assert(strcmp(narrativa.objetivo, "Jornada concluida") == 0);
}

static void testarEntradasInvalidas(void)
{
    Narrativa vazia = criarNarrativaIntroducao(NULL, NULL, NULL);

    assert(!narrativaValida(NULL));
    assert(!narrativaValida(&vazia));
    assert(vazia.titulo != NULL);
    assert(vazia.texto != NULL);
    assert(vazia.objetivo != NULL);
}

int main(void)
{
    testarIntroducao();
    testarTransicao();
    testarEncerramento();
    testarEntradasInvalidas();

    puts("Testes das sequencias narrativas passaram.");
    return 0;
}

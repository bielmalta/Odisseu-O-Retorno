#include "narrativa.h"

#include <stddef.h>

static Narrativa criarNarrativa(
    TipoNarrativa tipo,
    const char *titulo,
    const char *texto,
    const char *objetivo,
    bool mostrarControles
)
{
    return (Narrativa){
        tipo,
        titulo != NULL ? titulo : "",
        texto != NULL ? texto : "",
        objetivo != NULL ? objetivo : "",
        mostrarControles
    };
}

Narrativa criarNarrativaIntroducao(
    const char *titulo,
    const char *texto,
    const char *objetivo
)
{
    return criarNarrativa(
        NARRATIVA_INTRODUCAO, titulo, texto, objetivo, true
    );
}

Narrativa criarNarrativaTransicao(
    const char *titulo,
    const char *texto,
    const char *objetivo
)
{
    return criarNarrativa(
        NARRATIVA_TRANSICAO, titulo, texto, objetivo, false
    );
}

Narrativa criarNarrativaEncerramento(void)
{
    return criarNarrativa(
        NARRATIVA_ENCERRAMENTO,
        "Odisseu retornou ao lar",
        "Os pretendentes foram derrotados e o grande salao voltou ao silencio.\n"
        "Depois de enfrentar monstros, tempestades e o mar, Odisseu reencontra\n"
        "Penelope e conclui sua longa jornada de volta a Itaca.",
        "Jornada concluida",
        false
    );
}

bool narrativaValida(const Narrativa *narrativa)
{
    return narrativa != NULL && narrativa->titulo != NULL &&
           narrativa->titulo[0] != '\0' && narrativa->texto != NULL &&
           narrativa->texto[0] != '\0';
}

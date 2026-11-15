/* Cenarios completos de combate: vitoria, derrota, remocao no turno atual,
 * varias rodadas e velocidades iguais ou diferentes.
 *
 * Cada luta roda passo a passo por um executor que, depois de cada passo,
 * confere as invariantes do combate:
 *   - a lista circular continua consistente;
 *   - nenhum inimigo derrotado permanece na ordem de turnos;
 *   - o participante da vez, quando existe, esta na lista;
 *   - ninguem age duas vezes na mesma rodada;
 *   - o numero de passos tem limite (sem laco infinito). */

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "acoes_combate.h"
#include "combate.h"
#include "lista_turnos.h"
#include "personagem.h"

#define QUANTIDADE(vetor) ((int)(sizeof(vetor) / sizeof((vetor)[0])))

enum {
    MAX_PARTICIPANTES = 6,
    MAX_RODADAS = 32,
    MAX_ACOES = 128,
    LIMITE_PASSOS = 1000
};

/* O que Odisseu faz quando e a vez dele. */
typedef enum {
    JOGADOR_ATACA,   /* ataca o primeiro inimigo vivo da ordem de turnos */
    JOGADOR_DEFENDE
} PoliticaJogador;

typedef struct {
    Combate combate;
    Personagem *todos[MAX_PARTICIPANTES]; /* [0] e o jogador */
    int quantidade;
    PoliticaJogador politica;
    int passos;
    int acoes[MAX_RODADAS][MAX_PARTICIPANTES]; /* acoes por rodada */
    int sequencia[MAX_ACOES];                  /* quem agiu, em ordem */
    int totalAcoes;
    int rodadaDaMorte[MAX_PARTICIPANTES];      /* 0 = nao foi derrotado */
} Cenario;

static Cenario cenario;

static int indiceDe(const Cenario *c, const Personagem *personagem)
{
    for (int i = 0; i < c->quantidade; i++) {
        if (c->todos[i] == personagem) {
            return i;
        }
    }

    return -1;
}

static bool estaNaLista(const Combate *combate, const Personagem *personagem)
{
    const NoTurno *no = combate->turnos.inicio;

    for (int i = 0; i < combate->turnos.tamanho; i++) {
        if (no->personagem == personagem) {
            return true;
        }
        no = no->proximo;
    }

    return false;
}

static bool combateEncerrado(const Combate *combate)
{
    return combate->estado == COMBATE_VICTORY ||
           combate->estado == COMBATE_DEFEAT;
}

/* Recebe cada acao realizada e guarda quem agiu e em que rodada. */
static void observar(const EventoCombate *evento, void *contexto)
{
    Cenario *c = contexto;
    const int agente = indiceDe(c, evento->origem);
    const unsigned int rodada = c->combate.rodada;

    assert(agente >= 0);
    assert(rodada >= 1 && rodada < MAX_RODADAS);
    assert(c->totalAcoes < MAX_ACOES);

    /* So age quem esta com a vez, e vivo. */
    assert(c->combate.participanteAtual == evento->origem);
    assert(personagemVivo(evento->origem));

    c->acoes[rodada][agente]++;
    assert(c->acoes[rodada][agente] == 1); /* no maximo uma vez por rodada */
    c->sequencia[c->totalAcoes++] = agente;

    if (evento->tipo == EVENTO_COMBATE_ATAQUE && evento->alvoDerrotado) {
        const int morto = indiceDe(c, evento->alvo);

        assert(morto >= 0);
        c->rodadaDaMorte[morto] = (int)rodada;
    }
}

static void conferirInvariantes(const Cenario *c)
{
    const Combate *combate = &c->combate;
    const NoTurno *no = combate->turnos.inicio;

    assert(listaConsistente(&combate->turnos));
    assert(combate->turnos.tamanho >= 1);
    assert(combate->turnos.tamanho <= c->quantidade);

    /* Odisseu nunca sai da lista, mesmo derrotado. */
    assert(estaNaLista(combate, combate->jogador));

    if (combate->participanteAtual != NULL) {
        assert(estaNaLista(combate, combate->participanteAtual));
    }

    for (int i = 0; i < combate->turnos.tamanho; i++) {
        if (no->personagem->tipo == TIPO_INIMIGO) {
            assert(personagemVivo(no->personagem));
        }
        no = no->proximo;
    }

    assert(combate->turnosConcluidos >= 0);
    assert(combate->turnosConcluidos <= combate->turnos.tamanho);
}

static Personagem *primeiroInimigoVivo(const Cenario *c)
{
    const NoTurno *no = c->combate.turnos.inicio;

    for (int i = 0; i < c->combate.turnos.tamanho; i++) {
        if (no->personagem->tipo == TIPO_INIMIGO &&
            personagemVivo(no->personagem)) {
            return no->personagem;
        }
        no = no->proximo;
    }

    return NULL;
}

/* Um passo do jogo: Odisseu age na vez dele, o resto anda pelo controlador. */
static void darPasso(Cenario *c)
{
    Combate *combate = &c->combate;

    c->passos++;
    assert(c->passos < LIMITE_PASSOS); /* sem laco infinito */

    if (combate->estado == COMBATE_WAITING_PLAYER) {
        bool agiu;

        if (c->politica == JOGADOR_DEFENDE) {
            agiu = defenderNoCombate(combate);
        } else {
            Personagem *alvo = primeiroInimigoVivo(c);

            assert(alvo != NULL);
            agiu = atacarNoCombate(combate, alvo);
        }

        assert(agiu);
        (void)agiu;
    } else {
        atualizarCombate(combate);
    }

    conferirInvariantes(c);
}

static void iniciarCenario(
    Cenario *c,
    Personagem *jogador,
    Personagem **inimigos,
    int quantidadeInimigos,
    PoliticaJogador politica
)
{
    bool preparado;

    assert(quantidadeInimigos + 1 <= MAX_PARTICIPANTES);

    memset(c, 0, sizeof *c);
    c->todos[0] = jogador;
    for (int i = 0; i < quantidadeInimigos; i++) {
        c->todos[i + 1] = inimigos[i];
    }
    c->quantidade = quantidadeInimigos + 1;
    c->politica = politica;

    inicializarCombate(&c->combate);
    preparado = prepararCombate(
        &c->combate, jogador, inimigos, quantidadeInimigos
    );
    assert(preparado);
    (void)preparado;

    registrarObservadorCombate(observar, c);
    conferirInvariantes(c);
}

static void encerrarCenario(Cenario *c)
{
    registrarObservadorCombate(NULL, NULL);
    finalizarCombate(&c->combate);

    assert(listaVazia(&c->combate.turnos));
    assert(c->combate.turnos.tamanho == 0);
}

/* Roda ate haver `quantidade` acoes registradas, ou ate o combate acabar. */
static void executarAcoes(Cenario *c, int quantidade)
{
    while (c->totalAcoes < quantidade && !combateEncerrado(&c->combate)) {
        darPasso(c);
    }
}

static void executarAteOFim(Cenario *c)
{
    while (!combateEncerrado(&c->combate)) {
        darPasso(c);
    }
}

/* Para logo antes de `personagem` agir no turno inimigo. */
static void avancarAteAVezDe(Cenario *c, const Personagem *personagem)
{
    while (!combateEncerrado(&c->combate) &&
           !(c->combate.participanteAtual == personagem &&
             c->combate.estado == COMBATE_ENEMY_TURN)) {
        darPasso(c);
    }

    assert(c->combate.participanteAtual == personagem);
    assert(c->combate.estado == COMBATE_ENEMY_TURN);
}

/* Simula um dano vindo de fora do combate (armadilha, evento da fase). */
static void derrotarDeFora(Cenario *c, Personagem *personagem)
{
    const int indice = indiceDe(c, personagem);

    assert(indice > 0);
    personagem->vida = 0;
    c->rodadaDaMorte[indice] = (int)c->combate.rodada;
}

static void conferirSequencia(
    const Cenario *c,
    const char *const *esperada,
    int quantidade
)
{
    bool igual = c->totalAcoes == quantidade;

    for (int i = 0; igual && i < quantidade; i++) {
        igual = strcmp(c->todos[c->sequencia[i]]->nome, esperada[i]) == 0;
    }

    if (!igual) {
        fprintf(stderr, "Sequencia de acoes inesperada. Obtida:");
        for (int i = 0; i < c->totalAcoes; i++) {
            fprintf(stderr, " %s", c->todos[c->sequencia[i]]->nome);
        }
        fprintf(stderr, "\n");
    }

    assert(igual);
}

/* A mesma ordem de rodada repetida `repeticoes` vezes. */
static void conferirOrdemRepetida(
    const Cenario *c,
    const char *const *ordem,
    int quantidade,
    int repeticoes
)
{
    const char *esperada[MAX_ACOES];
    const int total = quantidade * repeticoes;

    assert(total <= MAX_ACOES);
    for (int i = 0; i < total; i++) {
        esperada[i] = ordem[i % quantidade];
    }

    conferirSequencia(c, esperada, total);
}

/* Em toda rodada que terminou, quem estava vivo agiu exatamente uma vez;
 * quem ja tinha sido derrotado nao agiu; na rodada da morte, no maximo uma
 * vez. A rodada em andamento so pode ter no maximo uma acao por pessoa. */
static void conferirAcoesPorRodada(const Cenario *c)
{
    const int ultima = (int)c->combate.rodada;

    assert(ultima < MAX_RODADAS);

    for (int rodada = 1; rodada <= ultima; rodada++) {
        const bool terminou = rodada < ultima;

        for (int i = 0; i < c->quantidade; i++) {
            const int morte = c->rodadaDaMorte[i];
            const int acoes = c->acoes[rodada][i];

            assert(acoes <= 1);

            if (morte != 0 && rodada > morte) {
                assert(acoes == 0); /* derrotado nao age */
            } else if (terminou && (morte == 0 || rodada < morte)) {
                assert(acoes == 1); /* vivo na rodada inteira: agiu */
            }
        }
    }
}

/* Depois do resultado, nada mais muda no combate. */
static void conferirCombateEncerrado(
    Cenario *c,
    EstadoCombate esperado,
    Personagem *alvo
)
{
    Combate *combate = &c->combate;
    const unsigned int rodada = combate->rodada;
    const int acoes = c->totalAcoes;
    const int tamanho = combate->turnos.tamanho;

    assert(combate->estado == esperado);
    assert(combate->participanteAtual == NULL);

    for (int i = 0; i < 5; i++) {
        atualizarCombate(combate);
    }
    concluirAcaoCombate(combate);

    assert(!atacarNoCombate(combate, alvo));
    assert(!defenderNoCombate(combate));

    assert(combate->estado == esperado);
    assert(combate->rodada == rodada);
    assert(combate->turnos.tamanho == tamanho);
    assert(c->totalAcoes == acoes);
    conferirInvariantes(c);
}

static void testarVitoriaEmVariasRodadas(void)
{
    Personagem odisseu = criarPersonagem("Odisseu", TIPO_HEROI, 100, 20, 10, 12);
    Personagem ciclope = criarPersonagem("Ciclope", TIPO_INIMIGO, 30, 25, 8, 8);
    Personagem *inimigos[] = {&ciclope};
    const char *esperada[] = {
        "Odisseu", "Ciclope", "Odisseu", "Ciclope", "Odisseu"
    };

    printf("\n--- Cenario: vitoria em varias rodadas ---\n");
    iniciarCenario(&cenario, &odisseu, inimigos, 1, JOGADOR_ATACA);
    executarAteOFim(&cenario);

    assert(cenario.combate.estado == COMBATE_VICTORY);
    assert(cenario.combate.rodada == 3);
    conferirSequencia(&cenario, esperada, QUANTIDADE(esperada));
    conferirAcoesPorRodada(&cenario);

    assert(ciclope.vida == 0);
    assert(odisseu.vida == 85); /* so o golpe da primeira rodada */
    assert(cenario.combate.turnos.tamanho == 1);
    assert(buscarPersonagem(&cenario.combate.turnos, "Ciclope") == NULL);

    conferirCombateEncerrado(&cenario, COMBATE_VICTORY, &ciclope);
    encerrarCenario(&cenario);
}

static void testarDerrota(void)
{
    Personagem odisseu = criarPersonagem("Odisseu", TIPO_HEROI, 40, 10, 0, 5);
    Personagem ciclope = criarPersonagem("Ciclope", TIPO_INIMIGO, 200, 30, 5, 9);
    Personagem *inimigos[] = {&ciclope};
    const char *esperada[] = {"Ciclope", "Odisseu", "Ciclope"};

    printf("\n--- Cenario: derrota de Odisseu ---\n");
    iniciarCenario(&cenario, &odisseu, inimigos, 1, JOGADOR_ATACA);
    executarAteOFim(&cenario);

    assert(cenario.combate.estado == COMBATE_DEFEAT);
    assert(cenario.combate.rodada == 2);
    conferirSequencia(&cenario, esperada, QUANTIDADE(esperada));
    conferirAcoesPorRodada(&cenario);

    assert(odisseu.vida == 0); /* o golpe de 30 passaria dos 10 restantes */
    assert(ciclope.vida == 195);
    assert(personagemVivo(&ciclope));

    conferirCombateEncerrado(&cenario, COMBATE_DEFEAT, &ciclope);
    encerrarCenario(&cenario);
}

static void testarVelocidadesDiferentes(void)
{
    Personagem odisseu = criarPersonagem("Odisseu", TIPO_HEROI, 500, 6, 10, 10);
    Personagem sereia = criarPersonagem("Sereia", TIPO_INIMIGO, 500, 15, 5, 15);
    Personagem harpia = criarPersonagem("Harpia", TIPO_INIMIGO, 500, 20, 5, 12);
    Personagem ciclope = criarPersonagem("Ciclope", TIPO_INIMIGO, 500, 25, 8, 8);
    Personagem *inimigos[] = {&ciclope, &sereia, &harpia}; /* fora de ordem */
    const char *ordem[] = {"Sereia", "Harpia", "Odisseu", "Ciclope"};

    printf("\n--- Cenario: velocidades diferentes, 3 rodadas ---\n");
    iniciarCenario(&cenario, &odisseu, inimigos, 3, JOGADOR_ATACA);
    executarAcoes(&cenario, 12);

    assert(!combateEncerrado(&cenario.combate));
    assert(cenario.combate.rodada == 3);
    conferirOrdemRepetida(&cenario, ordem, QUANTIDADE(ordem), 3);
    conferirAcoesPorRodada(&cenario);
    assert(cenario.combate.turnos.tamanho == 4);

    encerrarCenario(&cenario);
}

static void testarVelocidadesIguais(void)
{
    Personagem odisseu = criarPersonagem("Odisseu", TIPO_HEROI, 500, 6, 10, 10);
    Personagem cila = criarPersonagem("Cila", TIPO_INIMIGO, 500, 15, 5, 10);
    Personagem caribdis = criarPersonagem("Caribdis", TIPO_INIMIGO, 500, 15, 5, 10);
    Personagem argos = criarPersonagem("Argos", TIPO_INIMIGO, 500, 15, 5, 10);
    Personagem *inimigos[] = {&cila, &caribdis, &argos};
    /* Empate: decide o nome, em ordem alfabetica. */
    const char *ordem[] = {"Argos", "Caribdis", "Cila", "Odisseu"};

    printf("\n--- Cenario: todos com a mesma velocidade ---\n");
    iniciarCenario(&cenario, &odisseu, inimigos, 3, JOGADOR_ATACA);
    executarAcoes(&cenario, 12);

    assert(cenario.combate.rodada == 3);
    conferirOrdemRepetida(&cenario, ordem, QUANTIDADE(ordem), 3);
    conferirAcoesPorRodada(&cenario);

    encerrarCenario(&cenario);
}

static void testarVelocidadesIguaisEDiferentes(void)
{
    Personagem odisseu = criarPersonagem("Odisseu", TIPO_HEROI, 500, 6, 10, 12);
    Personagem sereia = criarPersonagem("Sereia", TIPO_INIMIGO, 500, 15, 5, 12);
    Personagem argos = criarPersonagem("Argos", TIPO_INIMIGO, 500, 15, 5, 12);
    Personagem ciclope = criarPersonagem("Ciclope", TIPO_INIMIGO, 500, 15, 5, 7);
    Personagem *inimigos[] = {&sereia, &ciclope, &argos};
    const char *ordem[] = {"Argos", "Odisseu", "Sereia", "Ciclope"};

    printf("\n--- Cenario: velocidades iguais e diferentes ---\n");
    iniciarCenario(&cenario, &odisseu, inimigos, 3, JOGADOR_ATACA);
    executarAcoes(&cenario, 8);

    assert(cenario.combate.rodada == 2);
    conferirOrdemRepetida(&cenario, ordem, QUANTIDADE(ordem), 2);
    conferirAcoesPorRodada(&cenario);

    encerrarCenario(&cenario);
}

static void testarInimigoDerrotadoAntesDeAgir(void)
{
    Personagem odisseu = criarPersonagem("Odisseu", TIPO_HEROI, 200, 30, 10, 20);
    Personagem sereia = criarPersonagem("Sereia", TIPO_INIMIGO, 20, 15, 5, 15);
    Personagem ciclope = criarPersonagem("Ciclope", TIPO_INIMIGO, 500, 25, 8, 8);
    Personagem *inimigos[] = {&sereia, &ciclope};
    const char *esperada[] = {"Odisseu", "Ciclope", "Odisseu", "Ciclope"};

    printf("\n--- Cenario: inimigo derrotado antes de agir ---\n");
    iniciarCenario(&cenario, &odisseu, inimigos, 2, JOGADOR_ATACA);
    executarAcoes(&cenario, 4);

    /* Odisseu derrubou a Sereia antes da vez dela: ela nunca age. */
    assert(sereia.vida == 0);
    assert(cenario.acoes[1][indiceDe(&cenario, &sereia)] == 0);
    assert(cenario.rodadaDaMorte[indiceDe(&cenario, &sereia)] == 1);
    assert(buscarPersonagem(&cenario.combate.turnos, "Sereia") == NULL);
    assert(cenario.combate.turnos.tamanho == 2);

    assert(cenario.combate.rodada == 2);
    conferirSequencia(&cenario, esperada, QUANTIDADE(esperada));
    conferirAcoesPorRodada(&cenario);

    encerrarCenario(&cenario);
}

static void testarInimigoDerrotadoDepoisDeAgir(void)
{
    Personagem odisseu = criarPersonagem("Odisseu", TIPO_HEROI, 200, 30, 10, 10);
    Personagem sereia = criarPersonagem("Sereia", TIPO_INIMIGO, 20, 15, 5, 15);
    Personagem ciclope = criarPersonagem("Ciclope", TIPO_INIMIGO, 500, 25, 8, 8);
    Personagem *inimigos[] = {&sereia, &ciclope};
    /* Sereia age, Odisseu a derrota, Ciclope fecha a rodada sem repeticao. */
    const char *esperada[] = {
        "Sereia", "Odisseu", "Ciclope", "Odisseu", "Ciclope"
    };

    printf("\n--- Cenario: inimigo derrotado depois de agir ---\n");
    iniciarCenario(&cenario, &odisseu, inimigos, 2, JOGADOR_ATACA);
    executarAcoes(&cenario, 5);

    assert(sereia.vida == 0);
    assert(cenario.acoes[1][indiceDe(&cenario, &sereia)] == 1);
    assert(cenario.rodadaDaMorte[indiceDe(&cenario, &sereia)] == 1);
    assert(cenario.combate.turnos.tamanho == 2);

    assert(cenario.combate.rodada == 2);
    conferirSequencia(&cenario, esperada, QUANTIDADE(esperada));
    conferirAcoesPorRodada(&cenario);

    encerrarCenario(&cenario);
}

/* Derruba `removido` justo na vez dele, com o combate no meio da rodada. */
static void rodarRemocaoNoTurnoAtual(
    int indiceDoRemovido,
    const char *titulo,
    const char *const *esperada,
    int quantidade
)
{
    Personagem odisseu = criarPersonagem("Odisseu", TIPO_HEROI, 500, 6, 10, 12);
    Personagem sirena = criarPersonagem("Sirena", TIPO_INIMIGO, 500, 15, 5, 20);
    Personagem harpia = criarPersonagem("Harpia", TIPO_INIMIGO, 500, 15, 5, 15);
    Personagem ciclope = criarPersonagem("Ciclope", TIPO_INIMIGO, 500, 15, 5, 8);
    Personagem *inimigos[] = {&sirena, &harpia, &ciclope};
    Personagem *removido = inimigos[indiceDoRemovido];

    /* Ordem da rodada: Sirena, Harpia, Odisseu, Ciclope. */
    printf("\n--- Cenario: remocao no turno atual (%s) ---\n", titulo);
    iniciarCenario(&cenario, &odisseu, inimigos, 3, JOGADOR_ATACA);

    avancarAteAVezDe(&cenario, removido);
    derrotarDeFora(&cenario, removido);
    executarAcoes(&cenario, quantidade);

    assert(!combateEncerrado(&cenario.combate));
    assert(cenario.combate.rodada == 2);
    assert(cenario.combate.turnos.tamanho == 3);
    assert(!estaNaLista(&cenario.combate, removido));
    assert(cenario.acoes[1][indiceDe(&cenario, removido)] == 0);
    assert(cenario.acoes[2][indiceDe(&cenario, removido)] == 0);

    conferirSequencia(&cenario, esperada, quantidade);
    conferirAcoesPorRodada(&cenario);

    encerrarCenario(&cenario);
}

static void testarRemocaoNoTurnoAtual(void)
{
    /* Primeiro da ordem, do meio e ultimo: a rodada deve fechar certo. */
    const char *primeiro[] = {
        "Harpia", "Odisseu", "Ciclope", "Harpia", "Odisseu", "Ciclope"
    };
    const char *meio[] = {
        "Sirena", "Odisseu", "Ciclope", "Sirena", "Odisseu", "Ciclope"
    };
    const char *ultimo[] = {
        "Sirena", "Harpia", "Odisseu", "Sirena", "Harpia", "Odisseu"
    };

    rodarRemocaoNoTurnoAtual(0, "primeiro", primeiro, QUANTIDADE(primeiro));
    rodarRemocaoNoTurnoAtual(1, "do meio", meio, QUANTIDADE(meio));
    rodarRemocaoNoTurnoAtual(2, "ultimo", ultimo, QUANTIDADE(ultimo));
}

static void testarLutaCompletaContraTresInimigos(void)
{
    Personagem odisseu = criarPersonagem("Odisseu", TIPO_HEROI, 300, 30, 10, 10);
    Personagem sereia = criarPersonagem("Sereia", TIPO_INIMIGO, 40, 15, 5, 15);
    Personagem harpia = criarPersonagem("Harpia", TIPO_INIMIGO, 60, 20, 5, 12);
    Personagem ciclope = criarPersonagem("Ciclope", TIPO_INIMIGO, 100, 25, 8, 8);
    Personagem *inimigos[] = {&sereia, &harpia, &ciclope};

    printf("\n--- Cenario: luta completa contra tres inimigos ---\n");
    iniciarCenario(&cenario, &odisseu, inimigos, 3, JOGADOR_ATACA);
    executarAteOFim(&cenario);

    assert(cenario.combate.estado == COMBATE_VICTORY);
    conferirAcoesPorRodada(&cenario);

    assert(personagemVivo(&odisseu));
    assert(cenario.combate.turnos.tamanho == 1);

    /* Todos caem, na ordem em que Odisseu os escolheu. */
    for (int i = 1; i < cenario.quantidade; i++) {
        assert(cenario.todos[i]->vida == 0);
        assert(cenario.rodadaDaMorte[i] >= 1);
        assert(!estaNaLista(&cenario.combate, cenario.todos[i]));
    }
    assert(cenario.rodadaDaMorte[1] <= cenario.rodadaDaMorte[2]);
    assert(cenario.rodadaDaMorte[2] <= cenario.rodadaDaMorte[3]);
    assert(cenario.rodadaDaMorte[3] == (int)cenario.combate.rodada);

    conferirCombateEncerrado(&cenario, COMBATE_VICTORY, &ciclope);
    encerrarCenario(&cenario);
}

static void testarJogadorQueSoDefende(void)
{
    Personagem odisseu = criarPersonagem("Odisseu", TIPO_HEROI, 100, 10, 10, 5);
    Personagem gigante = criarPersonagem("Gigante", TIPO_INIMIGO, 500, 25, 20, 9);
    Personagem *inimigos[] = {&gigante};
    const char *ordem[] = {"Gigante", "Odisseu"};

    printf("\n--- Cenario: Odisseu so se defende ---\n");
    iniciarCenario(&cenario, &odisseu, inimigos, 1, JOGADOR_DEFENDE);
    executarAcoes(&cenario, 6);

    assert(cenario.combate.rodada == 3);
    conferirOrdemRepetida(&cenario, ordem, QUANTIDADE(ordem), 3);
    conferirAcoesPorRodada(&cenario);

    /* 1o golpe sem guarda (15); os outros dois com guarda (10 cada). */
    assert(odisseu.vida == 100 - 15 - 10 - 10);
    assert(gigante.vida == 500);

    encerrarCenario(&cenario);
}

int main(void)
{
    testarVitoriaEmVariasRodadas();
    testarDerrota();
    testarVelocidadesDiferentes();
    testarVelocidadesIguais();
    testarVelocidadesIguaisEDiferentes();
    testarInimigoDerrotadoAntesDeAgir();
    testarInimigoDerrotadoDepoisDeAgir();
    testarRemocaoNoTurnoAtual();
    testarLutaCompletaContraTresInimigos();
    testarJogadorQueSoDefende();

    printf("\nTESTE DOS CENARIOS DE COMBATE PASSOU!\n");

    return 0;
}

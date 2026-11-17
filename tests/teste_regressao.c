#undef NDEBUG
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "acoes_combate.h"
#include "combate.h"
#include "configuracao_fases.h"
#include "fase.h"
#include "ia_inimigo.h"
#include "jornada.h"
#include "lista_turnos.h"
#include "narrativa.h"
#include "objetivo.h"
#include "personagem.h"
#include "quicksort.h"
#include "telas_jogo.h"

/* Regressao e requisitos academicos (commit 27).
 *
 * Cada bloco corresponde a uma linha do checklist em docs/REGRESSAO.md.
 * Roda sem abrir janela: a parte grafica (interface 3D, camera em primeira
 * pessoa, HUD) fica no roteiro manual do mesmo documento. */

#define LARGURA_TELA 1280
#define ALTURA_TELA 720

/* ------------------------------------------------------------------ */
/* R1. Lista circular duplamente encadeada com 6+ operacoes reais      */
/* ------------------------------------------------------------------ */

static void verificarCircular(const ListaTurnos *lista)
{
    assert(listaConsistente(lista));
    if (lista->tamanho > 0) {
        const NoTurno *fim = lista->inicio->anterior;
        assert(fim->proximo == lista->inicio);
        assert(lista->inicio->anterior == fim);
    }
}

static void requisitoListaCircular(void)
{
    ListaTurnos lista;
    Personagem p[5];
    Personagem *saida[5];
    const char *nomes[] = {"Odisseu", "Polifemo", "Ligeia", "Cila", "Antinoo"};
    int operacoes = 0;

    for (int i = 0; i < 5; i++) {
        p[i] = criarPersonagem(nomes[i], i == 0 ? TIPO_HEROI : TIPO_INIMIGO,
                               50, 10, 5, 10 + i);
    }
    operacoes++; /* criarPersonagem */

    iniciarLista(&lista);
    for (int i = 0; i < 5; i++) {
        assert(inserirPersonagem(&lista, &p[i]));
        verificarCircular(&lista);
    }
    operacoes++; /* inserirPersonagem */

    assert(buscarPersonagem(&lista, "Cila") == &p[3]);
    assert(buscarPersonagem(&lista, "Penelope") == NULL);
    operacoes++; /* buscarPersonagem */

    assert(percorrerParticipantes(&lista, saida, 5) == 5);
    for (int i = 0; i < 5; i++) {
        assert(saida[i] == &p[i]);
    }
    operacoes++; /* percorrerParticipantes */

    assert(avancarTurno(&lista) == &p[1]);
    assert(avancarTurno(&lista) == &p[2]);
    operacoes++; /* avancarTurno */

    assert(retrocederTurno(&lista) == &p[1]);
    assert(retrocederTurno(&lista) == &p[0]);
    assert(retrocederTurno(&lista) == &p[4]); /* do inicio volta ao fim */
    operacoes++; /* retrocederTurno */

    assert(removerPersonagem(&lista, &p[4])); /* remove o atual (o fim) */
    verificarCircular(&lista);
    assert(lista.tamanho == 4);
    assert(personagemDaVez(&lista) != &p[4]);
    operacoes++; /* removerPersonagem */

    assert(ordenarPorVelocidade(&lista));
    verificarCircular(&lista);
    assert(personagemDaVez(&lista) == &p[3]); /* maior iniciativa: Cila */
    operacoes++; /* ordenarPorVelocidade */

    limparListaCombate(&lista);
    assert(listaVazia(&lista) && lista.atual == NULL && lista.tamanho == 0);
    operacoes++; /* limparListaCombate */

    assert(operacoes >= 6);
    printf("ok R1: lista circular com %d operacoes reais\n", operacoes);
}

/* ------------------------------------------------------------------ */
/* R2. QuickSort proprio define a ordem real das acoes                 */
/* ------------------------------------------------------------------ */

static Personagem *primeiroAAgir(Personagem *odisseu, Personagem *inimigo)
{
    Combate combate;
    Personagem *inimigos[] = {inimigo};
    Personagem *primeiro;

    inicializarCombate(&combate);
    assert(prepararCombate(&combate, odisseu, inimigos, 1));
    atualizarCombate(&combate); /* inicio da rodada: QuickSort */
    primeiro = combate.participanteAtual;
    assert(primeiro == combate.turnos.inicio->personagem);
    finalizarCombate(&combate);
    return primeiro;
}

static void requisitoQuickSort(void)
{
    Personagem odisseu = criarPersonagem("Odisseu", TIPO_HEROI, 100, 20, 10, 12);
    Personagem polifemo = criarPersonagem("Polifemo", TIPO_INIMIGO, 80, 18, 6, 8);
    NoTurno nos[6];
    NoTurno *vetor[6];
    Personagem ps[6];
    const int ini[6] = {4, 9, 9, 1, 7, 9};

    /* Vetor de referencias: decrescente, empate pelo nome. */
    for (int i = 0; i < 6; i++) {
        char nome[8];
        snprintf(nome, sizeof nome, "P%d", i);
        ps[i] = criarPersonagem(nome, TIPO_INIMIGO, 10, 1, 1, ini[i]);
        nos[i].personagem = &ps[i];
        vetor[i] = &nos[i];
    }
    quickSort(vetor, 0, 5);
    for (int i = 0; i < 5; i++) {
        assert(agePrimeiro(vetor[i], vetor[i + 1]));
    }
    assert(vetor[0] == &nos[1] && vetor[1] == &nos[2] && vetor[2] == &nos[5]);

    /* No combate: trocar a velocidade troca quem age primeiro. */
    assert(primeiroAAgir(&odisseu, &polifemo) == &odisseu);
    polifemo.iniciativa = 20;
    assert(primeiroAAgir(&odisseu, &polifemo) == &polifemo);

    puts("ok R2: QuickSort ordena por iniciativa e muda a ordem real");
}

/* ------------------------------------------------------------------ */
/* R3. Funcoes academicas do plano continuam existindo e funcionando   */
/* ------------------------------------------------------------------ */

static void requisitoFuncoesDeRegra(void)
{
    Personagem odisseu = criarPersonagem("Odisseu", TIPO_HEROI, 30, 20, 10, 12);
    Personagem sereia = criarPersonagem("Sereia", TIPO_INIMIGO, 12, 15, 4, 5);
    Personagem *inimigos[] = {&sereia};
    ConfiguracaoObjetivo cfg = {OBJETIVO_COLETA, "Colete cera", "cera", 2};
    Objetivo objetivo = criarObjetivo(&cfg);
    Combate combate;

    /* realizarAtaque: dano minimo e vida nunca negativa. */
    assert(calcularDano(1, 50, 0) == DANO_MINIMO);
    assert(realizarAtaque(&odisseu, &sereia) == 12);
    assert(sereia.vida == 0);

    /* verificarFimCombate: inimigo derrotado gera vitoria e sai da lista. */
    sereia = criarPersonagem("Sereia", TIPO_INIMIGO, 12, 15, 4, 5);
    inicializarCombate(&combate);
    assert(prepararCombate(&combate, &odisseu, inimigos, 1));
    assert(!verificarFimCombate(&combate));
    sereia.vida = 0;
    assert(verificarFimCombate(&combate));
    assert(combate.estado == COMBATE_VICTORY);
    assert(combate.turnos.tamanho == 1);
    finalizarCombate(&combate);

    /* Objetivo: criar, iniciar, progredir, verificar, concluir. */
    iniciarObjetivo(&objetivo);
    assert(!verificarObjetivo(&objetivo));
    assert(!registrarProgresso(&objetivo, 1));
    assert(registrarProgresso(&objetivo, 5));
    assert(objetivo.progresso == 2 && verificarObjetivo(&objetivo));
    concluirObjetivo(&objetivo);
    assert(objetivo.cumprido);

    puts("ok R3: realizarAtaque, verificarFimCombate e funcoes de objetivo");
}

/* ------------------------------------------------------------------ */
/* R4. Regras em C puro, sem Raylib, e sem qsort()                     */
/* ------------------------------------------------------------------ */

/* Le um arquivo e apaga os comentarios, para que uma mencao a qsort()
 * num comentario nao conte como uso. */
static char *lerCodigoSemComentarios(const char *caminho)
{
    FILE *arquivo = fopen(caminho, "rb");
    char *texto;
    long tamanho;
    size_t lidos;

    if (arquivo == NULL) {
        return NULL;
    }
    fseek(arquivo, 0, SEEK_END);
    tamanho = ftell(arquivo);
    fseek(arquivo, 0, SEEK_SET);
    texto = malloc((size_t)tamanho + 1);
    assert(texto != NULL);
    lidos = fread(texto, 1, (size_t)tamanho, arquivo);
    texto[lidos] = '\0';
    fclose(arquivo);

    for (size_t i = 0; texto[i] != '\0'; i++) {
        if (texto[i] == '/' && texto[i + 1] == '*') {
            while (texto[i] != '\0' &&
                   !(texto[i] == '*' && texto[i + 1] == '/')) {
                texto[i++] = ' ';
            }
            if (texto[i] != '\0') {
                texto[i] = ' ';
                texto[i + 1] = ' ';
            }
        } else if (texto[i] == '/' && texto[i + 1] == '/') {
            while (texto[i] != '\0' && texto[i] != '\n') {
                texto[i++] = ' ';
            }
        }
    }
    return texto;
}

static bool terminaCom(const char *texto, const char *sufixo)
{
    const size_t a = strlen(texto);
    const size_t b = strlen(sufixo);
    return a >= b && strcmp(texto + a - b, sufixo) == 0;
}

static void requisitoCodigoFonte(void)
{
    /* Modulos de regra e estrutura: nao podem depender da Raylib. */
    static const char *const regras[] = {
        "personagem", "lista_turnos", "quicksort", "combate",
        "acoes_combate", "ia_inimigo", "jornada", "objetivo"
    };
    char fontes[] = ODISSEU_FONTES;
    int arquivosC = 0;
    int arquivosRegra = 0;

    for (char *caminho = strtok(fontes, "|"); caminho != NULL;
         caminho = strtok(NULL, "|")) {
        char *codigo;

        /* Nenhum arquivo C++ no jogo. */
        assert(!terminaCom(caminho, ".cpp") && !terminaCom(caminho, ".hpp"));
        if (terminaCom(caminho, ".c")) {
            arquivosC++;
        }

        codigo = lerCodigoSemComentarios(caminho);
        if (codigo == NULL) {
            fprintf(stderr, "nao foi possivel ler %s\n", caminho);
            assert(0);
        }

        /* Nenhuma ordenacao pronta substitui o QuickSort. */
        if (strstr(codigo, "qsort") != NULL) {
            fprintf(stderr, "uso de qsort em %s\n", caminho);
            assert(0);
        }

        for (size_t i = 0; i < sizeof regras / sizeof regras[0]; i++) {
            char c[64];
            char h[64];

            snprintf(c, sizeof c, "/%s.c", regras[i]);
            snprintf(h, sizeof h, "/%s.h", regras[i]);
            if (terminaCom(caminho, c) || terminaCom(caminho, h)) {
                arquivosRegra++;
                if (strstr(codigo, "raylib.h") != NULL ||
                    strstr(codigo, "raymath.h") != NULL) {
                    fprintf(stderr, "Raylib em modulo de regra: %s\n",
                            caminho);
                    assert(0);
                }
            }
        }
        free(codigo);
    }

    assert(arquivosC > 0);
    assert(arquivosRegra == 16); /* 8 modulos, cada um com .c e .h */
    printf("ok R4: %d arquivos .c, regras sem Raylib e sem qsort()\n",
           arquivosC);
}

/* ------------------------------------------------------------------ */
/* R5. Partida completa: menu -> 4 fases -> retorno ao lar             */
/* ------------------------------------------------------------------ */

static Vector3 centroDaSaida(const Fase *fase)
{
    return (Vector3){
        (fase->saida.min.x + fase->saida.max.x) * 0.5f,
        fase->saida.min.y + 0.01f,
        (fase->saida.min.z + fase->saida.max.z) * 0.5f
    };
}

/* Combate da fase com os inimigos da configuracao. Odisseu sempre ataca
 * o primeiro inimigo vivo; no meio da luta o jogador pausa e retoma. */
static EstadoCombate jogarConfronto(Personagem *odisseu,
                                    const ConfiguracaoFase *configuracao,
                                    bool pausarNoMeio)
{
    Personagem inimigos[FASE_MAX_INIMIGOS];
    Personagem *ponteiros[FASE_MAX_INIMIGOS];
    const int quantidade = configuracao->encontro.quantidadeInimigos;
    EstadoCombate resultado;
    Combate combate;
    int passos = 0;
    bool pausou = false;

    for (int i = 0; i < quantidade; i++) {
        inimigos[i] = criarPersonagemConfigurada(
            &configuracao->encontro.inimigos[i].personagem);
        ponteiros[i] = &inimigos[i];
    }

    inicializarCombate(&combate);
    assert(prepararCombate(&combate, odisseu, ponteiros, quantidade));
    configurarIdentidadeCombate(&combate, configuracao->identidade, false);

    while (combate.estado != COMBATE_VICTORY &&
           combate.estado != COMBATE_DEFEAT) {
        assert(++passos < 10000);
        assert(listaConsistente(&combate.turnos));

        if (combate.estado == COMBATE_WAITING_PLAYER) {
            Personagem *alvo = NULL;

            if (pausarNoMeio && !pausou && combate.rodada == 2) {
                EntradaTelaJogo esc = {0};
                esc.cancelar = true;
                assert(interpretarEntradaPausa(esc, true, LARGURA_TELA,
                                               ALTURA_TELA) ==
                       ACAO_TELA_RETOMAR);
                pausou = true;
            }

            for (int i = 0; i < quantidade; i++) {
                if (personagemVivo(ponteiros[i])) {
                    alvo = ponteiros[i];
                    break;
                }
            }
            assert(atacarNoCombate(&combate, alvo));
        } else {
            atualizarCombate(&combate);
        }
    }

    /* Depois do resultado, nenhuma acao de combate e aceita. */
    assert(!defenderNoCombate(&combate));
    assert(!executarTurnoInimigo(&combate));

    resultado = combate.estado;
    finalizarCombate(&combate);
    assert(listaVazia(&combate.turnos)); /* reinicio limpa a lista */
    return resultado;
}

static void requisitoPartidaCompleta(void)
{
    EntradaTelaJogo enter = {0};
    const ConfiguracaoFase *primeira = obterConfiguracaoFase(0);
    Narrativa narrativa;
    Jornada jornada;
    int fasesConcluidas = 0;

    /* Menu: ENTER inicia o jogo. */
    enter.confirmar = true;
    assert(interpretarEntradaMenu(enter, LARGURA_TELA, ALTURA_TELA) ==
           ACAO_TELA_JOGAR);

    /* Introducao com premissa, controles e primeiro objetivo. */
    narrativa = criarNarrativaIntroducao(primeira->titulo,
                                         primeira->textoNarrativo,
                                         primeira->objetivo.descricao);
    assert(narrativaValida(&narrativa) && narrativa.mostrarControles);

    assert(iniciarJornada(&jornada,
                          criarPersonagemConfigurada(&primeira->heroi),
                          quantidadeConfiguracoesFase()));

    for (int indice = 0; indice < quantidadeConfiguracoesFase(); indice++) {
        const ConfiguracaoFase *configuracao =
            obterConfiguracaoFase(jornada.indiceEtapaAtual);
        const int ataqueAntes = jornada.odisseu.ataque;
        ResultadoAvancoJornada avanco;
        Fase fase;

        assert(jornada.indiceEtapaAtual == indice);
        assert(criarFase(&fase, configuracao));
        assert(fase.etapa == ETAPA_EXPLORACAO);
        assert(fase.objetivo.iniciado && fase.objetivo.descricao != NULL);

        /* Sem objetivo cumprido, a saida nao abre e a jornada nao anda. */
        atualizarFase(&fase, centroDaSaida(&fase), 0.016f);
        assert(fase.etapa == ETAPA_EXPLORACAO);
        assert(avancarJornada(&jornada, false, false) ==
               JORNADA_AVANCO_BLOQUEADO);

        /* Explora: interage com cada item mirado. Sem mira, nada acontece. */
        fase.itemAlvo = -1;
        assert(!interagir(&fase));
        for (int i = 0; i < fase.quantidadeItens &&
                        fase.etapa == ETAPA_EXPLORACAO; i++) {
            fase.itemAlvo = i;
            assert(interagir(&fase));
        }
        assert(verificarObjetivo(&fase.objetivo));
        assert(fase.etapa == ETAPA_CONFRONTO);

        /* Objetivo cumprido mas combate nao vencido: ainda bloqueado. */
        assert(avancarJornada(&jornada, true, true) ==
               JORNADA_AVANCO_BLOQUEADO);

        /* Confronto por turnos. Na fase 1, perde de proposito uma vez e
         * refaz com vida cheia, como o jogo faz apos a derrota. */
        if (indice == 0) {
            const int vidaCheia = jornada.odisseu.vida;
            jornada.odisseu.vida = 1;
            assert(jogarConfronto(&jornada.odisseu, configuracao, false) ==
                   COMBATE_DEFEAT);
            jornada.odisseu.vida = vidaCheia;
            jornada.odisseu.bonusDefesa = 0;
        }
        assert(jogarConfronto(&jornada.odisseu, configuracao, indice == 1) ==
               COMBATE_VICTORY);
        registrarVitoriaJornada(&jornada);
        encerrarConfronto(&fase);
        assert(fase.etapa == ETAPA_SAIDA_LIBERADA);

        /* Vencer o combate nao pula a fase: falta chegar a saida. */
        assert(avancarJornada(&jornada, true, false) ==
               JORNADA_AVANCO_BLOQUEADO);
        atualizarFase(&fase, centroDaSaida(&fase), 0.016f);
        assert(fase.etapa == ETAPA_CONCLUIDA);

        avanco = avancarJornada(&jornada,
                                verificarObjetivo(&fase.objetivo),
                                fase.etapa == ETAPA_CONCLUIDA);
        fasesConcluidas++;

        /* Atributos de Odisseu preservados entre etapas. */
        assert(jornada.odisseu.ataque == ataqueAntes);
        assert(personagemVivo(&jornada.odisseu));

        if (indice < quantidadeConfiguracoesFase() - 1) {
            const ConfiguracaoFase *proxima =
                obterConfiguracaoFase(jornada.indiceEtapaAtual);
            assert(avanco == JORNADA_PROXIMA_FASE);
            narrativa = criarNarrativaTransicao(proxima->titulo,
                                                proxima->textoNarrativo,
                                                proxima->objetivo.descricao);
            assert(narrativaValida(&narrativa));
        } else {
            assert(avanco == JORNADA_RETORNO_AO_LAR);
        }
    }

    assert(fasesConcluidas == 4);
    assert(jornada.concluida);
    assert(avancarJornada(&jornada, true, true) == JORNADA_AVANCO_BLOQUEADO);

    narrativa = criarNarrativaEncerramento();
    assert(narrativaValida(&narrativa));
    assert(narrativa.tipo == NARRATIVA_ENCERRAMENTO);

    printf("ok R5: partida completa do menu ao retorno a Itaca "
           "(4 fases, vida final %d/%d)\n",
           jornada.odisseu.vida, jornada.odisseu.vidaMaxima);
}

/* ------------------------------------------------------------------ */
/* R6. Cada fase tem objetivo proprio e encontro configurado           */
/* ------------------------------------------------------------------ */

static void requisitoFasesConfiguradas(void)
{
    const int quantidade = quantidadeConfiguracoesFase();

    assert(quantidade == 4);
    for (int i = 0; i < quantidade; i++) {
        const ConfiguracaoFase *f = obterConfiguracaoFase(i);

        assert(f->titulo != NULL && f->instrucaoInicial != NULL);
        assert(f->objetivo.descricao != NULL);
        assert(f->objetivo.quantidadeNecessaria > 0);
        assert(f->encontro.quantidadeInimigos > 0);
        assert(f->encontro.quantidadeInimigos <= FASE_MAX_INIMIGOS);
        assert(f->textoObjetivoConfronto != NULL);
        assert(f->textoObjetivoSaida != NULL);

        for (int j = 0; j < i; j++) {
            assert(strcmp(f->objetivo.descricao,
                          obterConfiguracaoFase(j)->objetivo.descricao) != 0);
            assert(strcmp(f->id, obterConfiguracaoFase(j)->id) != 0);
        }
    }
    puts("ok R6: quatro fases com objetivos e encontros distintos");
}

int main(void)
{
    requisitoListaCircular();
    requisitoQuickSort();
    requisitoFuncoesDeRegra();
    requisitoCodigoFonte();
    requisitoFasesConfiguradas();
    requisitoPartidaCompleta();

    puts("\nregressao concluida: todos os requisitos automaticos passaram");
    return 0;
}

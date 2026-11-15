#undef NDEBUG
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "lista_turnos.h"
#include "personagem.h"
#include "quicksort.h"

/* Testes do QuickSort e da religacao da lista (commits 09 a 11).
 * Nao abrem janela Raylib: usam apenas os modulos de C puro. */

#define MAXIMO 16

static Personagem participantes[MAXIMO];

/* Confere as invariantes circulares do plano:
 * inicio->anterior == fim, fim->proximo == inicio,
 * no->proximo->anterior == no, no->anterior->proximo == no
 * e quantidade percorrida == tamanho (nos dois sentidos). */
static void verificarInvariantes(const ListaTurnos *lista)
{
    assert(listaConsistente(lista));

    if (listaVazia(lista)) {
        return;
    }

    {
        const NoTurno *fim = lista->inicio->anterior;
        const NoTurno *no = lista->inicio;
        int ida = 0;
        int volta = 0;

        assert(fim->proximo == lista->inicio);

        do {
            ida++;
            assert(ida <= lista->tamanho);
            no = no->proximo;
        } while (no != lista->inicio);

        no = lista->inicio;
        do {
            volta++;
            assert(volta <= lista->tamanho);
            no = no->anterior;
        } while (no != lista->inicio);

        assert(ida == lista->tamanho);
        assert(volta == lista->tamanho);
    }
}

/* Guarda os enderecos dos nos (nao dos personagens) em uma volta. */
static int capturarNos(const ListaTurnos *lista, const NoTurno **saida)
{
    const NoTurno *no = lista->inicio;

    for (int i = 0; i < lista->tamanho; i++) {
        saida[i] = no;
        no = no->proximo;
    }

    return lista->tamanho;
}

/* Prova que o conjunto de nos e o mesmo: cada no antigo aparece exatamente
 * uma vez depois da ordenacao, e cada no continua com seu personagem. */
static void verificarMesmosNos(const NoTurno **antes,
                               Personagem *const *personagensAntes,
                               int quantidade,
                               const ListaTurnos *lista)
{
    const NoTurno *depois[MAXIMO];
    int vezes[MAXIMO] = {0};

    assert(lista->tamanho == quantidade);
    assert(capturarNos(lista, depois) == quantidade);

    for (int i = 0; i < quantidade; i++) {
        int achados = 0;

        for (int j = 0; j < quantidade; j++) {
            if (depois[j] == antes[i]) {
                achados++;
                vezes[j]++;
                assert(depois[j]->personagem == personagensAntes[i]);
            }
        }

        assert(achados == 1);
    }

    for (int j = 0; j < quantidade; j++) {
        assert(vezes[j] == 1);
    }
}

/* Confere a ordem esperada pelos nomes, seguindo avancarTurno(). */
static void verificarOrdemPorTurnos(ListaTurnos *lista,
                                    const char *const *esperado,
                                    int quantidade)
{
    assert(lista->tamanho == quantidade);
    assert(personagemDaVez(lista) == lista->inicio->personagem);

    for (int i = 0; i < quantidade; i++) {
        assert(strcmp(personagemDaVez(lista)->nome, esperado[i]) == 0);
        avancarTurno(lista);
    }

    /* Uma volta completa retorna ao primeiro da rodada. */
    assert(strcmp(personagemDaVez(lista)->nome, esperado[0]) == 0);
}

/* Ordena e confere invariantes e preservacao dos nos. */
static void ordenarComVerificacao(ListaTurnos *lista)
{
    const NoTurno *antes[MAXIMO];
    Personagem *personagensAntes[MAXIMO];
    const int quantidade = capturarNos(lista, antes);

    for (int i = 0; i < quantidade; i++) {
        personagensAntes[i] = antes[i]->personagem;
    }

    assert(ordenarPorVelocidade(lista));
    verificarInvariantes(lista);
    verificarMesmosNos(antes, personagensAntes, quantidade, lista);
    assert(lista->atual == lista->inicio);
}

static void montar(ListaTurnos *lista, int quantidade)
{
    iniciarLista(lista);

    for (int i = 0; i < quantidade; i++) {
        assert(inserirPersonagem(lista, &participantes[i]));
    }

    verificarInvariantes(lista);
}

/* ---- Commit 09: quickSort() sobre vetores de referencias ---- */

static void testarQuickSortVetor(void)
{
    NoTurno nos[5];
    NoTurno *vetor[5];
    const int iniciativas[5] = {3, 9, 1, 9, 5};
    const char *nomes[5] = {"C", "B", "E", "A", "D"};

    for (int i = 0; i < 5; i++) {
        participantes[i] = criarPersonagem(nomes[i], TIPO_INIMIGO, 10, 1, 1,
                                           iniciativas[i]);
        nos[i].personagem = &participantes[i];
        vetor[i] = &nos[i];
    }

    quickSort(vetor, 0, 4);

    /* 9(A), 9(B), 5(D), 3(C), 1(E): empate decidido pelo nome. */
    assert(vetor[0] == &nos[3]);
    assert(vetor[1] == &nos[1]);
    assert(vetor[2] == &nos[4]);
    assert(vetor[3] == &nos[0]);
    assert(vetor[4] == &nos[2]);

    for (int i = 0; i < 4; i++) {
        assert(agePrimeiro(vetor[i], vetor[i + 1]));
    }

    /* Vetor unitario e intervalo vazio nao fazem nada. */
    quickSort(vetor, 2, 2);
    assert(vetor[2] == &nos[4]);
    quickSort(vetor, 3, 1);
    quickSort(NULL, 0, 4);

    puts("ok: quickSort em vetor (empates, unitario, intervalo vazio)");
}

/* ---- Commit 10/11: ordenarPorVelocidade() sobre a lista ---- */

static void testarListaVaziaEUnitaria(void)
{
    ListaTurnos lista;

    iniciarLista(&lista);
    assert(ordenarPorVelocidade(&lista));
    assert(listaVazia(&lista));
    verificarInvariantes(&lista);
    assert(!ordenarPorVelocidade(NULL));

    participantes[0] = criarPersonagem("Odisseu", TIPO_HEROI, 100, 18, 8, 12);
    montar(&lista, 1);
    {
        NoTurno *unico = lista.inicio;

        assert(ordenarPorVelocidade(&lista));
        assert(lista.inicio == unico && lista.atual == unico);
        assert(unico->proximo == unico && unico->anterior == unico);
    }
    verificarInvariantes(&lista);

    limparListaCombate(&lista);
    puts("ok: ordenar lista vazia e lista com um no");
}

static void testarEntradasClassicas(void)
{
    ListaTurnos lista;
    const char *crescente[] = {"P1", "P2", "P3", "P4", "P5", "P6"};

    /* Entrada ja ordenada. */
    for (int i = 0; i < 6; i++) {
        participantes[i] = criarPersonagem(crescente[i], TIPO_INIMIGO, 10, 1,
                                           1, 60 - i * 10);
    }
    montar(&lista, 6);
    ordenarComVerificacao(&lista);
    verificarOrdemPorTurnos(&lista, crescente, 6);
    limparListaCombate(&lista);

    /* Entrada invertida: iniciativa crescente na insercao. */
    for (int i = 0; i < 6; i++) {
        participantes[i] = criarPersonagem(crescente[5 - i], TIPO_INIMIGO, 10,
                                           1, 1, 10 + i * 10);
    }
    montar(&lista, 6);
    ordenarComVerificacao(&lista);
    verificarOrdemPorTurnos(&lista, crescente, 6);
    limparListaCombate(&lista);

    puts("ok: entradas ordenada e invertida");
}

static void testarVelocidadesRepetidas(void)
{
    ListaTurnos lista;
    const char *esperado[] = {"Circe", "Odisseu", "Sereia", "Cila", "Polifemo"};

    participantes[0] = criarPersonagem("Sereia", TIPO_INIMIGO, 40, 10, 2, 12);
    participantes[1] = criarPersonagem("Polifemo", TIPO_INIMIGO, 150, 22, 4, 5);
    participantes[2] = criarPersonagem("Odisseu", TIPO_HEROI, 100, 18, 8, 12);
    participantes[3] = criarPersonagem("Cila", TIPO_INIMIGO, 90, 16, 5, 7);
    participantes[4] = criarPersonagem("Circe", TIPO_INIMIGO, 80, 15, 6, 12);

    montar(&lista, 5);
    ordenarComVerificacao(&lista);
    verificarOrdemPorTurnos(&lista, esperado, 5);

    /* Ordenar de novo sem mudar nada produz a mesma ordem (determinismo). */
    ordenarComVerificacao(&lista);
    verificarOrdemPorTurnos(&lista, esperado, 5);

    /* Todos iguais: so o nome decide. */
    for (int i = 0; i < 5; i++) {
        participantes[i].iniciativa = 7;
    }
    ordenarComVerificacao(&lista);
    {
        const char *alfabetica[] = {"Cila", "Circe", "Odisseu", "Polifemo",
                                    "Sereia"};
        verificarOrdemPorTurnos(&lista, alfabetica, 5);
    }

    limparListaCombate(&lista);
    puts("ok: velocidades repetidas com desempate estavel");
}

/* Criterio de aceite do commit 10: mudar a velocidade muda o turno. */
static void testarBonusDeIniciativa(void)
{
    ListaTurnos lista;
    const char *antes[] = {"Circe", "Odisseu", "Polifemo"};
    const char *depois[] = {"Odisseu", "Circe", "Polifemo"};
    const char *lento[] = {"Circe", "Polifemo", "Odisseu"};

    participantes[0] = criarPersonagem("Odisseu", TIPO_HEROI, 100, 18, 8, 12);
    participantes[1] = criarPersonagem("Polifemo", TIPO_INIMIGO, 150, 22, 4, 5);
    participantes[2] = criarPersonagem("Circe", TIPO_INIMIGO, 80, 15, 6, 14);

    montar(&lista, 3);
    ordenarComVerificacao(&lista);
    verificarOrdemPorTurnos(&lista, antes, 3);

    /* Bonus temporario da rodada: Odisseu passa de 12 para 12 + 5. */
    participantes[0].iniciativa += 5;
    ordenarComVerificacao(&lista);
    verificarOrdemPorTurnos(&lista, depois, 3);
    assert(personagemDaVez(&lista) == &participantes[0]);

    /* Fim do bonus e penalidade: Odisseu cai para o ultimo turno. */
    participantes[0].iniciativa = 2;
    ordenarComVerificacao(&lista);
    verificarOrdemPorTurnos(&lista, lento, 3);

    /* Retroceder a partir do primeiro chega ao ultimo da rodada. */
    assert(retrocederTurno(&lista) == &participantes[0]);

    limparListaCombate(&lista);
    puts("ok: bonus de iniciativa altera a ordem real dos turnos");
}

static void testarDiferentesQuantidades(void)
{
    /* Iniciativas embaralhadas e com repeticoes, de 2 ate MAXIMO nos. */
    const int base[MAXIMO] = {7, 3, 15, 3, 9, 0, 12, 7, 1, 15, 4, 8, 7, 2, 11,
                              5};

    for (int quantidade = 2; quantidade <= MAXIMO; quantidade++) {
        ListaTurnos lista;
        const NoTurno *no;
        char nome[TAMANHO_NOME_PERSONAGEM];

        for (int i = 0; i < quantidade; i++) {
            snprintf(nome, sizeof nome, "P%02d", i);
            participantes[i] = criarPersonagem(nome, TIPO_INIMIGO, 10, 1, 1,
                                               base[i]);
        }

        montar(&lista, quantidade);
        ordenarComVerificacao(&lista);

        /* Cada vizinho respeita a ordem: iniciativa nao cresce e, em
         * empate, o nome vem em ordem alfabetica. */
        no = lista.inicio;
        for (int i = 0; i < quantidade - 1; i++) {
            assert(agePrimeiro(no, no->proximo));
            no = no->proximo;
        }

        /* Remover o primeiro e reordenar continua valido. */
        assert(removerPersonagem(&lista, lista.inicio->personagem));
        verificarInvariantes(&lista);
        ordenarComVerificacao(&lista);

        limparListaCombate(&lista);
        assert(listaVazia(&lista));
    }

    puts("ok: diferentes quantidades (2 a 16) com remocao e reordenacao");
}

int main(void)
{
    testarQuickSortVetor();
    testarListaVaziaEUnitaria();
    testarEntradasClassicas();
    testarVelocidadesRepetidas();
    testarBonusDeIniciativa();
    testarDiferentesQuantidades();

    puts("todos os testes de quicksort passaram");
    return 0;
}

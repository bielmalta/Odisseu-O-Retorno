#undef NDEBUG
#include <assert.h>
#include <stdio.h>

#include "lista_turnos.h"
#include "personagem.h"

static Personagem odisseu;
static Personagem polifemo;
static Personagem circe;

static void prepararPersonagens(void){
    odisseu = criarPersonagem("Odisseu", TIPO_HEROI, 100, 18, 8, 12);
    polifemo = criarPersonagem("Polifemo", TIPO_INIMIGO, 150, 22, 4, 5);
    circe = criarPersonagem("Circe", TIPO_INIMIGO, 80, 15, 6, 14);
}

static void montarLista(ListaTurnos *lista){
    iniciarLista(lista);
    assert(inserirPersonagem(lista, &odisseu));
    assert(inserirPersonagem(lista, &polifemo));
    assert(inserirPersonagem(lista, &circe));
    assert(listaConsistente(lista));
}

static void testarPersonagem(void){
    Personagem fraco = criarPersonagem(NULL, TIPO_INIMIGO, 0, -3, -1, -7);

    assert(fraco.vida == 1);
    assert(fraco.ataque == 0 && fraco.defesa == 0 && fraco.iniciativa == 0);

    assert(aplicarDano(&odisseu, 30) == 30);
    assert(odisseu.vida == 70);
    assert(aplicarDano(&odisseu, -5) == 0);
    assert(aplicarDano(&odisseu, 500) == 70);
    assert(odisseu.vida == 0);
    assert(!personagemVivo(&odisseu));
    assert(personagemVivo(&circe));

    puts("ok: personagem");
}

static void testarListaVazia(void){
    ListaTurnos lista;
    Personagem *saida[3];

    iniciarLista(&lista);
    assert(listaVazia(&lista));
    assert(listaConsistente(&lista));
    assert(personagemDaVez(&lista) == NULL);
    assert(avancarTurno(&lista) == NULL);
    assert(retrocederTurno(&lista) == NULL);
    assert(buscarPersonagem(&lista, "Odisseu") == NULL);
    assert(percorrerParticipantes(&lista, saida, 3) == 0);
    assert(!removerPersonagem(&lista, &odisseu));
    assert(!inserirPersonagem(&lista, NULL));
    limparListaCombate(&lista);

    puts("ok: lista vazia");
}

static void testarUmNo(void){
    ListaTurnos lista;

    iniciarLista(&lista);
    assert(inserirPersonagem(&lista, &odisseu));
    assert(listaConsistente(&lista));

    assert(lista.inicio->proximo == lista.inicio);
    assert(lista.inicio->anterior == lista.inicio);
    assert(avancarTurno(&lista) == &odisseu);
    assert(retrocederTurno(&lista) == &odisseu);

    assert(removerPersonagem(&lista, &odisseu));
    assert(listaVazia(&lista));
    assert(listaConsistente(&lista));

    puts("ok: lista com um no");
}

static void testarInsercaoBuscaEPercurso(void){
    ListaTurnos lista;
    Personagem *saida[5];

    montarLista(&lista);
    assert(lista.tamanho == 3);
    assert(personagemDaVez(&lista) == &odisseu);
    assert(lista.inicio->anterior->personagem == &circe);

    assert(buscarPersonagem(&lista, "Circe") == &circe);
    assert(buscarPersonagem(&lista, "Calipso") == NULL);

    assert(percorrerParticipantes(&lista, saida, 5) == 3);
    assert(saida[0] == &odisseu);
    assert(saida[1] == &polifemo);
    assert(saida[2] == &circe);

    assert(percorrerParticipantes(&lista, saida, 2) == 2);

    limparListaCombate(&lista);
    puts("ok: insercao, busca e percurso");
}

static void testarNavegacao(void){
    ListaTurnos lista;

    montarLista(&lista);

    assert(avancarTurno(&lista) == &polifemo);
    assert(avancarTurno(&lista) == &circe);
    assert(avancarTurno(&lista) == &odisseu);

    assert(retrocederTurno(&lista) == &circe);
    assert(retrocederTurno(&lista) == &polifemo);
    assert(listaConsistente(&lista));

    limparListaCombate(&lista);
    puts("ok: avanco e retrocesso");
}

static void testarRemocao(void){
    ListaTurnos lista;

    montarLista(&lista);
    assert(removerPersonagem(&lista, &polifemo));
    assert(lista.tamanho == 2);
    assert(listaConsistente(&lista));
    assert(!removerPersonagem(&lista, &polifemo));
    assert(avancarTurno(&lista) == &circe);
    limparListaCombate(&lista);

    montarLista(&lista);
    assert(removerPersonagem(&lista, &odisseu));
    assert(lista.inicio->personagem == &polifemo);
    assert(listaConsistente(&lista));
    limparListaCombate(&lista);

    montarLista(&lista);
    assert(removerPersonagem(&lista, &circe));
    assert(lista.inicio->anterior->personagem == &polifemo);
    assert(lista.inicio->anterior->proximo == lista.inicio);
    assert(listaConsistente(&lista));
    limparListaCombate(&lista);

    puts("ok: remocao no inicio, meio e fim");
}

static void testarRemocaoDoPersonagemDaVez(void){
    ListaTurnos lista;

    montarLista(&lista);
    avancarTurno(&lista);
    assert(personagemDaVez(&lista) == &polifemo);

    assert(removerPersonagem(&lista, &polifemo));
    assert(personagemDaVez(&lista) == &odisseu);
    assert(listaConsistente(&lista));
    assert(avancarTurno(&lista) == &circe);

    limparListaCombate(&lista);
    puts("ok: remocao de quem esta jogando");
}

static void testarLimpeza(void){
    ListaTurnos lista;

    montarLista(&lista);
    limparListaCombate(&lista);

    assert(listaVazia(&lista));
    assert(lista.atual == NULL);
    assert(lista.tamanho == 0);

    assert(inserirPersonagem(&lista, &circe));
    assert(personagemDaVez(&lista) == &circe);
    limparListaCombate(&lista);

    puts("ok: limpeza");
}

int main(void){
    prepararPersonagens();
    testarPersonagem();

    prepararPersonagens();
    testarListaVazia();
    testarUmNo();
    testarInsercaoBuscaEPercurso();
    testarNavegacao();
    testarRemocao();
    testarRemocaoDoPersonagemDaVez();
    testarLimpeza();

    puts("todos os testes passaram");
    return 0;
}
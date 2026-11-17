#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "configuracao_fases.h"
#include "fase.h"
#include "jogador.h"

/* Mesma regra de colisao do jogador: o corpo nao pode nascer dentro de uma
 * caixa, senao ele fica preso (aconteceu no confronto com Cila). */
static bool corpoDentroDe(Vector3 p, BoundingBox c)
{
    return p.x + JOGADOR_RAIO > c.min.x && p.x - JOGADOR_RAIO < c.max.x &&
           p.z + JOGADOR_RAIO > c.min.z && p.z - JOGADOR_RAIO < c.max.z &&
           p.y + JOGADOR_ALTURA_CORPO > c.min.y && p.y < c.max.y;
}

static void testarTabelaPrincipal(void)
{
    const ConfiguracaoFase *configuracao = obterConfiguracaoFase(0);
    Personagem heroi;
    Personagem inimigo;
    Fase fase;

    assert(quantidadeConfiguracoesFase() == 4);
    assert(configuracao != NULL);
    assert(obterConfiguracaoFase(-1) == NULL);
    assert(obterConfiguracaoFase(4) == NULL);
    assert(strcmp(configuracao->id, "caverna_polifemo") == 0);
    assert(configuracao->quantidadeCaixas == 14);
    assert(configuracao->quantidadeItens == 3);
    assert(configuracao->encontro.quantidadeInimigos == 1);

    heroi = criarPersonagemConfigurada(&configuracao->heroi);
    inimigo = criarPersonagemConfigurada(
        &configuracao->encontro.inimigos[0].personagem
    );
    assert(strcmp(heroi.nome, "Odisseu") == 0);
    assert(heroi.vida == 170);
    assert(strcmp(inimigo.nome, "Polifemo") == 0);
    assert(inimigo.vida == 84);

    assert(criarFase(&fase, configuracao));
    assert(fase.configuracao == configuracao);
    assert(fase.cenario.quantidade == configuracao->quantidadeCaixas);
    assert(fase.quantidadeItens == configuracao->quantidadeItens);
    assert(fase.quantidadeInimigos ==
           configuracao->encontro.quantidadeInimigos);
    assert(fase.objetivo.necessario == 3);
    assert(fase.objetivo.iniciado);
    assert(fase.indicePortao >= 0);
}

static void testarFasesMitologicas(void)
{
    const char *ids[] = {
        "caverna_polifemo",
        "canto_sereias",
        "estreito_cila",
        "retorno_itaca"
    };
    const TipoObjetivo tipos[] = {
        OBJETIVO_COLETA,
        OBJETIVO_INTERACAO,
        OBJETIVO_INTERACAO,
        OBJETIVO_COLETA
    };
    const int quantidades[] = { 3, 3, 3, 1 };
    const int inimigos[] = { 1, 2, 3, 2 };

    for (int i = 0; i < quantidadeConfiguracoesFase(); i++) {
        const ConfiguracaoFase *configuracao = obterConfiguracaoFase(i);
        Fase fase;

        assert(configuracao != NULL);
        assert(strcmp(configuracao->id, ids[i]) == 0);
        assert(configuracao->objetivo.tipo == tipos[i]);
        assert(configuracao->objetivo.quantidadeNecessaria == quantidades[i]);
        assert(configuracao->quantidadeItens == quantidades[i]);
        assert(configuracao->encontro.quantidadeInimigos == inimigos[i]);
        assert(configuracao->encontro.recompensaNarrativa != NULL);
        assert(configuracao->encontro.inimigos[0].visual.modelo != NULL);

        assert(criarFase(&fase, configuracao));
        assert(fase.configuracao == configuracao);
        assert(fase.objetivo.tipo == tipos[i]);
        assert(fase.quantidadeInimigos == inimigos[i]);
        assert(fase.indicePortao >= 0);

        for (int c = 0; c < fase.cenario.quantidade; c++) {
            assert(!corpoDentroDe(fase.pontoInicial, fase.cenario.caixas[c]));
            assert(!corpoDentroDe(
                fase.pontoJogadorConfronto, fase.cenario.caixas[c]
            ));
        }
    }
}

static void testarConfiguracaoAlternativa(void)
{
    const ConfiguracaoFase *base = obterConfiguracaoFase(0);
    ConfiguracaoFase alternativa = *base;
    ConfiguracaoInimigo inimigos[2] = {
        base->encontro.inimigos[0],
        base->encontro.inimigos[0]
    };
    Fase fase;
    Personagem segundo;

    inimigos[0].personagem.nome = "Sereia";
    inimigos[0].personagem.vidaMaxima = 70;
    inimigos[0].posicao = (Vector3){ -2.0f, 0.0f, -3.0f };
    inimigos[1].personagem.nome = "Harpia";
    inimigos[1].personagem.vidaMaxima = 80;
    inimigos[1].posicao = (Vector3){ 2.0f, 0.0f, -3.0f };

    alternativa.id = "fase_teste";
    alternativa.titulo = "Fase configurada para teste";
    alternativa.objetivo.descricao = "Colete dois simbolos";
    alternativa.objetivo.nomeAlvo = "simbolo";
    alternativa.objetivo.quantidadeNecessaria = 2;
    alternativa.encontro.nome = "Encontro de teste";
    alternativa.encontro.inimigos = inimigos;
    alternativa.encontro.quantidadeInimigos = 2;

    assert(criarFase(&fase, &alternativa));
    assert(strcmp(fase.titulo, "Fase configurada para teste") == 0);
    assert(strcmp(fase.objetivo.nomeAlvo, "simbolo") == 0);
    assert(fase.objetivo.necessario == 2);
    assert(fase.quantidadeInimigos == 2);
    assert(fase.pontosInimigos[0].x == -2.0f);
    assert(fase.pontosInimigos[1].x == 2.0f);

    segundo = criarPersonagemConfigurada(
        &alternativa.encontro.inimigos[1].personagem
    );
    assert(strcmp(segundo.nome, "Harpia") == 0);
    assert(segundo.vida == 80);
}

int main(void)
{
    testarTabelaPrincipal();
    testarFasesMitologicas();
    testarConfiguracaoAlternativa();

    puts("Testes das configuracoes de fase passaram.");
    return 0;
}

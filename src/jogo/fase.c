#include "fase.h"

#include <stdio.h>
#include <string.h>

enum {
    ITENS_NECESSARIOS = 3
};

static const float DISTANCIA_INTERACAO = 3.0f;
static const float FOLGA_MIRA = 0.12f;
static const float TEMPO_MENSAGEM = 3.5f;

static const Vector3 TAMANHO_ITEM = { 0.9f, 0.3f, 0.3f };

static void definirMensagem(Fase *fase, const char *texto)
{
    snprintf(fase->mensagem, sizeof fase->mensagem, "%s", texto);
    fase->tempoMensagem = TEMPO_MENSAGEM;
}

static BoundingBox caixaDoItem(const ItemFase *item, float folga)
{
    const Vector3 meio = {
        TAMANHO_ITEM.x / 2 + folga,
        TAMANHO_ITEM.y / 2 + folga,
        TAMANHO_ITEM.z / 2 + folga
    };

    return (BoundingBox){
        { item->posicao.x - meio.x, item->posicao.y - meio.y, item->posicao.z - meio.z },
        { item->posicao.x + meio.x, item->posicao.y + meio.y, item->posicao.z + meio.z }
    };
}

static void adicionarItem(Fase *fase, Vector3 posicao)
{
    if (fase->quantidadeItens >= FASE_MAX_ITENS) {
        return;
    }
    fase->itens[fase->quantidadeItens++] = (ItemFase){ posicao, false };
}

static void montarCenario(Fase *fase)
{
    Cenario *c = &fase->cenario;
    const Color pedra = { 150, 135, 110, 255 };
    const Color pedraEscura = { 120, 108, 90, 255 };
    const Color portao = { 140, 50, 40, 255 };

    limparCenario(c);

    /* Limites da arena (40x40). A parede norte tem uma abertura de 4 m
     * em x = 0, fechada pelo portao ate o confronto terminar. */
    adicionarCaixaCenario(c, (Vector3){ 0, 2, 20.5f }, (Vector3){ 42, 4, 1 }, pedra);
    adicionarCaixaCenario(c, (Vector3){ -20.5f, 2, 0 }, (Vector3){ 1, 4, 42 }, pedra);
    adicionarCaixaCenario(c, (Vector3){ 20.5f, 2, 0 }, (Vector3){ 1, 4, 42 }, pedra);
    adicionarCaixaCenario(c, (Vector3){ -11.5f, 2, -20.5f }, (Vector3){ 19, 4, 1 }, pedra);
    adicionarCaixaCenario(c, (Vector3){ 11.5f, 2, -20.5f }, (Vector3){ 19, 4, 1 }, pedra);

    /* Alcova da saida, atras do portao */
    adicionarCaixaCenario(c, (Vector3){ 0, 2, -23.5f }, (Vector3){ 5, 4, 1 }, pedraEscura);
    adicionarCaixaCenario(c, (Vector3){ -2.5f, 2, -22 }, (Vector3){ 1, 4, 4 }, pedraEscura);
    adicionarCaixaCenario(c, (Vector3){ 2.5f, 2, -22 }, (Vector3){ 1, 4, 4 }, pedraEscura);

    fase->indicePortao = adicionarCaixaCenario(
        c, (Vector3){ 0, 2, -20.5f }, (Vector3){ 4, 4, 1 }, portao
    );

    /* Obstaculos */
    adicionarCaixaCenario(c, (Vector3){ 0, 2, -8 }, (Vector3){ 6, 4, 1 }, pedra);
    adicionarCaixaCenario(c, (Vector3){ -8, 2, 4 }, (Vector3){ 2, 4, 2 }, pedraEscura);
    adicionarCaixaCenario(c, (Vector3){ 8, 2, 4 }, (Vector3){ 2, 4, 2 }, pedraEscura);
    adicionarCaixaCenario(c, (Vector3){ 6, 0.4f, -2 }, (Vector3){ 2, 0.8f, 2 }, pedra);
    adicionarCaixaCenario(c, (Vector3){ 8.5f, 0.8f, -2 }, (Vector3){ 3, 1.6f, 2 }, pedra);
}

void criarFaseBase(Fase *fase)
{
    memset(fase, 0, sizeof *fase);

    fase->titulo = "Fase 1 - A Caverna de Polifemo";
    montarCenario(fase);

    fase->pontoInicial = (Vector3){ 0.0f, 0.0f, 14.0f };

    /* Area de saida dentro da alcova, atras do portao */
    fase->saida = (BoundingBox){
        { -1.8f, 0.0f, -23.0f }, { 1.8f, 3.0f, -21.2f }
    };

    /* Pecas de madeira: uma no alto da plataforma (exige pulo), uma atras
     * de um pilar e uma no canto distante. */
    adicionarItem(fase, (Vector3){ 8.5f, 1.75f, -2.0f });
    adicionarItem(fase, (Vector3){ -9.8f, 0.15f, 2.5f });
    adicionarItem(fase, (Vector3){ -15.0f, 0.15f, -15.0f });
    fase->itemAlvo = -1;

    fase->objetivo = (ObjetivoFase){
        "Encontre pecas de madeira para preparar a estaca",
        "peca de madeira",
        0,
        ITENS_NECESSARIOS,
        false
    };

    /* Palco do confronto: area livre no centro da arena */
    fase->pontoJogadorConfronto = (Vector3){ 0.0f, 0.0f, 6.0f };
    fase->pontosInimigos[0] = (Vector3){ 0.0f, 0.0f, -2.0f };
    fase->quantidadeInimigos = 1;

    fase->etapa = ETAPA_EXPLORACAO;
    definirMensagem(fase, "Explore a caverna e cumpra o objetivo");
}

void atualizarMiraFase(Fase *fase, Ray raio)
{
    fase->itemAlvo = -1;
    if (fase->etapa != ETAPA_EXPLORACAO) {
        return;
    }

    float melhorDistancia = DISTANCIA_INTERACAO;
    int melhorItem = -1;

    for (int i = 0; i < fase->quantidadeItens; i++) {
        if (fase->itens[i].coletado) {
            continue;
        }
        const RayCollision colisao =
            GetRayCollisionBox(raio, caixaDoItem(&fase->itens[i], FOLGA_MIRA));
        if (colisao.hit && colisao.distance <= melhorDistancia) {
            melhorDistancia = colisao.distance;
            melhorItem = i;
        }
    }

    if (melhorItem < 0) {
        return;
    }

    /* Uma parede entre a camera e o item bloqueia a interacao. */
    for (int i = 0; i < fase->cenario.quantidade; i++) {
        const RayCollision parede = GetRayCollisionBox(raio, fase->cenario.caixas[i]);
        if (parede.hit && parede.distance < melhorDistancia - 0.02f) {
            return;
        }
    }

    fase->itemAlvo = melhorItem;
}

static void abrirPortao(Fase *fase)
{
    removerCaixaCenario(&fase->cenario, fase->indicePortao);
}

/* Soma uma coleta ao objetivo e, ao completa-lo, inicia o confronto. */
static void registrarColeta(Fase *fase)
{
    ObjetivoFase *objetivo = &fase->objetivo;

    objetivo->progresso++;
    if (objetivo->progresso < objetivo->necessario) {
        char texto[FASE_TAMANHO_MENSAGEM];
        snprintf(
            texto, sizeof texto, "Coletou %s (%d/%d)",
            objetivo->nomeItem, objetivo->progresso, objetivo->necessario
        );
        definirMensagem(fase, texto);
        return;
    }

    objetivo->cumprido = true;
    fase->etapa = ETAPA_CONFRONTO;
    definirMensagem(fase, "Estaca pronta! Polifemo surge diante de voce");
}

bool interagir(Fase *fase)
{
    if (fase->etapa != ETAPA_EXPLORACAO || fase->itemAlvo < 0) {
        return false;
    }

    fase->itens[fase->itemAlvo].coletado = true;
    fase->itemAlvo = -1;
    registrarColeta(fase);
    return true;
}

void encerrarConfronto(Fase *fase)
{
    if (fase->etapa != ETAPA_CONFRONTO) {
        return;
    }

    fase->etapa = ETAPA_SAIDA_LIBERADA;
    abrirPortao(fase);
    definirMensagem(fase, "Polifemo foi vencido. A passagem da caverna se abriu!");
}

void atualizarFase(Fase *fase, Vector3 posicaoJogador, float deltaTempo)
{
    if (fase->tempoMensagem > 0.0f) {
        fase->tempoMensagem -= deltaTempo;
    }

    if (fase->etapa != ETAPA_SAIDA_LIBERADA) {
        return;
    }

    const BoundingBox s = fase->saida;
    if (posicaoJogador.x >= s.min.x && posicaoJogador.x <= s.max.x &&
        posicaoJogador.z >= s.min.z && posicaoJogador.z <= s.max.z &&
        posicaoJogador.y >= s.min.y && posicaoJogador.y <= s.max.y) {
        fase->etapa = ETAPA_CONCLUIDA;
        definirMensagem(fase, "Fase concluida! A jornada continua...");
    }
}

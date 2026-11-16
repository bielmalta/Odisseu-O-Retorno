#include "fase.h"

#include <stdio.h>
#include <string.h>

static const float DISTANCIA_INTERACAO = 3.0f;
static const float FOLGA_MIRA = 0.12f;
static const float TEMPO_MENSAGEM = 3.5f;

static void definirMensagem(Fase *fase, const char *texto)
{
    if (fase == NULL) {
        return;
    }

    snprintf(
        fase->mensagem,
        sizeof fase->mensagem,
        "%s",
        texto != NULL ? texto : ""
    );
    fase->tempoMensagem = TEMPO_MENSAGEM;
}

static BoundingBox caixaDoItem(
    const Fase *fase,
    const ItemFase *item,
    float folga
)
{
    const Vector3 tamanho = fase->configuracao->tamanhoItem;
    const Vector3 meio = {
        tamanho.x / 2.0f + folga,
        tamanho.y / 2.0f + folga,
        tamanho.z / 2.0f + folga
    };

    return (BoundingBox){
        {
            item->posicao.x - meio.x,
            item->posicao.y - meio.y,
            item->posicao.z - meio.z
        },
        {
            item->posicao.x + meio.x,
            item->posicao.y + meio.y,
            item->posicao.z + meio.z
        }
    };
}

static bool configuracaoValida(const ConfiguracaoFase *configuracao)
{
    if (configuracao == NULL || configuracao->titulo == NULL ||
        configuracao->quantidadeCaixas < 0 ||
        configuracao->quantidadeCaixas > CENARIO_MAX_CAIXAS ||
        (configuracao->quantidadeCaixas > 0 && configuracao->caixas == NULL) ||
        configuracao->quantidadeItens < 0 ||
        configuracao->quantidadeItens > FASE_MAX_ITENS ||
        (configuracao->quantidadeItens > 0 && configuracao->itens == NULL) ||
        configuracao->encontro.inimigos == NULL ||
        configuracao->encontro.quantidadeInimigos <= 0 ||
        configuracao->encontro.quantidadeInimigos > FASE_MAX_INIMIGOS) {
        return false;
    }

    return true;
}

bool criarFase(Fase *fase, const ConfiguracaoFase *configuracao)
{
    if (fase == NULL || !configuracaoValida(configuracao)) {
        return false;
    }

    memset(fase, 0, sizeof *fase);
    fase->configuracao = configuracao;
    fase->titulo = configuracao->titulo;
    fase->pontoInicial = configuracao->pontoInicial;
    fase->saida = configuracao->saida;
    fase->indicePortao = -1;
    fase->itemAlvo = -1;

    limparCenario(&fase->cenario);
    configurarChaoCenario(
        &fase->cenario,
        configuracao->tamanhoChao,
        configuracao->corChao,
        configuracao->divisoesGrade
    );

    for (int i = 0; i < configuracao->quantidadeCaixas; i++) {
        const ConfiguracaoCaixaCenario *caixa = &configuracao->caixas[i];
        const int indice = adicionarCaixaCenario(
            &fase->cenario, caixa->centro, caixa->tamanho, caixa->cor
        );

        if (caixa->portao) {
            fase->indicePortao = indice;
        }
    }

    for (int i = 0; i < configuracao->quantidadeItens; i++) {
        fase->itens[i] = (ItemFase){ configuracao->itens[i].posicao, false };
    }
    fase->quantidadeItens = configuracao->quantidadeItens;

    fase->objetivo = criarObjetivo(&configuracao->objetivo);
    iniciarObjetivo(&fase->objetivo);

    fase->pontoJogadorConfronto = configuracao->encontro.pontoJogador;
    fase->quantidadeInimigos = configuracao->encontro.quantidadeInimigos;
    for (int i = 0; i < fase->quantidadeInimigos; i++) {
        fase->pontosInimigos[i] = configuracao->encontro.inimigos[i].posicao;
    }

    fase->etapa = ETAPA_EXPLORACAO;
    definirMensagem(fase, configuracao->instrucaoInicial);
    return true;
}

void criarFaseBase(Fase *fase)
{
    const ConfiguracaoFase *configuracao = obterConfiguracaoFase(0);

    if (!criarFase(fase, configuracao) && fase != NULL) {
        memset(fase, 0, sizeof *fase);
        fase->itemAlvo = -1;
        fase->indicePortao = -1;
    }
}

void atualizarMiraFase(Fase *fase, Ray raio)
{
    float melhorDistancia = DISTANCIA_INTERACAO;
    int melhorItem = -1;

    if (fase == NULL) {
        return;
    }

    fase->itemAlvo = -1;
    if (fase->etapa != ETAPA_EXPLORACAO) {
        return;
    }

    for (int i = 0; i < fase->quantidadeItens; i++) {
        if (fase->itens[i].concluido) {
            continue;
        }

        const RayCollision colisao = GetRayCollisionBox(
            raio, caixaDoItem(fase, &fase->itens[i], FOLGA_MIRA)
        );
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
        const RayCollision parede = GetRayCollisionBox(
            raio, fase->cenario.caixas[i]
        );
        if (parede.hit && parede.distance < melhorDistancia - 0.02f) {
            return;
        }
    }

    fase->itemAlvo = melhorItem;
}

static void abrirPortao(Fase *fase)
{
    if (fase->indicePortao >= 0) {
        removerCaixaCenario(&fase->cenario, fase->indicePortao);
    }
}

static void registrarInteracao(Fase *fase)
{
    Objetivo *objetivo = &fase->objetivo;
    const char *verbo = objetivo->tipo == OBJETIVO_COLETA
                            ? "Coletou"
                            : "Ativou";

    registrarProgresso(objetivo, 1);
    if (!verificarObjetivo(objetivo)) {
        char texto[FASE_TAMANHO_MENSAGEM];
        snprintf(
            texto,
            sizeof texto,
            "%s %s (%d/%d)",
            verbo,
            objetivo->nomeAlvo,
            objetivo->progresso,
            objetivo->necessario
        );
        definirMensagem(fase, texto);
        return;
    }

    fase->etapa = ETAPA_CONFRONTO;
    definirMensagem(fase, fase->configuracao->mensagemObjetivoConcluido);
}

bool interagir(Fase *fase)
{
    if (fase == NULL || fase->etapa != ETAPA_EXPLORACAO ||
        (fase->objetivo.tipo != OBJETIVO_COLETA &&
         fase->objetivo.tipo != OBJETIVO_INTERACAO) ||
        fase->itemAlvo < 0) {
        return false;
    }

    fase->itens[fase->itemAlvo].concluido = true;
    fase->itemAlvo = -1;
    registrarInteracao(fase);
    return true;
}

void encerrarConfronto(Fase *fase)
{
    if (fase == NULL || fase->etapa != ETAPA_CONFRONTO) {
        return;
    }

    fase->etapa = ETAPA_SAIDA_LIBERADA;
    abrirPortao(fase);
    definirMensagem(
        fase,
        fase->configuracao->encontro.recompensaNarrativa
    );
}

void atualizarFase(Fase *fase, Vector3 posicaoJogador, float deltaTempo)
{
    if (fase == NULL) {
        return;
    }

    if (fase->tempoMensagem > 0.0f) {
        fase->tempoMensagem -= deltaTempo;
    }

    if (fase->etapa != ETAPA_SAIDA_LIBERADA) {
        return;
    }

    const BoundingBox saida = fase->saida;
    if (posicaoJogador.x >= saida.min.x &&
        posicaoJogador.x <= saida.max.x &&
        posicaoJogador.z >= saida.min.z &&
        posicaoJogador.z <= saida.max.z &&
        posicaoJogador.y >= saida.min.y &&
        posicaoJogador.y <= saida.max.y) {
        fase->etapa = ETAPA_CONCLUIDA;
        definirMensagem(fase, fase->configuracao->mensagemFaseConcluida);
    }
}

#include "painel_combate.h"

#include <stdio.h>
#include <string.h>

void iniciarPainelCombate(PainelCombate *painel, const Combate *combate)
{
    const NoTurno *no;

    if (painel == NULL) {
        return;
    }

    memset(painel, 0, sizeof *painel);

    if (combate == NULL) {
        return;
    }

    no = combate->turnos.inicio;
    for (int i = 0; i < combate->turnos.tamanho; i++) {
        if (no->personagem->tipo == TIPO_INIMIGO &&
            painel->quantidadeInimigos < PAINEL_MAX_INIMIGOS) {
            painel->inimigos[painel->quantidadeInimigos++] = no->personagem;
        }
        no = no->proximo;
    }

    atualizarAlvoPainel(painel);
}

void atualizarAlvoPainel(PainelCombate *painel)
{
    if (painel == NULL) {
        return;
    }

    if (painel->alvo != NULL && personagemVivo(painel->alvo)) {
        return;
    }

    painel->alvo = NULL;
    for (int i = 0; i < painel->quantidadeInimigos; i++) {
        if (personagemVivo(painel->inimigos[i])) {
            painel->alvo = painel->inimigos[i];
            return;
        }
    }
}

bool selecionarAlvoPainel(PainelCombate *painel, int indice)
{
    if (painel == NULL || indice < 0 || indice >= painel->quantidadeInimigos ||
        !personagemVivo(painel->inimigos[indice])) {
        return false;
    }

    painel->alvo = painel->inimigos[indice];
    return true;
}

void mudarAlvoPainel(PainelCombate *painel, int passo)
{
    int posicao = -1;
    const int quantidade = painel != NULL ? painel->quantidadeInimigos : 0;
    const int direcao = passo > 0 ? 1 : -1;

    if (quantidade == 0 || passo == 0) {
        return;
    }

    for (int i = 0; i < quantidade; i++) {
        if (painel->inimigos[i] == painel->alvo) {
            posicao = i;
        }
    }

    /* Sem alvo atual, comeca pela ponta de onde o passo sai. */
    if (posicao < 0) {
        posicao = direcao > 0 ? quantidade - 1 : 0;
    }

    for (int tentativa = 0; tentativa < quantidade; tentativa++) {
        posicao = (posicao + direcao + quantidade) % quantidade;

        if (personagemVivo(painel->inimigos[posicao])) {
            painel->alvo = painel->inimigos[posicao];
            return;
        }
    }
}

void registrarMensagemPainel(PainelCombate *painel, const char *texto)
{
    if (painel == NULL || texto == NULL) {
        return;
    }

    if (painel->quantidadeMensagens == PAINEL_MAX_MENSAGENS) {
        memmove(
            painel->mensagens[0],
            painel->mensagens[1],
            (size_t)(PAINEL_MAX_MENSAGENS - 1) * PAINEL_TAMANHO_MENSAGEM
        );
        painel->quantidadeMensagens--;
    }

    snprintf(
        painel->mensagens[painel->quantidadeMensagens],
        PAINEL_TAMANHO_MENSAGEM, "%s", texto
    );
    painel->quantidadeMensagens++;
}

int montarOrdemTurnos(const Combate *combate, TurnoPainel *saida, int capacidade)
{
    Personagem *ordem[PAINEL_MAX_TURNOS];
    int quantidade;
    int posicaoDaVez = -1;

    if (combate == NULL || saida == NULL || capacidade <= 0) {
        return 0;
    }

    quantidade = percorrerParticipantes(
        &combate->turnos, ordem,
        capacidade < PAINEL_MAX_TURNOS ? capacidade : PAINEL_MAX_TURNOS
    );

    for (int i = 0; i < quantidade; i++) {
        if (combate->participanteAtual != NULL &&
            ordem[i] == combate->participanteAtual) {
            posicaoDaVez = i;
        }
    }

    for (int i = 0; i < quantidade; i++) {
        saida[i].personagem = ordem[i];

        if (posicaoDaVez < 0 || i > posicaoDaVez) {
            saida[i].situacao = TURNO_AGUARDANDO;
        } else if (i == posicaoDaVez) {
            saida[i].situacao = TURNO_ATUAL;
        } else {
            saida[i].situacao = TURNO_JA_AGIU;
        }
    }

    return quantidade;
}

void observarEventoPainel(const EventoCombate *evento, void *contexto)
{
    PainelCombate *painel = contexto;
    char texto[PAINEL_TAMANHO_MENSAGEM];

    if (painel == NULL || evento == NULL) {
        return;
    }

    if (evento->tipo == EVENTO_COMBATE_DEFESA) {
        snprintf(texto, sizeof texto, "%s se defendeu", evento->origem->nome);
        registrarMensagemPainel(painel, texto);
        return;
    }

    if (evento->tipo == EVENTO_COMBATE_PREPARO_PESADO) {
        snprintf(texto, sizeof texto, "%s prepara um golpe pesado!",
                 evento->origem->nome);
        registrarMensagemPainel(painel, texto);
        return;
    }

    if (evento->tipo == EVENTO_COMBATE_ENCANTO) {
        snprintf(texto, sizeof texto, "%s entoa um canto encantador!",
                 evento->origem->nome);
        registrarMensagemPainel(painel, texto);
        return;
    }

    if (evento->tipo == EVENTO_COMBATE_PROTECAO) {
        snprintf(texto, sizeof texto, "%s protegeu %s",
                 evento->origem->nome, evento->alvo->nome);
        registrarMensagemPainel(painel, texto);
        return;
    }

    if (evento->alvoDefendia) {
        snprintf(texto, sizeof texto, "%.22s%s atacou %.22s: %d de dano (defendido)",
                 evento->origem->nome,
                 evento->tipo == EVENTO_COMBATE_ATAQUE_PESADO ? " [PESADO]" : "",
                 evento->alvo->nome, evento->dano);
    } else if (evento->tipo == EVENTO_COMBATE_ATAQUE_PESADO) {
        snprintf(texto, sizeof texto, "%s usou GOLPE PESADO em %s: %d de dano",
                 evento->origem->nome, evento->alvo->nome, evento->dano);
    } else {
        snprintf(texto, sizeof texto, "%s atacou %s: %d de dano",
                 evento->origem->nome, evento->alvo->nome, evento->dano);
    }
    registrarMensagemPainel(painel, texto);

    if (evento->alvoDerrotado) {
        snprintf(texto, sizeof texto, "%s foi derrotado!", evento->alvo->nome);
        registrarMensagemPainel(painel, texto);
    }
}

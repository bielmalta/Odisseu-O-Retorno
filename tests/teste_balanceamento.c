#undef NDEBUG
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "acoes_combate.h"
#include "combate.h"
#include "configuracao_fases.h"
#include "ia_inimigo.h"
#include "personagem.h"

/* Balanceamento dos encontros (commit 26).
 *
 * Simula as quatro fases com os atributos reais de configuracao_fases.c,
 * usando o controlador de combate, a IA dos inimigos e a lista circular de
 * verdade. Nao abre janela Raylib. Os limites abaixo sao os criterios de
 * aceite: se alguem mudar um atributo e quebrar a jornada, este teste falha.
 */

#define RODADAS_MINIMAS 4  /* menos que isso, a luta acaba antes de ensinar */
#define RODADAS_MAXIMAS 10 /* mais que isso, a luta fica arrastada */
#define MAXIMO_INIMIGOS 8

typedef enum {
    JOGADOR_ATACA_SEMPRE, /* jogador novo: so aperta atacar */
    JOGADOR_SO_DEFENDE
} EstrategiaJogador;

typedef struct {
    EstadoCombate resultado;
    unsigned int rodadas;
    int danoSofrido;
    int posicaoOdisseu; /* 1 = age primeiro na rodada 1 */
} ResultadoLuta;

static Personagem *primeiroInimigoVivo(Personagem **inimigos, int quantidade)
{
    for (int i = 0; i < quantidade; i++) {
        if (personagemVivo(inimigos[i])) {
            return inimigos[i];
        }
    }
    return NULL;
}

static int posicaoNaOrdem(const Combate *combate, const Personagem *alvo)
{
    const NoTurno *no = combate->turnos.inicio;

    for (int i = 0; i < combate->turnos.tamanho; i++) {
        if (no->personagem == alvo) {
            return i + 1;
        }
        no = no->proximo;
    }
    return 0;
}

static ResultadoLuta lutar(Personagem *odisseu,
                           const ConfiguracaoFase *fase,
                           EstrategiaJogador estrategia)
{
    Personagem inimigos[MAXIMO_INIMIGOS];
    Personagem *ponteiros[MAXIMO_INIMIGOS];
    const int quantidade = fase->encontro.quantidadeInimigos;
    const int vidaInicial = odisseu->vida;
    ResultadoLuta resultado = {0};
    Combate combate;
    int passos = 0;

    assert(quantidade > 0 && quantidade <= MAXIMO_INIMIGOS);
    for (int i = 0; i < quantidade; i++) {
        inimigos[i] = criarPersonagemConfigurada(
            &fase->encontro.inimigos[i].personagem);
        ponteiros[i] = &inimigos[i];
    }

    inicializarCombate(&combate);
    assert(prepararCombate(&combate, odisseu, ponteiros, quantidade));
    configurarIdentidadeCombate(&combate, fase->identidade, false);

    while (combate.estado != COMBATE_VICTORY &&
           combate.estado != COMBATE_DEFEAT) {
        assert(++passos < 10000); /* nenhuma luta pode ser infinita */

        if (combate.estado == COMBATE_WAITING_PLAYER) {
            if (estrategia == JOGADOR_SO_DEFENDE) {
                assert(defenderNoCombate(&combate));
            } else {
                assert(atacarNoCombate(
                    &combate, primeiroInimigoVivo(ponteiros, quantidade)));
            }
        } else {
            atualizarCombate(&combate);
            if (combate.rodada == 1 && resultado.posicaoOdisseu == 0 &&
                combate.estado != COMBATE_ROUND_START) {
                resultado.posicaoOdisseu = posicaoNaOrdem(&combate, odisseu);
            }
        }
    }

    resultado.resultado = combate.estado;
    resultado.rodadas = combate.rodada;
    resultado.danoSofrido = vidaInicial - odisseu->vida;
    finalizarCombate(&combate);
    return resultado;
}

static Personagem criarOdisseu(void)
{
    return criarPersonagemConfigurada(&obterConfiguracaoFase(0)->heroi);
}

/* Odisseu e o mesmo em todas as fases: a jornada preserva o personagem. */
static void testarHeroiConsistente(void)
{
    const ConfiguracaoPersonagem *base = &obterConfiguracaoFase(0)->heroi;

    for (int i = 1; i < quantidadeConfiguracoesFase(); i++) {
        const ConfiguracaoPersonagem *h = &obterConfiguracaoFase(i)->heroi;

        assert(h->vidaMaxima == base->vidaMaxima);
        assert(h->ataque == base->ataque);
        assert(h->defesa == base->defesa);
        assert(h->iniciativa == base->iniciativa);
    }
    puts("ok: Odisseu tem os mesmos atributos em todas as fases");
}

/* Nenhum encontro e impossivel nem arrastado, mesmo isolado. */
static void testarCadaEncontroComVidaCheia(void)
{
    puts("\n  Encontro                    rodadas  dano sofrido  posicao");
    for (int i = 0; i < quantidadeConfiguracoesFase(); i++) {
        const ConfiguracaoFase *fase = obterConfiguracaoFase(i);
        Personagem odisseu = criarOdisseu();
        const ResultadoLuta r = lutar(&odisseu, fase, JOGADOR_ATACA_SEMPRE);

        printf("  %-27s %7u  %12d  %7d\n", fase->encontro.nome,
               r.rodadas, r.danoSofrido, r.posicaoOdisseu);

        assert(r.resultado == COMBATE_VICTORY);
        assert(r.rodadas >= RODADAS_MINIMAS);
        assert(r.rodadas <= RODADAS_MAXIMAS);
        assert(r.danoSofrido > 0); /* todo encontro oferece algum risco */
    }
    puts("ok: todo encontro e vencivel com vida cheia, em 4 a 10 rodadas");
}

/* Jogador novo, so atacando, com a vida carregada de uma fase para a
 * outra (como no jogo), termina a jornada sem perder nenhuma vez. */
static void testarJornadaCompleta(void)
{
    Personagem odisseu = criarOdisseu();
    int danoPorFase[8];
    const int quantidade = quantidadeConfiguracoesFase();

    assert(quantidade <= 8);
    puts("\n  Jornada (vida carregada)    vida ao entrar  vida ao sair");
    for (int i = 0; i < quantidade; i++) {
        const ConfiguracaoFase *fase = obterConfiguracaoFase(i);
        const int vidaAntes = odisseu.vida;
        const ResultadoLuta r = lutar(&odisseu, fase, JOGADOR_ATACA_SEMPRE);

        printf("  %-27s %14d  %12d\n", fase->encontro.nome, vidaAntes,
               odisseu.vida);
        assert(r.resultado == COMBATE_VICTORY);
        danoPorFase[i] = r.danoSofrido;
    }

    /* Chega ao fim vivo, mas sem folga exagerada: o final tem tensao. */
    assert(odisseu.vida > 0);
    assert(odisseu.vida * 100 <= odisseu.vidaMaxima * 30);

    /* O confronto final e o mais perigoso, e a primeira fase nao e a mais
     * dificil (ela ensina os controles). */
    for (int i = 0; i < quantidade - 1; i++) {
        assert(danoPorFase[quantidade - 1] > danoPorFase[i]);
    }
    puts("ok: jornada concluida sem derrota, com o final mais dificil");
}

/* So defender nunca vence: atacar e defender levam a resultados
 * diferentes em todos os encontros. */
static void testarEscolhasFazemDiferenca(void)
{
    for (int i = 0; i < quantidadeConfiguracoesFase(); i++) {
        const ConfiguracaoFase *fase = obterConfiguracaoFase(i);
        Personagem atacante = criarOdisseu();
        Personagem defensor = criarOdisseu();

        assert(lutar(&atacante, fase, JOGADOR_ATACA_SEMPRE).resultado ==
               COMBATE_VICTORY);
        assert(lutar(&defensor, fase, JOGADOR_SO_DEFENDE).resultado ==
               COMBATE_DEFEAT);
    }
    puts("ok: so defender perde, atacar vence");
}

/* A velocidade muda de fato a ordem: Odisseu age primeiro contra
 * Polifemo, no meio contra Sereias e Cila e por ultimo em Itaca. */
static void testarVelocidadePerceptivel(void)
{
    const int quantidade = quantidadeConfiguracoesFase();
    int posicoes[8];

    for (int i = 0; i < quantidade; i++) {
        Personagem odisseu = criarOdisseu();
        posicoes[i] = lutar(&odisseu, obterConfiguracaoFase(i),
                            JOGADOR_ATACA_SEMPRE).posicaoOdisseu;
    }

    assert(posicoes[0] == 1);                     /* Polifemo e lento */
    assert(posicoes[1] > 1);                      /* Ligeia e mais rapida */
    assert(posicoes[2] > 1 &&
           posicoes[2] < 1 + obterConfiguracaoFase(2)->encontro
                                 .quantidadeInimigos);
    assert(posicoes[quantidade - 1] ==            /* pretendentes rapidos */
           1 + obterConfiguracaoFase(quantidade - 1)
                   ->encontro.quantidadeInimigos);

    /* Se Odisseu fosse mais rapido que os pretendentes, agiria primeiro e
     * sofreria menos dano: a velocidade pesa no resultado. */
    {
        Personagem normal = criarOdisseu();
        Personagem veloz = criarOdisseu();
        const ConfiguracaoFase *final = obterConfiguracaoFase(quantidade - 1);
        ResultadoLuta rNormal;
        ResultadoLuta rVeloz;

        veloz.iniciativa = 20;
        rNormal = lutar(&normal, final, JOGADOR_ATACA_SEMPRE);
        rVeloz = lutar(&veloz, final, JOGADOR_ATACA_SEMPRE);
        assert(rVeloz.posicaoOdisseu == 1);
        assert(rVeloz.danoSofrido < rNormal.danoSofrido);
    }
    puts("ok: velocidade altera a ordem e o dano sofrido");
}

int main(void)
{
    setvbuf(stdout, NULL, _IONBF, 0);
    testarHeroiConsistente();
    testarCadaEncontroComVidaCheia();
    testarJornadaCompleta();
    testarEscolhasFazemDiferenca();
    testarVelocidadePerceptivel();

    puts("\ntodos os testes de balanceamento passaram");
    return 0;
}

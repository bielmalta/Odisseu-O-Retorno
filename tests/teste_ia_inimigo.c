#undef NDEBUG
#include <assert.h>
#include <stdio.h>

#include "acoes_combate.h"
#include "combate.h"
#include "ia_inimigo.h"
#include "personagem.h"
#include "ritmo_combate.h"

/* Testes da IA basica dos inimigos (commit 15). Nao abrem janela Raylib. */

#define MAXIMO_EVENTOS 512

typedef struct {
    EventoCombate eventos[MAXIMO_EVENTOS];
    int vidaAlvoAntes[MAXIMO_EVENTOS];
    int quantidade;
} Registro;

static void registrarEvento(const EventoCombate *evento, void *contexto)
{
    Registro *registro = contexto;

    assert(registro->quantidade < MAXIMO_EVENTOS);
    registro->eventos[registro->quantidade] = *evento;
    registro->vidaAlvoAntes[registro->quantidade] =
        evento->alvo->vida + evento->dano;
    registro->quantidade++;
}

static void testarSelecaoDeAlvo(void)
{
    Combate combate;
    Personagem heroiCaido = criarPersonagem("Euriloco", TIPO_HEROI, 50, 5, 5, 9);
    Personagem odisseu = criarPersonagem("Odisseu", TIPO_HEROI, 100, 20, 10, 12);
    Personagem sereia = criarPersonagem("Sereia", TIPO_INIMIGO, 40, 15, 5, 20);
    Personagem cila = criarPersonagem("Cila", TIPO_INIMIGO, 90, 16, 5, 7);

    /* Lista montada a mao para ter dois herois, um deles derrotado. */
    inicializarCombate(&combate);
    assert(inserirPersonagem(&combate.turnos, &sereia));
    assert(inserirPersonagem(&combate.turnos, &heroiCaido));
    assert(inserirPersonagem(&combate.turnos, &cila));
    assert(inserirPersonagem(&combate.turnos, &odisseu));

    heroiCaido.vida = 0;
    assert(selecionarAlvoInimigo(&combate, &sereia) == &odisseu);
    assert(selecionarAlvoInimigo(&combate, &cila) == &odisseu);

    /* Heroi vivo vem antes na lista: ele e o escolhido. */
    heroiCaido.vida = 10;
    assert(selecionarAlvoInimigo(&combate, &sereia) == &heroiCaido);

    /* Ninguem do lado oposto vivo: nenhum alvo. */
    heroiCaido.vida = 0;
    odisseu.vida = 0;
    assert(selecionarAlvoInimigo(&combate, &sereia) == NULL);

    /* Personagem fora da lista nunca e alvo. */
    {
        Personagem forasteiro = criarPersonagem("Forasteiro", TIPO_HEROI,
                                                30, 1, 1, 1);
        assert(selecionarAlvoInimigo(&combate, &sereia) != &forasteiro);
    }

    assert(selecionarAlvoInimigo(NULL, &sereia) == NULL);
    assert(selecionarAlvoInimigo(&combate, NULL) == NULL);

    finalizarCombate(&combate);
    puts("ok: selecao de alvo valido");
}

static void testarEscolhaDaAcao(void)
{
    Personagem odisseu = criarPersonagem("Odisseu", TIPO_HEROI, 100, 20, 10, 12);
    Personagem polifemo = criarPersonagem("Polifemo", TIPO_INIMIGO, 100, 25, 8, 8);

    /* Vida cheia: ataca. */
    assert(escolherAcaoInimigo(&polifemo, &odisseu) == ACAO_INIMIGO_ATACAR);

    /* Exatamente no limiar (30%) e sem guarda: defende. */
    polifemo.vida = 30;
    assert(escolherAcaoInimigo(&polifemo, &odisseu) == ACAO_INIMIGO_DEFENDER);

    /* Logo acima do limiar: ataca. */
    polifemo.vida = 31;
    assert(escolherAcaoInimigo(&polifemo, &odisseu) == ACAO_INIMIGO_ATACAR);

    /* Vida baixa, mas a guarda ainda esta de pe: ataca. */
    polifemo.vida = 10;
    polifemo.bonusDefesa = BONUS_DEFESA_ACAO;
    assert(escolherAcaoInimigo(&polifemo, &odisseu) == ACAO_INIMIGO_ATACAR);

    /* Vida baixa sem guarda, mas o golpe derrota o alvo: ataca. */
    polifemo.bonusDefesa = 0;
    odisseu.vida = 15; /* dano = 25 - 10 = 15 */
    assert(escolherAcaoInimigo(&polifemo, &odisseu) == ACAO_INIMIGO_ATACAR);

    /* Se o alvo estiver em guarda, o golpe ja nao e fatal: defende. */
    odisseu.bonusDefesa = BONUS_DEFESA_ACAO;
    assert(escolherAcaoInimigo(&polifemo, &odisseu) == ACAO_INIMIGO_DEFENDER);

    puts("ok: escolha entre atacar e defender");
}

static void testarTurnoAutomatico(void)
{
    Combate combate;
    Registro registro = {0};
    Personagem odisseu = criarPersonagem("Odisseu", TIPO_HEROI, 100, 20, 10, 5);
    Personagem polifemo = criarPersonagem("Polifemo", TIPO_INIMIGO, 120, 25, 8, 9);
    Personagem *inimigos[] = {&polifemo};

    registrarObservadorCombate(registrarEvento, &registro);
    inicializarCombate(&combate);
    assert(prepararCombate(&combate, &odisseu, inimigos, 1));

    /* Fora do turno inimigo a IA nao age. */
    assert(!executarTurnoInimigo(&combate));
    assert(!executarTurnoInimigo(NULL));

    atualizarCombate(&combate); /* rodada 1: Polifemo e mais rapido */
    assert(combate.estado == COMBATE_ENEMY_TURN);
    assert(combate.participanteAtual == &polifemo);

    atualizarCombate(&combate); /* o inimigo age sem intervencao */
    assert(combate.estado == COMBATE_RESOLVING);
    assert(combate.turnosConcluidos == 1);
    assert(odisseu.vida == 85);
    assert(registro.quantidade == 1);
    assert(registro.eventos[0].tipo == EVENTO_COMBATE_ATAQUE);
    assert(registro.eventos[0].origem == &polifemo);
    assert(registro.eventos[0].alvo == &odisseu);

    atualizarCombate(&combate); /* passa a vez a Odisseu */
    assert(combate.estado == COMBATE_WAITING_PLAYER);
    assert(!executarTurnoInimigo(&combate));

    registrarObservadorCombate(NULL, NULL);
    finalizarCombate(&combate);
    puts("ok: inimigo conclui o turno sozinho");
}

static void testarIdentidadesDosEncontros(void)
{
    /* Polifemo anuncia o golpe; defender transforma o impacto em dano minimo. */
    {
        Combate combate;
        Personagem odisseu = criarPersonagem("Odisseu", TIPO_HEROI, 170, 20, 10, 12);
        Personagem polifemo = criarPersonagem("Polifemo", TIPO_INIMIGO, 84, 19, 6, 8);
        Personagem *inimigos[] = { &polifemo };

        inicializarCombate(&combate);
        assert(prepararCombate(&combate, &odisseu, inimigos, 1));
        configurarIdentidadeCombate(&combate, IDENTIDADE_POLIFEMO, false);
        combate.estado = COMBATE_ENEMY_TURN;
        combate.participanteAtual = &polifemo;
        combate.rodada = 2;
        assert(prepararGolpePesadoNoCombate(&combate));
        assert(combate.golpePesadoPreparado);
        assert(intencaoEspecialInimigo(&combate) != NULL);

        odisseu.bonusDefesa = BONUS_DEFESA_ACAO;
        combate.estado = COMBATE_ENEMY_TURN;
        combate.participanteAtual = &polifemo;
        assert(golpePesadoNoCombate(&combate, &odisseu));
        assert(odisseu.vida == 169);
        assert(!combate.golpePesadoPreparado);
        finalizarCombate(&combate);
    }

    /* O canto cria uma penalidade para a proxima ordenacao da lista. */
    {
        Combate combate;
        Personagem odisseu = criarPersonagem("Odisseu", TIPO_HEROI, 170, 20, 10, 12);
        Personagem sereia = criarPersonagem("Sereia de Ligeia", TIPO_INIMIGO, 36, 16, 4, 15);
        Personagem *inimigos[] = { &sereia };

        inicializarCombate(&combate);
        assert(prepararCombate(&combate, &odisseu, inimigos, 1));
        configurarIdentidadeCombate(&combate, IDENTIDADE_SEREIAS, false);
        combate.estado = COMBATE_ENEMY_TURN;
        combate.participanteAtual = &sereia;
        combate.rodada = 2;
        assert(enfeiticarNoCombate(&combate, &odisseu));
        assert(combate.penalidadeIniciativaPendente == -PENALIDADE_ENCANTO_SEREIA);
        assert(combate.rodadaUltimoEncanto == 2);
        finalizarCombate(&combate);
    }

    /* Em Cila, remover um tentaculo acelera e fortalece os restantes. */
    {
        Combate combate;
        Personagem odisseu = criarPersonagem("Odisseu", TIPO_HEROI, 170, 50, 10, 30);
        Personagem t1 = criarPersonagem("Tentaculo I", TIPO_INIMIGO, 20, 10, 2, 10);
        Personagem t2 = criarPersonagem("Tentaculo II", TIPO_INIMIGO, 30, 11, 3, 9);
        Personagem *inimigos[] = { &t1, &t2 };

        inicializarCombate(&combate);
        assert(prepararCombate(&combate, &odisseu, inimigos, 2));
        configurarIdentidadeCombate(&combate, IDENTIDADE_CILA, false);
        atualizarCombate(&combate);
        assert(combate.estado == COMBATE_WAITING_PLAYER);
        assert(atacarNoCombate(&combate, &t1));
        assert(!personagemVivo(&t1));
        assert(t2.ataque == 11);
        assert(t2.iniciativa == 11);
        assert(combate.inimigosDerrotados == 1);
        finalizarCombate(&combate);
    }

    /* Em Itaca, um pretendente pode gastar a vez protegendo o aliado. */
    {
        Combate combate;
        Personagem odisseu = criarPersonagem("Odisseu", TIPO_HEROI, 170, 20, 10, 12);
        Personagem antinoo = criarPersonagem("Antinoo", TIPO_INIMIGO, 45, 15, 5, 13);
        Personagem eurimaco = criarPersonagem("Eurimaco", TIPO_INIMIGO, 52, 16, 5, 14);
        Personagem *inimigos[] = { &antinoo, &eurimaco };

        antinoo.vida = 20;
        inicializarCombate(&combate);
        assert(prepararCombate(&combate, &odisseu, inimigos, 2));
        configurarIdentidadeCombate(&combate, IDENTIDADE_ITACA, false);
        combate.estado = COMBATE_ENEMY_TURN;
        combate.participanteAtual = &eurimaco;
        assert(executarTurnoInimigo(&combate));
        assert(antinoo.bonusDefesa == BONUS_PROTECAO_ALIADO);
        assert(combate.estado == COMBATE_RESOLVING);
        finalizarCombate(&combate);
    }

    puts("ok: identidades de Polifemo, Sereias, Cila e Itaca");
}

/* Simula um combate inteiro. O jogador segue uma estrategia fixa e a IA
 * cuida dos inimigos. Devolve o estado final. */
static EstadoCombate simularCombate(Personagem *odisseu,
                                    Personagem **inimigos,
                                    int quantidade,
                                    bool jogadorSoDefende,
                                    Registro *registro)
{
    Combate combate;
    EstadoCombate final;
    int passos = 0;

    registro->quantidade = 0;
    registrarObservadorCombate(registrarEvento, registro);
    inicializarCombate(&combate);
    assert(prepararCombate(&combate, odisseu, inimigos, quantidade));

    while (combate.estado != COMBATE_VICTORY &&
           combate.estado != COMBATE_DEFEAT) {
        assert(++passos < 5000); /* sem laco infinito */
        assert(listaConsistente(&combate.turnos));

        if (combate.estado == COMBATE_WAITING_PLAYER) {
            if (jogadorSoDefende) {
                assert(defenderNoCombate(&combate));
            } else {
                Personagem *alvo = NULL;
                const NoTurno *no = combate.turnos.inicio;

                for (int i = 0; i < combate.turnos.tamanho; i++) {
                    if (no->personagem->tipo == TIPO_INIMIGO &&
                        personagemVivo(no->personagem)) {
                        alvo = no->personagem;
                        break;
                    }
                    no = no->proximo;
                }
                assert(alvo != NULL);
                assert(atacarNoCombate(&combate, alvo));
            }
        } else {
            atualizarCombate(&combate);
        }
    }

    final = combate.estado;
    registrarObservadorCombate(NULL, NULL);
    finalizarCombate(&combate);
    return final;
}

static void verificarAlvosDosInimigos(const Registro *registro)
{
    for (int i = 0; i < registro->quantidade; i++) {
        const EventoCombate *e = &registro->eventos[i];

        if (e->origem->tipo != TIPO_INIMIGO ||
            e->tipo != EVENTO_COMBATE_ATAQUE) {
            continue;
        }

        /* Nunca atacou derrotado nem aliado. */
        assert(e->alvo->tipo == TIPO_HEROI);
        assert(registro->vidaAlvoAntes[i] > 0);
        assert(e->dano >= DANO_MINIMO);
    }
}

static void testarCombatesCompletos(void)
{
    Registro registro;

    /* Odisseu forte vence dois inimigos. */
    {
        Personagem odisseu = criarPersonagem("Odisseu", TIPO_HEROI, 100, 30, 10, 12);
        Personagem sereia = criarPersonagem("Sereia", TIPO_INIMIGO, 40, 15, 5, 20);
        Personagem cila = criarPersonagem("Cila", TIPO_INIMIGO, 90, 16, 5, 7);
        Personagem *inimigos[] = {&sereia, &cila};

        assert(simularCombate(&odisseu, inimigos, 2, false, &registro) ==
               COMBATE_VICTORY);
        verificarAlvosDosInimigos(&registro);
    }

    /* Odisseu fraco e derrotado; a IA nunca ataca alguem ja caido. */
    {
        Personagem odisseu = criarPersonagem("Odisseu", TIPO_HEROI, 30, 5, 2, 1);
        Personagem polifemo = criarPersonagem("Polifemo", TIPO_INIMIGO, 200, 25, 8, 9);
        Personagem *inimigos[] = {&polifemo};

        assert(simularCombate(&odisseu, inimigos, 1, false, &registro) ==
               COMBATE_DEFEAT);
        assert(odisseu.vida == 0);
        verificarAlvosDosInimigos(&registro);
    }

    puts("ok: combates completos terminam sem alvo invalido");
}

/* Com vida baixa e o jogador so defendendo, o inimigo alterna defesa e
 * ataque: nunca fica preso defendendo para sempre. */
static void testarSemDefesaInfinita(void)
{
    Registro registro;
    Personagem odisseu = criarPersonagem("Odisseu", TIPO_HEROI, 100, 20, 10, 5);
    Personagem polifemo = criarPersonagem("Polifemo", TIPO_INIMIGO, 100, 25, 8, 9);
    Personagem *inimigos[] = {&polifemo};
    int defesasInimigo = 0;
    int ataquesInimigo = 0;

    polifemo.vida = 20; /* abaixo do limiar desde o inicio */

    assert(simularCombate(&odisseu, inimigos, 1, true, &registro) ==
           COMBATE_DEFEAT);

    for (int i = 0; i < registro.quantidade; i++) {
        const EventoCombate *e = &registro.eventos[i];

        if (e->origem != &polifemo) {
            continue;
        }

        if (e->tipo == EVENTO_COMBATE_DEFESA) {
            defesasInimigo++;
        } else {
            ataquesInimigo++;
        }

        /* Duas defesas seguidas do inimigo nao acontecem. */
        if (e->tipo == EVENTO_COMBATE_DEFESA) {
            for (int j = i + 1; j < registro.quantidade; j++) {
                if (registro.eventos[j].origem == &polifemo) {
                    assert(registro.eventos[j].tipo == EVENTO_COMBATE_ATAQUE);
                    break;
                }
            }
        }
    }

    assert(defesasInimigo > 0);
    assert(ataquesInimigo > 0);
    puts("ok: inimigo nao defende para sempre");
}

static void testarReprodutibilidade(void)
{
    Registro primeiro;
    Registro segundo;

    for (int vez = 0; vez < 2; vez++) {
        Personagem odisseu = criarPersonagem("Odisseu", TIPO_HEROI, 100, 22, 10, 12);
        Personagem sereia = criarPersonagem("Sereia", TIPO_INIMIGO, 40, 15, 5, 20);
        Personagem polifemo = criarPersonagem("Polifemo", TIPO_INIMIGO, 120, 25, 8, 8);
        Personagem *inimigos[] = {&sereia, &polifemo};

        simularCombate(&odisseu, inimigos, 2, false,
                       vez == 0 ? &primeiro : &segundo);
    }

    assert(primeiro.quantidade == segundo.quantidade);
    for (int i = 0; i < primeiro.quantidade; i++) {
        assert(primeiro.eventos[i].tipo == segundo.eventos[i].tipo);
        assert(primeiro.eventos[i].dano == segundo.eventos[i].dano);
        assert(primeiro.vidaAlvoAntes[i] == segundo.vidaAlvoAntes[i]);
    }

    puts("ok: mesma situacao gera as mesmas decisoes");
}

static void testarIntervaloVisual(void)
{
    Combate combate;
    EsperaTurnoInimigo espera;
    Personagem odisseu = criarPersonagem("Odisseu", TIPO_HEROI, 100, 20, 10, 5);
    /* 10 > 5 + BONUS_INICIATIVA_DEFESA: defender nao tira a vez do inimigo. */
    Personagem polifemo = criarPersonagem("Polifemo", TIPO_INIMIGO, 120, 25, 8, 10);
    Personagem *inimigos[] = {&polifemo};

    iniciarEsperaTurnoInimigo(&espera);
    inicializarCombate(&combate);
    assert(prepararCombate(&combate, &odisseu, inimigos, 1));

    /* Fora do turno inimigo nao ha espera. */
    assert(aguardarTurnoInimigo(&espera, &combate, 0.0f));
    atualizarCombate(&combate);
    assert(combate.estado == COMBATE_ENEMY_TURN);

    /* No turno inimigo, so libera depois do intervalo. */
    assert(!aguardarTurnoInimigo(&espera, &combate, 0.0f));
    assert(!aguardarTurnoInimigo(&espera, &combate, 0.4f));
    assert(!aguardarTurnoInimigo(&espera, &combate, -1.0f));
    assert(odisseu.vida == 100); /* a espera nao executa nada */
    assert(aguardarTurnoInimigo(&espera, &combate, 0.4f));

    atualizarCombate(&combate);
    assert(odisseu.vida < 100);

    /* Saindo do turno inimigo a contagem zera. */
    atualizarCombate(&combate);
    assert(aguardarTurnoInimigo(&espera, &combate, 0.0f));
    assert(espera.decorrido == 0.0f);

    /* Rodada seguinte: novo turno inimigo, nova espera. */
    assert(defenderNoCombate(&combate));
    atualizarCombate(&combate); /* fim da rodada */
    atualizarCombate(&combate); /* rodada 2 */
    assert(combate.estado == COMBATE_ENEMY_TURN);
    assert(!aguardarTurnoInimigo(&espera, &combate, 0.1f));

    assert(aguardarTurnoInimigo(NULL, &combate, 0.0f));
    finalizarCombate(&combate);
    puts("ok: intervalo visual nao bloqueia a logica");
}

int main(void)
{
    testarSelecaoDeAlvo();
    testarEscolhaDaAcao();
    testarTurnoAutomatico();
    testarIdentidadesDosEncontros();
    testarCombatesCompletos();
    testarSemDefesaInfinita();
    testarReprodutibilidade();
    testarIntervaloVisual();

    puts("todos os testes da IA dos inimigos passaram");
    return 0;
}

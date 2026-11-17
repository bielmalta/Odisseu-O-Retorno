# Regressão e validação dos requisitos acadêmicos

Commit 27 do `PLANO_DE_COMMITS.md`. Este documento reúne:

1. a forma de executar a regressão;
2. o checklist de todos os requisitos, automático e manual;
3. o roteiro manual de uma partida completa;
4. os bugs conhecidos e as limitações aceitas.

Data da última execução: 17/11/2026, com as mecânicas exclusivas das quatro fases.

## 1. Como executar

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

Resultado esperado: `100% tests passed, 0 tests failed out of 15`.

Nenhum teste abre janela. Para ver só a regressão completa:

```bash
./build/bin/teste_regressao        # Windows: build\bin\teste_regressao.exe
```

Saída esperada:

```text
ok R1: lista circular com 9 operacoes reais
ok R2: QuickSort ordena por iniciativa e muda a ordem real
ok R3: realizarAtaque, verificarFimCombate e funcoes de objetivo
ok R4: 26 arquivos .c, regras sem Raylib e sem qsort()
ok R6: quatro fases com objetivos e encontros distintos
ok R5: partida completa do menu ao retorno a Itaca (4 fases, vida final 17/170)
```

### Testes existentes

| Teste (CTest) | Arquivo | O que garante |
|---|---|---|
| `lista_turnos` | `tests/teste_lista_turnos.c` | operações e invariantes da lista circular |
| `quicksort` | `tests/teste_quicksort.c` | QuickSort, religação e preservação dos nós |
| `combate` | `tests/teste_combate.c` | estados e rodadas do controlador |
| `acoes_combate` | `tests/teste_acoes_combate.c` | ataque, defesa e fórmula de dano |
| `ia_inimigo` | `tests/teste_ia_inimigo.c` | decisões e intervalo da IA |
| `cenarios_combate` | `tests/teste_cenarios_combate.c` | vitória, derrota, remoções e rodadas seguidas |
| `painel_combate` | `tests/teste_painel_combate.c` | alvo e mensagens do HUD |
| `animacao_combate` | `tests/teste_animacao_combate.c` | animação sem alterar a lógica |
| `telas_jogo` | `tests/teste_telas_jogo.c` | menu e pausa |
| `objetivo` | `tests/teste_objetivo.c` | sistema de objetivos |
| `configuracao_fases` | `tests/teste_configuracao_fases.c` | tabela de fases |
| `balanceamento` | `tests/teste_balanceamento.c` | jornada vencível e curva de dificuldade |
| `jornada` | `tests/teste_jornada.c` | progressão entre fases |
| `narrativa` | `tests/teste_narrativa.c` | introdução, transições e final |
| `regressao` | `tests/teste_regressao.c` | requisitos acadêmicos e partida completa |

## 2. Checklist de requisitos

Legenda: **A** = verificado automaticamente pelo CTest; **M** = verificado no roteiro manual da seção 3.

### Requisitos acadêmicos

| # | Requisito | Onde está | Evidência | Tipo |
|---|---|---|---|---|
| 1 | Lista circular duplamente encadeada como fila de turnos | `include/lista_turnos.h`, `src/estruturas/lista_turnos.c` (`NoTurno` com `proximo`/`anterior`) | `teste_regressao` R1, `teste_lista_turnos` | A |
| 2 | Seis ou mais operações reais sobre a lista | `criarPersonagem`, `inserirPersonagem`, `removerPersonagem`, `buscarPersonagem`, `avancarTurno`, `retrocederTurno`, `percorrerParticipantes`, `limparListaCombate`, `ordenarPorVelocidade` (9) | R1 conta e executa as 9 | A |
| 3 | QuickSort por velocidade/iniciativa | `particionar()` e `quickSort()` em `src/estruturas/quicksort.c` | R2, `teste_quicksort` | A |
| 4 | A ordenação altera a sequência real das ações | `ordenarPorVelocidade()` chamada no início de cada rodada (`src/jogo/combate.c`) | R2 troca a iniciativa e o primeiro a agir muda | A |
| 5 | Sem ordenação pronta (`qsort`) | todo `src/` e `include/` | R4 lê o código-fonte sem comentários | A |
| 6 | Linguagem C com Raylib, sem C++ | `CMakeLists.txt` (`LANGUAGES C`, C11) | R4 recusa `.cpp`/`.hpp` | A |
| 7 | Regras em C puro, sem Raylib | 8 módulos de regra (`personagem`, `lista_turnos`, `quicksort`, `combate`, `acoes_combate`, `ia_inimigo`, `jornada`, `objetivo`) | R4 recusa `raylib.h`/`raymath.h` neles | A |
| 8 | Combate por turnos | `src/jogo/combate.c` | `teste_combate`, `teste_cenarios_combate`, R5 | A |
| 9 | `realizarAtaque()`, `verificarFimCombate()`, `avancarJornada()` | `acoes_combate.c`, `combate.c`, `jornada.c` | R3 e R5 | A |
| 10 | Objetivo de exploração em cada fase | `ConfiguracaoFase.objetivo`, `src/jogo/objetivo.c`, `interagir()` em `fase.c` | R5 e R6 | A |
| 11 | Funções de objetivo do plano | `criarObjetivo`, `iniciarObjetivo`, `registrarProgresso`, `verificarObjetivo`, `concluirObjetivo` | R3 | A |
| 12 | Progressão até o retorno a Ítaca | `avancarJornada()` e `JORNADA_RETORNO_AO_LAR` | R5 percorre as 4 fases | A |
| 13 | Interface gráfica 3D e HUD de batalha | `src/render/*` | roteiro, passos 3 a 7 | M |
| 14 | Câmera e movimento em primeira pessoa | `src/render/jogador.c` | roteiro, passos 3 e 5 | M |
| 15 | Combate bloqueia o movimento e mantém a primeira pessoa | `definirControleJogador()` em `main.c` | roteiro, passo 5 | M |

### Regras de jogo do plano

| Regra | Evidência | Tipo |
|---|---|---|
| Vencer o combate não pula a fase; é preciso chegar à saída | R5 confere o bloqueio em `avancarJornada()` | A |
| Atributos de Odisseu preservados entre fases | R5 e `teste_jornada` | A |
| Derrota reinicia o encontro com vida cheia e lista limpa | R5 na Fase 1 | A |
| Após o resultado, entradas de combate ficam bloqueadas | R5 tenta agir depois da vitória | A |
| Pausa durante o combate retoma sem perder o turno | R5 na Fase 2 | A |
| Nenhum golpe causa menos de 1 de dano e a vida nunca fica negativa | R3, `teste_acoes_combate` | A |
| Inimigo nunca escolhe alvo derrotado | `teste_ia_inimigo` | A |
| Jornada vencível por um jogador novo e final mais difícil | `teste_balanceamento` | A |

### Checklist da apresentação (seção 8 do plano)

| Item | Como demonstrar | Situação |
|---|---|---|
| Lista com `proximo` e `anterior` | `lista_turnos.h` e R1 | pronto |
| Explorar uma fase e cumprir o objetivo por interação | roteiro, passo 3 | pronto |
| Avançar e retroceder um turno em modo de depuração | apenas pelos testes (`teste_lista_turnos`, R1) | **limitação L1** |
| Remover monstro derrotado sem quebrar a lista | Fase 2 ou 3 do roteiro; `teste_cenarios_combate` | pronto |
| Alterar a velocidade e mostrar a nova ordem | ordem no HUD muda entre as fases; R2 | pronto |
| Ordem calculada = ordem real das ações | HUD de ordem (commit 18) e R2 | pronto |
| Combate bloqueia movimento e mantém a primeira pessoa | roteiro, passo 5 | pronto |
| Vencer, liberar a saída, avançar e chegar ao final | roteiro completo; R5 | pronto |
| Testes de casos vazios, unitários e com empates | `teste_lista_turnos`, `teste_quicksort` | pronto |

## 3. Roteiro manual de uma partida completa

Executar o jogo compilado (`build/bin/odisseu_o_retorno`) e marcar cada passo.

| Passo | Ação | Resultado esperado | ✓ |
|---|---|---|---|
| 1 | Abrir o jogo | Menu com o título "ODISSEU — O ÚLTIMO RETORNO" e os botões **[ENTER] Jogar** e **[S] Sair** | |
| 2 | ENTER | Introdução da Fase 1 com premissa, controles e primeiro objetivo | |
| 3 | ENTER e explorar com WASD + mouse | Visão em primeira pessoa; HUD mostra fase, objetivo e progresso (0/3); não atravessa paredes | |
| 4 | Mirar uma peça de madeira e apertar E | Aviso de interação só com o item no centro; progresso sobe; ao chegar a 3/3 começa o confronto | |
| 5 | Tentar andar durante o combate | Movimento bloqueado; câmera vai suavemente até Polifemo e continua em primeira pessoa | |
| 6 | Conferir o HUD de combate | Vida de Odisseu e dos inimigos, ordem dos turnos com o atual destacado, botões **[A] Atacar** e **[D] Defender** | |
| 7 | Atacar até vencer | Inimigo age sozinho após uma pausa curta; animações e sons de golpe; tela de vitória | |
| 8 | ESC no meio de um combate | Pausa com **Retomar**, **[R] Reiniciar combate** e **[M] Menu principal**; Retomar volta ao mesmo turno | |
| 9 | Perder um combate de propósito (só defender) | Tela de derrota; **[ENTER] Tentar de novo** refaz o encontro com vida cheia | |
| 10 | Após a vitória, ir até a saída | Portão aberto; ao entrar na saída aparece a transição para a Fase 2 | |
| 11 | Fase 2 — Sereias | Objetivo próprio; com 3 participantes, Ligeia age antes de Odisseu; uma sereia derrotada some da ordem | |
| 12 | Fase 3 — Cila | Três tentáculos removidos da lista um a um | |
| 13 | Fase 4 — Ítaca | Os pretendentes agem antes de Odisseu; confronto mais difícil | |
| 14 | Chegar a Penélope | Cena final do retorno ao lar; ENTER ou ESC volta ao menu | |
| 15 | Menu → Sair | Janela fecha sem erro no console | |

### Execução automatizada da abertura (smoke test)

Em 16/11/2026, o jogo foi executado numa tela virtual Linux (Xvfb, 1280×720). Menu, introdução da Fase 1, exploração e movimento em primeira pessoa funcionaram a 60 FPS, sem erros no log da Raylib. O único aviso foi a falta de dispositivo de áudio no ambiente, e o jogo seguiu em silêncio (ver L4).

## 4. Bugs conhecidos e limitações aceitas

Nenhum bug que impeça concluir a jornada foi encontrado nesta regressão. O bug grave encontrado durante a validação foi corrigido no commit 26.

| ID | Tipo | Descrição | Situação |
|---|---|---|---|
| B1 | Bug (corrigido) | Fase 1 impossível: Polifemo (120 de vida, 25 de ataque) derrotava Odisseu (100 de vida) em todas as estratégias | corrigido no commit 26; `teste_balanceamento` impede regressão |
| L1 | Limitação | Não há modo de depuração dentro do jogo para avançar/retroceder turno manualmente (item da seção 8 do plano); `retrocederTurno()` é demonstrado pelos testes | aceita; demonstrar pelos testes ou criar uma tecla de depuração |
| B2 | Melhoria aplicada | A defesa comum vale +5 contra o próximo golpe; contra o golpe pesado anunciado de Polifemo, ela recebe +20 adicionais e reduz o dano de 26 para 1 | coberto por `teste_ia_inimigo` |
| L3 | Limitação visual | `assets/models` está vazio: inimigos, mãos e cenário usam primitivas da Raylib | aceita pelo escopo do protótipo |
| L4 | Ambiente | Sem placa de som, a Raylib avisa e o jogo segue em silêncio | comportamento esperado |
| L5 | Cobertura | Câmera, HUD, colisão com paredes e desenho 3D não têm teste automático (dependem de janela); são cobertos pelo roteiro manual | aceita |
| L6 | Projeto | A vida de Odisseu é carregada entre as fases e só é restaurada após uma derrota. Um jogador que chega ao final com pouca vida pode precisar repetir o último confronto | aceita; balanceada para a jornada ser vencível sem derrota |

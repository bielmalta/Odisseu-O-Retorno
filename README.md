
# Odisseu: O Último Retorno

Jogo 3D em primeira pessoa inspirado em A Odisseia, desenvolvido em linguagem C com a biblioteca Raylib para a disciplina de Algoritmos e Estruturas de Dados.

## Objetivo do jogo

O jogador assume o papel de Odisseu, que precisa enfrentar criaturas mitológicas e superar diferentes desafios para conseguir retornar à sua terra natal, Ítaca.

Cada fase apresenta um cenário inspirado na mitologia grega, com objetivos de exploração, interações e combates por turnos. Para avançar, o jogador deverá cumprir o objetivo da fase, derrotar os inimigos e encontrar a saída.

## Funcionamento do jogo

O jogo será dividido em dois modos principais:

- Exploração: o jogador poderá movimentar Odisseu em primeira pessoa, observar o cenário e interagir com objetos para cumprir os objetivos.
- Combate: ao encontrar criaturas mitológicas, o jogador participará de batalhas por turnos, podendo atacar ou defender.

A ordem dos participantes no combate será organizada por um algoritmo QuickSort, considerando seus atributos de velocidade e iniciativa.

Uma lista circular duplamente encadeada será responsável por controlar a sequência dos turnos.

### Fluxo principal

```text
Início do jogo
      |
      v
Exploração do cenário
      |
      v
Cumprimento do objetivo
      |
      v
Combate por turnos
      |
      v
Vitória no combate
      |
      v
Saída liberada
      |
      v
Próxima fase
      |
      v
Retorno a Ítaca
```

Em caso de derrota, o jogador poderá reiniciar o confronto.

## Controles previstos

| Controle | Ação |
|---|---|
| W, A, S, D | Movimentar Odisseu |
| Mouse | Controlar a câmera |
| E | Interagir com objetos |
| Botão esquerdo do mouse | Selecionar opções da interface |
| ESC | Pausar ou retomar o jogo |

No menu principal, `Enter` inicia uma nova partida e `S` ou `Esc` fecha o
jogo. Durante a pausa, `Enter` ou `Esc` retoma, `M` volta ao menu
principal e, se um combate estiver em andamento, `R` reinicia o confronto.

Durante os combates, o jogador poderá selecionar as ações de atacar e defender pela interface.

| Controle no combate | Ação |
|---|---|
| A ou clique em **Atacar** | Atacar o alvo selecionado (só no turno de Odisseu) |
| D ou clique em **Defender** | Reduzir o próximo golpe recebido (só no turno de Odisseu) |
| Tab, setas ou clique no inimigo | Trocar o alvo selecionado |
| Enter ou clique no botão | Continuar depois da vitória ou tentar de novo depois da derrota |
| M ou clique em **Menu principal** | Voltar ao menu depois da vitória ou derrota |

Depois do encerramento do combate, atacar, defender e trocar o alvo ficam
bloqueados. A tela de resultado aceita apenas continuar, tentar novamente,
voltar ao menu ou abrir a pausa.

## Tecnologias

- Linguagem C11
- Raylib 6.2
- CMake 3.20 ou superior

## Pré-requisitos no Windows

Instale:

1. Git;
2. CMake 3.20 ou superior;
3. Um compilador C, como Visual Studio Build Tools com o componente de C++ ou MinGW-w64.

A Raylib não precisa ser instalada manualmente. Por padrão, o CMake procura uma instalação local compatível e, caso não encontre, baixa a tag oficial `6.2` do repositório da Raylib durante a primeira configuração.

Nenhum caminho pessoal para SDK, compilador ou Raylib deve ser versionado no projeto.

## Compilação

Na raiz do repositório, execute:

```bash
cmake -S . -B build
cmake --build build --config Release
```

O executável será gerado dentro de `build/bin`. Em geradores com múltiplas configurações, como Visual Studio, ele poderá estar em `build/bin/Release`.

Para exigir uma Raylib já instalada e impedir o download automático:

```bash
cmake -S . -B build -DODISSEU_FETCH_RAYLIB=OFF
```

## Execução

Após a compilação, execute o arquivo `odisseu_o_retorno.exe` gerado na pasta `build/bin` ou `build/bin/Release`, dependendo do gerador utilizado.

Ao abrir, o jogo apresenta o menu principal. Uma nova partida inicia a fase da
Caverna de Polifemo, com exploração em primeira pessoa, coleta do objetivo e
combate por turnos. O menu de pausa funciona tanto na exploração quanto no
combate, preservando o estado até o jogador retomar.

## Arquitetura inicial

```text
src/
├── main.c
├── core/
├── estruturas/
├── jogo/
└── render/
include/
assets/
├── audio/
├── fonts/
├── models/
└── textures/
tests/
```

### Organização dos módulos

- `src/main.c`: ponto de entrada e loop principal.
- `src/core`: estados gerais, atualização do jogo e troca de telas.
- `src/estruturas`: lista circular duplamente encadeada e QuickSort.
- `src/jogo`: personagens, regras de combate, objetivos, fases e jornada.
- `src/render`: câmera, controles, HUD, áudio e renderização com Raylib.
- `include`: arquivos de cabeçalho com as interfaces públicas dos módulos.
- `assets`: modelos, texturas, fontes e arquivos de áudio.
- `tests`: testes das estruturas de dados e regras do jogo.

### Configuração de fases e encontros

O conteúdo da fase atual fica em uma tabela constante de C no módulo
`configuracao_fases`. Cada entrada reúne título, instrução, geometria do
cenário, itens, objetivo, saída, atributos do herói e inimigos, posições do
encontro, aparência provisória e textos de resultado.

O módulo `objetivo` controla o progresso por meio de `criarObjetivo()`,
`iniciarObjetivo()`, `registrarProgresso()`, `verificarObjetivo()` e
`concluirObjetivo()`. As regras da fase consomem essas configurações sem
conhecer nomes ou atributos específicos de Polifemo. Assim, novos conteúdos
podem ser descritos por outra entrada da tabela sem alterar o controlador de
combate.

### Fases mitológicas

1. **A Caverna de Polifemo:** coletar três peças de madeira, preparar a estaca,
   derrotar Polifemo e encontrar a saída da caverna.
2. **O Canto das Sereias:** proteger três pontos do convés, enfrentar duas
   Sereias e alcançar o leme.
3. **O Estreito de Cila:** reativar três mecanismos do navio, derrotar os três
   tentáculos de Cila e alcançar a proa.
4. **O Retorno a Ítaca:** recuperar o arco de Odisseu, derrotar os pretendentes
   e atravessar o palácio até Penélope.

Cada fase possui cenário, cores, objetivo, interação, quantidade de inimigos,
atributos e representação visual próprios. A mesma lista circular, o mesmo
QuickSort e o mesmo controlador de combate são reutilizados em todas elas.

### Sequências narrativas

Antes da primeira fase, uma introdução apresenta a tentativa de Odisseu de
retornar a Ítaca, os controles e o objetivo inicial. Ao avançar na jornada,
uma transição curta contextualiza o próximo destino e mostra o novo objetivo.
Depois da última fase, o epílogo apresenta o reencontro com Penélope e encerra
a viagem antes de retornar ao menu principal.

### Animações, câmera e sons do combate

O dano é sempre resolvido pela lógica (`acoes_combate`), no instante da ação. O módulo `animacao_combate`, em C puro, só conta o tempo do que a tela mostra: cada ataque ou defesa passa por começo, impacto e retorno, e o combate espera a animação terminar antes de passar o turno. A barra de vida cai no impacto, a mensagem aparece junto e a tela de vitória ou derrota só surge depois do golpe final e da queda.

Na tela, a câmera vai suavemente do modo livre até o foco do combate, e Odisseu aparece em primeira pessoa com lança e escudo. O inimigo investe, recua, se abaixa na defesa e tomba ao ser derrotado, e há tremor de câmera e vinheta ao receber um golpe.

Os efeitos sonoros de `assets/audio` (golpe, impacto, bloqueio, defesa e queda) foram gerados por síntese, sem amostras de terceiros. Para recriá-los:

```bash
gcc -std=c11 tools/gerar_sons.c -o gerar_sons -lm
./gerar_sons assets/audio
```

Sem placa de som ou sem esses arquivos, o jogo segue normalmente, em silêncio.

### Convenções de programação

O projeto seguirá as seguintes convenções:

- Todo o código do jogo será escrito em C11.
- Os arquivos `.c` conterão as implementações das funções.
- Os arquivos `.h` declararão as estruturas e funções compartilhadas entre os módulos.
- Os módulos de lógica deverão ser separados da renderização gráfica.
- As funções internas de cada módulo poderão ser declaradas como `static`.
- A memória alocada dinamicamente deverá ser liberada corretamente.
- Os recursos carregados pela Raylib deverão ser descarregados ao encerrar o jogo.

## Estruturas de dados e algoritmos

### Lista circular duplamente encadeada

Será utilizada para organizar os participantes dos combates.

Cada nó terá referências para o participante anterior e o próximo, permitindo avançar e retroceder entre os turnos.

### QuickSort

Será utilizado para ordenar os participantes por iniciativa, considerando a velocidade e possíveis modificadores.

A ordenação determinará a sequência real das ações durante cada rodada.

## Testes

Os testes das estruturas de dados e das regras do jogo serão escritos em C e executados separadamente, sem a necessidade de abrir uma janela gráfica.

Serão utilizados testes com `assert` para validar operações da lista, ordenação dos participantes e funcionamento dos combates.

Os executáveis e comandos específicos dos testes serão adicionados quando os módulos de teste forem implementados.

## Escopo do primeiro protótipo

O primeiro protótipo jogável deverá conter:

- Uma fase 3D explorável em primeira pessoa.
- Movimentação e controle de câmera.
- Interação com objetos e objetivo de exploração.
- Combate por turnos entre Odisseu e uma criatura.
- Ações de ataque e defesa.
- Interface com informações de vida e ordem dos turnos.
- Condições de vitória, derrota e reinício.

As demais fases e criaturas serão adicionadas posteriormente.

## Equipe

Projeto desenvolvido pela Turma A para a disciplina de Algoritmos e Estruturas de Dados da CESAR School.

- Gabriel Gondim Malta
- Arthur de Almeida Oliveira
- Márcio Rodriguez Perez
- Guilherme Silva Guimarães
- Hélio de Moraes Rêgo Neto

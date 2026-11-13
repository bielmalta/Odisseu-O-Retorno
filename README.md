# Odisseu: O Último Retorno

Jogo 3D em primeira pessoa desenvolvido em C com Raylib para a disciplina de
Algoritmos e Estruturas de Dados.

## Tecnologias

- C11
- Raylib 6.2
- CMake 3.20 ou superior

## Pré-requisitos no Windows

Instale:

1. Git;
2. CMake 3.20 ou superior;
3. um compilador C, como Visual Studio Build Tools com o componente de C++ ou
   MinGW-w64.

A Raylib não precisa ser instalada manualmente. Por padrão, o CMake procura uma
instalação local compatível e, caso não encontre, baixa a tag oficial `6.2` do
repositório da Raylib durante a primeira configuração.

Nenhum caminho pessoal para SDK, compilador ou Raylib deve ser versionado no
projeto.

## Compilação

Na raiz do repositório, execute:

```powershell
cmake -S . -B build
cmake --build build --config Release
```

O executável será gerado dentro de `build/bin`. Em geradores com múltiplas
configurações, como Visual Studio, ele poderá estar em `build/bin/Release`.

Para exigir uma Raylib já instalada e impedir o download automático:

```powershell
cmake -S . -B build -DODISSEU_FETCH_RAYLIB=OFF
```

## Estrutura inicial

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

- `src/core`: loop, estados gerais e troca de telas.
- `src/estruturas`: lista circular e QuickSort em C puro.
- `src/jogo`: personagem, combate, objetivos, fases e jornada.
- `src/render`: câmera, entrada, HUD, áudio e desenho com Raylib.
- `include`: interfaces públicas dos módulos.
- `assets`: recursos visuais e sonoros.
- `tests`: testes da estrutura de dados e das regras, sem janela gráfica.

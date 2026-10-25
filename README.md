# G8_D-C_SpaceInvadersRE_PA-26.2

# || 👾 Space Invaders: Dividir e Conquistar 👾 ||

Este projeto irá ser uma recriação moderna e caótica do clássico *Space Invaders*, desenvolvido em **C++** com a biblioteca **Raylib**. O objetivo principal é demonstrar a aplicação prática do paradigma de **Dividir e Conquistar** para otimizar o desempenho ao processar frotas massivas de inimigos.

## > --- | Mecânicas e Algoritmos | --- <

O jogo utiliza dois algoritmos clássicos de Divisão e Conquista para resolver gargalos computacionais geométricos e estatísticos em tempo real:

### 1. Quebra de Escudo (Mediana das Medianas)
- **O Problema:** A frota inimiga está protegida por um escudo impenetrável. Para desativá-lo, o jogador deve focar o tiro na "Nave Comandante", que é dinamicamente o centro de massa da frota (a mediana das coordenadas X).
- **A Solução:** Em vez de ordenar o vetor de naves a cada *frame* para encontrar o centro, utilizamos o algoritmo da **Mediana das Medianas** para encontrar o alvo exato com garantia de tempo linear **O(n)** no pior caso, evitando quedas na taxa de fotogramas.

### 2. Reação em Cadeia Letal (Par de Pontos mais Próximos)
- **O Problema:** Após a queda do escudo, o jogador pode disparar um raio elétrico. Este raio deve atingir a área mais densa do enxame inimigo para maximizar o dano em cadeia.
- **A Solução:** Calcular a distância entre todas as naves através de força bruta custaria tempo quadrático. Utilizamos o algoritmo do **Par de Pontos mais Próximos** para varrer o enxame e encontrar as duas naves mais próximas entre si em **O(n log n)**.

## > --- | Tecnologias a Serem Utilizadas | --- <
- **Linguagem:** C++
- **Gráficos e Janela:** Raylib
- **Build System:** CMake

## > --- | Estrutura de Diretórios | --- <

| **Diretório / Ficheiro** | **Descrição e Responsabilidade** |
| :--- | :--- |
| `assets/` | Ficheiros de recursos estáticos utilizados no jogo, como texturas (sprites), efeitos sonoros (SFX) e fontes tipográficas personalizadas. |
| `include/` | Diretório raiz para os ficheiros de cabeçalho (`.hpp`). Define as interfaces e contratos das classes. |
| ├── `algorithms/` | Declarações dos módulos de **Dividir e Conquistar**, separando a lógica matemática pura (Mediana das Medianas, Par de Pontos) da lógica visual. |
| ├── `core/` | Estruturas base do motor de jogo, abrangendo a gestão da janela do Raylib, controlo de estados (*Game Loop*) e gestão de *inputs*. |
| └── `entities/` | Definição estrutural dos objetos do jogo: Nave do Jogador, Invasores individuais, Frota Inimiga completa e Projéteis. |
| `src/` | Diretório raiz para a implementação do código-fonte (`.cpp`). Onde a mecânica é efetivamente programada. |
| ├── `algorithms/` | Implementação detalhada e otimizada dos algoritmos ( funções com complexidade $O(n)$ e $O(n \log n)$ ). |
| ├── `core/` | Implementação do funcionamento global, inicialização gráfica e limpeza de memória. |
| ├── `entities/` | Lógica de atualização posição/velocidade, processamento de colisões e desenho de cada entidade a cada *frame*. |
| `src/main.cpp` | O ponto de entrada principal do programa (`int main()`), que instancia os sistemas e dá início à simulação. |
| `CMakeLists.txt` | Script de orquestração do compilador. Define a versão do C++, trata do descarregamento automático do Raylib (*FetchContent*) e compila os binários. |
| `.gitignore` | Protege o repositório impedindo que o diretório `build/` e outros ficheiros gerados localmente sejam comitados acidentalmente. |

O Nix não possui suporte nativo direto para Windows, mas funciona perfeitamente lá através do **WSL2** (Windows Subsystem for Linux). Utilizadores de Windows com WSL2 podem rodar o comando `nix-shell` e abrir a janela gráfica do Raylib perfeitamente graças ao suporte GUI integrado do Windows 11 (WSLg).

No entanto, a grande vantagem da arquitetura que montámos com o `CMake` + `FetchContent` é que o seu projeto **já é compatível com Windows nativo**! Um utilizador de Windows não precisa do Nix; basta ter o Visual Studio (MSVC) ou o MinGW instalados, e o CMake tratará de baixar o Raylib sozinho.

Para documentar todas estas facilidades e deixar o projeto extremamente acessível, substitua a secção **"Como vai funcionar a Compilação e Execução"** do seu `README.md` por esta versão atualizada:

## > --- | Como vai funcionar a Compilação e Execução | --- <

### Pré-requisitos
- Compilador C++20 (GCC, Clang ou MSVC)
- CMake (v3.16+)
- Raylib (obtida dinamicamente via FetchContent no CMake)

### 🐧 Compilação (Linux Nativo)
Abra o terminal na raiz do projeto e execute:
```bash
cmake -B build
cmake --build build
./build/space_invaders_dc

```

### ❄️ Compilação (NixOS / Nix via WSL2 no Windows)

Para utilizadores de NixOS ou Windows rodando WSL2, o projeto inclui um ambiente de desenvolvimento reprodutível (`shell.nix`) com todas as dependências gráficas do X11/Wayland configuradas.

```bash
nix-shell
cmake -B build
cmake --build build
./build/space_invaders_dc
```

### 🪟 Compilação (Windows Nativo)

Graças à orquestração do CMake, o projeto compila nativamente no Windows. Pode abrir a pasta no **Visual Studio 2022** (que deteta o CMake automaticamente) ou usar o terminal (via Developer Command Prompt):

```cmd
cmake -B build
cmake --build build
.\build\Debug\space_invaders_dc.exe
```

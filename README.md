# Planejador — caminho mais curto com A*

Projeto da disciplina **Programação Avançada** do Departamento de Engenharia de Computação e Automação (DCA) da **UFRN**, com o professor Adelardo Adelino Dantas de Medeiros.

A disciplina tem cinco projetos práticos, feitos nesta ordem:

| # | Projeto | Assunto principal |
| :-: | :-- | :-- |
| 1 | [BigInt](https://github.com/Marcs-paulo/BigInt) | classes, ponteiros e memória dinâmica |
| 2 | **Planejador** (este) | exceções, arquivos e STL (algoritmo A*) |
| 3 | [Plotador](https://github.com/Marcs-paulo/Plotador) | interface gráfica com Qt |
| 4 | Designer | herança, polimorfismo e métodos virtuais (desenhos 2D exportados em SVG) |
| 5 | Supervisório de tanques | aplicação cliente-servidor com sockets TCP/IP e cliente em Qt |

## O que é

Um planejador de rotas que lê um mapa de pontos e estradas do Rio Grande do Norte e encontra o **caminho mais curto** entre dois pontos usando o algoritmo **A\***, com a distância em linha reta (fórmula de haversine) como estimativa do custo restante.

```
ID do ponto de origem: #1
ID do ponto de destino: #2
Tempo: 0.43ms   Nohs em aberto: 8 fechado: 22
TOTAL: 268.6km
De Natal - Viaduto de Ponta Negra
Por BR101 ateh Parnamirim (7.8km)
Por BR226 ateh Macaiba (10.1km)
...
Por Av. Pres. Dutra ateh Mossoro - Centro (4.2km)
```

## Foco do projeto e o que buscamos aprender

- **Tratamento de exceções** (`try`, `catch`, `throw`): todo erro de leitura ou de busca é sinalizado por exceção, e um arquivo com erro não pode alterar o mapa já carregado.
- **Leitura de arquivos com streams**: ler pontos e rotas campo a campo com `operator>>`, `getline` e `ws`, aceitando espaços, quebras de linha e vários dados por linha, sem ler a linha inteira numa string para depois recortar.
- **Contêineres da STL**: escolher entre `vector`, `deque` e `list` para os conjuntos Aberto e Fechado do A\* conforme as operações de cada um (inserir no meio, retirar do início, percorrer).
- **Algoritmos da STL** (`find`, `upper_bound` etc.) no lugar de laços `for` com `if` escritos à mão, usando sempre o algoritmo mais direto e sobrecarregando os operadores de comparação nas classes.
- **O algoritmo A\***: custo passado (g), custo futuro estimado (h), inserção ordenada sem reordenar tudo, e o caso em que um nó já existente é substituído por um caminho mais barato.

## O que foi pedido

O enunciado completo está em [`docs/planejador-especificacao.pdf`](docs/planejador-especificacao.pdf). Em resumo:

- Implementar a leitura dos arquivos `pontos.txt` e `rotas.txt` (campos separados por `;`, com cabeçalho fixo) na função `Planejador::ler`.
- Criar a classe `Noh` só com os dados do A\*: id do ponto, id da rota, custo passado e custo futuro.
- Implementar `calculaCaminho` com o A\*, guardando quantos nós ficaram em Aberto e em Fechado.
- Não usar `priority_queue`, `set`, `map` nem outros contêineres de árvore ou heap.
- Não modificar as classes e funções que já vieram prontas (`IDPonto`, `IDRota`, `main`).

## Como compilar e rodar

Precisa só de um compilador C++17 (g++ ou clang). Rode a partir da pasta do projeto, porque o programa abre `pontos.txt` e `rotas.txt` dela.

```bash
g++ -std=c++17 -Wall -o planejador planejador-main.cpp planejador.cpp
./planejador
```

Para conferir a leitura de arquivos com os 48 casos de teste do professor (arquivos certos e com erro em `arq_teste/`):

```bash
g++ -std=c++17 -o teste teste.cpp planejador.cpp
./teste
```

Também dá para abrir `planejador.cbp` no **Code::Blocks**, que foi a IDE usada na disciplina.

## Arquivos

| Arquivo | Origem |
| :-- | :-- |
| `planejador.h`, `planejador.cpp` | leitura dos arquivos e A\*, implementados por mim a partir da base do professor |
| `planejador-main.cpp`, `teste.cpp` | programas fornecidos pelo professor (sem alterações) |
| `pontos.txt`, `rotas.txt` | mapa usado pelo programa principal |
| `arq_teste/` | arquivos certos e com erro usados pelo `teste.cpp` |
| `planejador.cbp` | projeto do Code::Blocks |
| `docs/planejador-especificacao.pdf` | enunciado do projeto |

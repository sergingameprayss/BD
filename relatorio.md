# Relatório


# main

The main.c is the responsible for the interface of the program



## Decisões de Projeto
### Representação dos atributos e das dependências funcionais
Para cada dependência funcional, representada pela struct fd, marcamos quais letras estão do lado esquerdo e quais estão do lado direito, por isso MAX_ATTRS possui tamanho 26, uma para cada letra do alfabeto.
Para imprimir na tela, já sai ordenado.

### Leitura dos arquivos
Dado um arquivo .fds e um ponteiro para o conjunto U, F. Se U, então ler todo o universo de atributos, ou seja, enquanto não for final do arquivo ou } ler todas as letras válidas. Se F,ler lado esquerdo e direito, alterando a cada -> ou ,.

## Descrições dos algoritmos
### Closure
Inicialmente, colocar os próprios atributos do fecho, no fecho, por exemplo, se queremos fecho de AB, colocamos {A,B}. Depois, para cada dependência funcional, se a letra está do lado esquerdo da dependência, mas não está nas letras do fecho, então não tem o que adicionar no fecho, caso contrário, eu olho o lado direito, se tem alguém que está na dependência, mas que não está no fecho, ele é adicionado, e reiniciamos o loop, a partir do momento que não tem mais o que ser adicionado, encerramos o loop.
Então, inicialmente colocamos o atributo no fecho, depois para cada dependencia funcional (primeiro for) vamos analisar se o atributo j está na dp e se não está no fecho (ex: se fecho temos A, mas dependencia é BD->E, então não tem o que adicionar)
Se passar por esse if, entramos no segundo for, pois, se temos o atributo no lado esquerdo, podemos colocar os atributos do lado direito.
Quando que acaba?
Depois de passar por todas as dependências e não ter mais o que colocar, reiniciando o primeiro for, no segundo if, o atributo vai estar lá, então new_attr vai permanecer false.
### Mincover

### ...


## Análise de complexidade

## Estudos de caso

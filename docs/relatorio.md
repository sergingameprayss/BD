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
O algoritmo da cobertura mínima é dividida em três passos: decompor o lado direto para que fique um atributo único; remover os atributos redundantes do lado esquerdo e, por fim, remover as dependências funcionais redundantes. 

O código recebe como parâmetro as dependências funcionais originais e um ponteiro para a cobertura mínima, que inicialmente está vazia e será construída ao longo de cada passo. A descrição de cada passo está separado em tópicos abaixo:

Passo 1: Para cada dependência funcional, vemos quais letras estão no lado direito. Se a letra estiver do lado direito `if(sys->fds[i].rhs[j])` então criamos uma nova dependência funcional (primeiro for dentro do if) que conterá o lado esquerdo na íntegra (segundo for dentro do if) e o lado direito unitário ` fd_mincover->fds[idx].rhs[j] = true`.

Passo 2: Para cada dependência funcional, resultante do passo 1, contamos quantas letras temos do lado esquerdo (segundo for) e, se for mais que uma (`if(count_lhs_attr > 1)`), verificamos se há redundância. Para cada letra presente no lado esquerdo (`if(fd_mincover->fds[i].lhs[k])`) calculamos o fecho ignorando esse atributo, se o lado direito estiver no fecho (`if(fd_mincover->fds[i].rhs[l] && attr_clos[l])`) então o atributo do lado esquerdo realmente é redundante e pode ser retirado (`fd_mincover->fds[i].lhs[k] =false`)

Passo 3: Para cada dependência funcional, resultante do passo 2, criamos uma dependência funcional temporária (`fd_t temp_fd = fd_mincover->fds[i]`), salvamos o lado esquerdo da dependência a ser retirada (`lhs[idx++] = 'A' +j`), ignoramos a dependencia funcional (`fd_mincover->fds[i].lhs[j] = false; fd_mincover->fds[i].rhs[j] = false`) e calculamos o fecho, se o lado direito da dependência retirada ainda estiver no fecho significa que a dependência funcional realmente pode ser retirada, então devemos retirá-la e puxar todas as dependências "para trás" (bloco do else), senão apenas devolvemos a dependência (`fd_mincover->fds[i] = temp_fd`)



## Análise de complexidade

## Estudos de caso


## Exemplos
### Closure
`closure_01`: Testa se o código é capaz de fazer mais de uma passada. Saída esperada: F = {A,B,C,D,E}, para fecho de A
`closure_02`: Testa se o código exige que todas as letras do lado esquerdo estejam presentes para validar o lado direito. Saída esperada: F = {A, B, C, D, E}, para fecho AC
`closure_03`: Testa fecho que não é superchave. Saída esperada: F = {A, B}, para fecho de A
`closure_04`: Testa se o código identifica que não tem como incluir atributos, apenas o dado. Saída esperada: F = {A}, para fecho A


### Mincover
Lembrando dos passos para obter a cobertura mínima:
1) Decompor o lado direito;
2) Verificar atributos redundantes do lado esquerdo;
3) Remover dependências funcionais redundantes.

` mincover_01`: Dos exercícios de cobertura mínima, exige os passos 1 e 3. Saída esperada: F = {A->B, B->C}
` mincover_02`: Dos exercícios de cobertura mínima, exige o passo 2. Saída esperada: F = {A->C, C->D, D->E}
` mincover_03`: Dos exercícios de cobertura mínima, exige o passo 1 e 2. Saída esperada: F = {A->C, C->D, D->E}
` mincover_04`: Dos exercícios de cobertura mínima, exige os passos 2 e 3. Saída esperada:F = {A-> B, B-> C, C-> D, A-> E}
` mincover_05`: Exige apenas o passo 1. Saída esperada: F = { A -> B, A -> C, C -> D }
` mincover_06`: Exige apenas o passo 2. Saída esperada: F = { A -> B, B -> C, A -> D }
` mincover_07`: Exige apenas o passo 3. Saída esperada: F = { A -> B, B -> C, C -> D}
` mincover_08`: Exige os passos 1 e 3. Saída esperada: F = { A -> B, B -> C, C -> D }
` mincover_09`: Exige os três passos. Saída esperada: F = { A -> B, B -> C, A -> D, A -> E }
` mincover_10`: Testa se o código identifica, no passo 2, que um lhs não pode ser separado. Saída esperada: F = { AB -> C, A -> D, B -> E }
` mincover_11`: Testa se o código identifica repetições ou dependências triviais. Saída esperada: F = {A -> B}
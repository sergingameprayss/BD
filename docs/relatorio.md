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
O algoritmo para cálculo do fecho recebe como parâmetros universo de atributos U e o conjunto de dependências funcionais  F, representados por `sys`, o atributo que se deseja calcular o fecho (`attr`) e o ponteiro para o conjunto de atributos que representará o fecho (`attr_clos`). Inicialmente, verificamos se `sys` e `attr_clos` existem, se existirem, para cada atributo em `attr` o incluimos em attr_clos, ou seja, colocamos os atributos no fecho. Após isso, colocamos o controlador `new_attr` como falso e, para cada dependência funcional, verificamos se a letra no lado esquerdo está não está no fecho (`if (sys->fds[i].lhs[j] && !attr_clos[j])`), por exemplo, se no fecho temos {A} e a dependência é BD->E, então `lhs_valid = false` e pulamos essa iteração. Se `lhs_valid = true` então para cada letra no lado direito que não está no fecho (`if (sys->fds[i].rhs[j] && !attr_clos[j])`) nós a adicionamos no fecho (`attr_clos[j] = true`) e mantemos o loop funcionando para próximas verificações (`new_attr = true`).

O loop encerra quando, na última passagem por todas as dependências funcionais (`for (int i = 0; i < sys->size; i++)`) não há nenhum atributo do lado direito que não esteja no fecho (não entra em `if (sys->fds[i].rhs[j] && !attr_clos[j])`), então como atribuimos (`new_attr = false`) e nunca mudamos para (`new_attr = true`), então o loop termina.


### Mincover
O algoritmo da cobertura mínima é dividida em três passos: decompor o lado direto para que fique um atributo único; remover os atributos redundantes do lado esquerdo e, por fim, remover as dependências funcionais redundantes. 

O código recebe como parâmetro as dependências funcionais originais e um ponteiro para a cobertura mínima, que inicialmente está vazia e será construída ao longo de cada passo. A descrição de cada passo está separado em tópicos abaixo:

Passo 1: Para cada dependência funcional, vemos quais letras estão no lado direito. Se a letra estiver do lado direito `if(sys->fds[i].rhs[j])` então criamos uma nova dependência funcional (primeiro for dentro do if) que conterá o lado esquerdo na íntegra (segundo for dentro do if) e o lado direito unitário ` fd_mincover->fds[idx].rhs[j] = true`.

Passo 2: Para cada dependência funcional, resultante do passo 1, contamos quantas letras temos do lado esquerdo (segundo for) e, se for mais que uma (`if(count_lhs_attr > 1)`), verificamos se há redundância. Para cada letra presente no lado esquerdo (`if(fd_mincover->fds[i].lhs[k])`) calculamos o fecho ignorando esse atributo, se o lado direito estiver no fecho (`if(fd_mincover->fds[i].rhs[l] && attr_clos[l])`) então o atributo do lado esquerdo realmente é redundante e pode ser retirado (`fd_mincover->fds[i].lhs[k] =false`)

Passo 3: Para cada dependência funcional, resultante do passo 2, criamos uma dependência funcional temporária (`fd_t temp_fd = fd_mincover->fds[i]`), salvamos o lado esquerdo da dependência a ser retirada (`lhs[idx++] = 'A' +j`), ignoramos a dependencia funcional (`fd_mincover->fds[i].lhs[j] = false; fd_mincover->fds[i].rhs[j] = false`) e calculamos o fecho, se o lado direito da dependência retirada ainda estiver no fecho significa que a dependência funcional realmente pode ser retirada, então devemos retirá-la e puxar todas as dependências "para trás" (bloco do else), senão apenas devolvemos a dependência (`fd_mincover->fds[i] = temp_fd`)

### Keys

No código desenvolvido, a função `candidate_keys` recebe como parâmetros o conjunto de dependências funcionais (`sys`) e um ponteiro para as chaves candidatas encontradas (`(*keys)[MAX_ATTRS]`). Após o seu termino, ela retorna um inteiro não-negativo, indicando a quantidade de chaves candidatas encontradas.

O código pode ser separado, para melhor compreensão, em dois passos, sendo eles o cálculo dos elementos essenciais e o cálculo do fecho. Na primeira parte, o laço procura dentro do conjunto de atributos existentes das dependências funcionais todos os atributos essenciais, ou seja, os atributos que não são determinados por nenhum outro.

Terminado o primeiro passo, o segundo passo consiste em verificar se existe atributos essenciais e se estes formam uma superchave. Em caso de verdadeiro é feito a cópia dos atributos para o ponteiro `keys`, retornando a quantidade de chaves candidatas como **uma** apenas. Em caso de falso, é testado todas as combinações de elementos restantes utilizando *backtracking* (`generate_combinations`), sempre incluindo os atributos essenciais no início de cada iteração junto de uma quantidade `tam` de atributos extras, nesse caso, os atributos de `remaning_idx`. É necessário que seja em ordem crescente os testes, pois pela definição de chave candidata, ela é uma superchave minimal. Após a geração de todas as combinações restantes para todos os tamanhos de subconjuntos restantes é retornado o total de chaves candidatas.

Uma breve explicação de como funciona a função `generate_combinations`. Ela espera receber os seguintes parâmetros: conjunto de dependências funcionais (`sys`), vetor de indices a serem verificados (`remaning_idx`), quantidade de valores no vetor (`num_remaning`), o índice inicial (`start_idx`), a quantidade de atributos inseridos em `test_set`, o tamanho que estamos testando atualmente (`target_size`), o conjunto testado (`test_set`), as chaves candidatas já encontradas (`keys`) e o total de chaves candidatas (`num_keys`). O caso base é quando a quantidade de elementos extras inseridos em `test_set` for igual a quantidade desejada, nesse caso verificamos se as chaves do ramo atual é uma superchave minimal. Caso seja, a nova chave é inserida. Em ambos os casos, a descida na árvore encerra-se. O passo recursivo testa todos os elementos não essenciais restantes, descendo na árvore a cada nova chamada.

### Normal form

No código desenvolvido, `normal_form` recebe como parâmetro o conjunto de depedências funcionais e verifica se elas respeitam a 3NF e BCNF. Após o seu termino, ela retorna `OK` em caso de sucesso, indicando que ela executou por completo sem erros.

O código é divido em 3 etapas: verificação de atributos primos, verificação de violação das normas e impressão de mensagens.

O primeiro laço verifica os atributos primos, os atributos 


## Análise de complexidade 
### Mincover
Considerando N o número de dependências, A = MAX_ATTRS, N' a quantidade de dependências após um passo e C a complexidade de closure, vamos analisar cada passo.

Passo 1: 
```c
for(int i = 0; i < sys->size; i++){ // roda N vezes
    for(int j = 0; j < MAX_ATTRS; j++){ // roda A vezes
        if(sys->fds[i].rhs[j]){
            for(int k = 0; k < MAX_ATTRS; k++) // roda A vezes
            for(int k = 0; k < MAX_ATTRS; k++) // roda A vezes
            // ou seja, A+A = 2A
```
Então, a complexidade é N*A*2A = O(N*A²) = O(N), já que A é uma constante. Além disso, o tamanho do conjunto de dependências funcionais pode no máximo ser N' = N*A

Passo 2:
```c
for(int i = 0; i < fd_mincover->size; i++){ // roda N' vezes
    for(int k = 0; k < MAX_ATTRS; k++) // roda A vezes
    for(int k = 0; k < MAX_ATTRS; k++){ // roda A vezes
        if(fd_mincover->fds[i].lhs[k]){
            for(int m = 0; m < MAX_ATTRS; m++) // roda A vezes
            closure(fd_mincover, lhs_wo_k, attr_clos); // O(C)
            for(int l = 0; l < MAX_ATTRS; l++) // roda A vezes
            // ou seja 2A + O(C)
        }
    } // ou seja A*(2A + O(C))
}
```
Então a complexidade é O(N'*A²*C), e substituindo N' por N e sabendo que N é uma constante temos O(N*C)

Passo 3:
```c
for(int i = 0; i < fd_mincover->size; i++){ // roda N' vezes
    for(int j = 0; j < MAX_ATTRS; j++) // roda A vezes
    for (int j = 0; j < MAX_ATTRS; j++) // roda A vezes
    closure(fd_mincover, lhs, attr_clos); // O(C)
    for(int l = 0; l < MAX_ATTRS; l++) // roda A vezes
    
    // se for redundante, desloca todo o resto das dependencias
    for (int k = i; k < fd_mincover->size - 1; k++) { // O(N') no pior caso
        fd_mincover->fds[k] = fd_mincover->fds[k + 1];
    }
}
```
Então a complexidade é O(N'*(3A+C) + N'*N'), como A é constante e substituindo N' por N então O(N'*C + N²)

Então no total temos O(N) + O(N*C) + O(N*C + N²), em outras palavras, O(N²) no pior casos.

### Keys

### Normal Form

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

### Keys

### Normal Form
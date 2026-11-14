# Compilação
Compilar o código
```bash
make
```
Remover arquivos objeto
```bash
make clean
```
Remover arquivos objeto e binário
```bash
make purge
```
Gerar arquivo .tgz
```bash
make tgz
```

# Execução
Fecho
```bash
./fdtool closure --fds <exemplos/arquivo.fds> --X <ATRIBUTOS>
```
Cobertura Mínima
```bash
./fdtool mincover --fds <exemplos/arquivo.fds> 
```
Chave Candidata
```bash
./fdtool keys --fds <exemplos/arquivo.fds> 
```
Verificar BCFN e 3FN
```bash
./fdtool normalform --fds <exemplos/arquivo.fds> 
```

# Exemplos
Os exemplos, os quais estão no diretório `exemplos/`, testam exaustivamente cada ferramenta que manipula as Dependências Funcionais. 
## Fecho 
`closure_01.fds`: Testa se o código identifica que deve-se fazer mais de uma passada pelas dependências funcionais.
```bash
U = {A, B, C, D, E}
F = { A -> B, B -> C, C -> D, D -> E }
```
Para o fecho de A, a saída esperada é:
```bash
ABCDE
```

`closure_02.fds`: Confere se o cógio exige que todas as letras do lado esquerdo estejam presentes para validar o lado direito.
```bash
U = {A, B, C, D, E}
F = { A -> B, C -> D, BD -> E }
```
Para o fecho de AC, a saída esperada é:
```bash
ABCDE
```

`closure_03.fds`: Fecho de um atributo que não é sueperchave.
```bash
U = {A, B, C, D, E}
F = { A -> B, C -> D, BD -> E }
```
Para o fecho de A, a saída esperada é:
```bash
AB
```

`closure_04.fds`: Testa se o código identifica que não tem como incluir atributos. 
```bash
U = {A, B, C}
F = { B -> C }
```
Para o fecho de A, a saída esperada é:
```bash
A
```
## Cobertura Mínima
Lembrando dos passos para obter a cobertura mínima:
1) Decompor o lado direito;
2) Verificar atributos redundantes do lado esquerdo;
3) Remover dependências funcionais redundantes.

` mincover_01`: Dos exercícios de cobertura mínima, exige os passos 1 e 3. 
```bash 
U = {A,B,C}
F = {A->BC, B->C, A->B}
```
Saída esperada: 
```bash
A->B
B->C
```

` mincover_02`: Dos exercícios de cobertura mínima, exige o passo 2. 
```bash
U = {A,B,C,D,E}
F = {AB->C, A->C, C->D, D->E}
```
Saída esperada: 
```bash
A->C 
C->D
D->E
```

` mincover_03`: Dos exercícios de cobertura mínima, exige o passo 1 e 2.
```bash
U = {A,B,C,D,E}
F = {AB->CD, A->C, C->D, D->E}
```
 Saída esperada:
```bash
A->C
C->D 
D->E
```

` mincover_04`: Dos exercícios de cobertura mínima, exige os passos 2 e 3. 
```bash
U = {A,B,C,D,E}
F = {AB->C, A->B, B->C, C->D, AD->E}
```
Saída esperada:
```bash
A-> B
B-> C
C-> D 
A-> E
```

` mincover_05`: Exige apenas o passo 1. 
```bash 
U = {A, B, C, D}
F = { A -> BC, C -> D }
```
Saída esperada: 
```bash
A -> B
A -> C 
C -> D
```

` mincover_06`: Exige apenas o passo 2.
```bash 
U = {A, B, C, D}
F = { A -> B, B -> C, AC -> D }
```
Saída esperada:
```bash
A -> B 
B -> C 
A -> D
```

` mincover_07`: Exige apenas o passo 3. 
```bash 
U = {A, B, C, D}
F = { A -> B, B -> C, C -> D, A -> C, A -> D }
```
Saída esperada: 
```bash
A -> B
B -> C 
C -> D
```

` mincover_08`: Exige os passos 1 e 3.
```bash
U = {A, B, C, D, E}
F = { A -> BCD, B -> C, C -> D }
```

Saída esperada: 
```bash
A -> B 
B -> C 
C -> D
```

` mincover_09`: Exige os três passos. 
```bash 
U = {A, B, C, D, E}
F = { A -> BC, B -> C, AB -> D, ACD -> E }
```
Saída esperada: 
```bash
A -> B
B -> C
A -> D 
A -> E
```

` mincover_10`: Testa se o código identifica, no passo 2, que um lhs não pode ser separado. 
```bash
U = {A, B, C, D, E}
F = { AB -> C, A -> D, B -> E }
```
Saída esperada: 
```bash
AB -> C 
A -> D 
B -> E
```

` mincover_11`: Testa se o código identifica repetições ou dependências triviais. 
```bash
U = {A, B, C}
F = { A -> A, A -> B, A -> B, AB -> B }
```
Saída esperada:
```bash
A -> B
```

## Chave Candidata

## Verificar BCFN e 3FN
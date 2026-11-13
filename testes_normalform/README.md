# Testes do comando normalform

Executar, para cada caso:

    ./fdtool normalform --fds testes_normalform/01_ambas_ok.fds

Comparar a saída com o arquivo de mesmo nome terminado em `.expected.txt`.

Convenção: testar DFs originais após separar o RHS em atributos unitários,
ignorar DFs triviais e não repetir DFs idênticas.

As DFs são listadas na ordem em que aparecem no arquivo, e os atributos
do RHS são listados alfabeticamente. Caso a implementação use outra ordem,
a comparação deve considerar o conjunto de violações, não a ordem das linhas.

`OK` indica que a forma normal é satisfeita. `VIOLATIONS` indica o contrário.

Casos:
- 01_ambas_ok: BCNF e 3FN satisfeitas; chaves candidatas: A.
- 02_ambas_violadas: Violação de ambas, exemplo do enunciado; chaves candidatas: A.
- 03_3fn_ok_bcnf_nao: 3FN satisfeita, BCNF violada; chaves candidatas: AB, AC.
- 04_redundancia: DF redundante: listar entrada original; chaves candidatas: AD.
- 05_duas_violacoes_bcnf: Duas violações de BCNF, 3FN satisfeita; chaves candidatas: AC, BC.
- 06_rhs_multiplo: RHS não unitário, separar violações; chaves candidatas: A.
- 07_trivial: DF trivial não deve ser reportada; chaves candidatas: AB.
- 08_superchave_nao_minima: AB é superchave, embora não seja chave candidata; chaves candidatas: A.

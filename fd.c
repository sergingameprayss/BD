#include <stdio.h>
#include <string.h>
#include "fd.h"

void init_fd_set (fd_set_t *sys) {
    for (int i = 0; i < MAX_ATTRS; i++) {
        sys->u[i] = false;
    }
    sys->size = 0;
}

// the attr_clos need to be at minimum 26bytes long
int closure (const fd_set_t *sys, const char *attr, bool *attr_clos) {
    if (!sys || !attr_clos) {
        return ERROR;
    }
    
    // bool already_seen[MAX_ATTRS] = {0};
    bool new_attr = true;

    // set the attributes of the closure as part of the closure. reflexive rule
    for (int i = 0; attr[i] != '\0'; i++) { //not the most secure way, but we can handle this latter with some size param
        attr_clos[attr[i] - 'A'] = true;
    }
    
    while (new_attr ) {
        new_attr = false;

        for (int i = 0; i < sys->size; i++) {
            bool lhs_valid = true;
            for (int j = 0; j < MAX_ATTRS; j++) {
                if (sys->fds[i].lhs[j] && !attr_clos[j]) {// this means that the lhs attr needed arent in the closure of these attrs...
                    lhs_valid = false;
                    break;
                }
            }
            if (lhs_valid) {
                for (int j = 0; j < MAX_ATTRS; j++) {
                    if (sys->fds[i].rhs[j] && !attr_clos[j]) {
                        attr_clos[j] = true;
                        new_attr = true; //new attr discorvered
                    }
                }
            }

        }
    }

    return OK;
}

int mincover(const fd_set_t *sys, fd_set_t *fd_mincover){
    if(!sys || !fd_mincover)
        return ERROR;
    
    fd_mincover->size = 0;
    // decompose rhs to have unique attr
    for(int i = 0; i < sys->size; i++){ // for each fd
        for(int j = 0; j < MAX_ATTRS; j++){
            // for each rhs, create new fd
            if(sys->fds[i].rhs[j]){
                int idx = fd_mincover->size;
                //create new fd, fill up with false 
                for(int k = 0; k < MAX_ATTRS; k++){
                    fd_mincover->fds[idx].lhs[k] = fd_mincover->fds[idx].rhs[k] = false;
                }
                
                for(int k = 0; k < MAX_ATTRS; k++){// copy all lhs, because a lhs can have more than one letter
                    fd_mincover->fds[idx].lhs[k] = sys->fds[i].lhs[k];
                }

                fd_mincover->fds[idx].rhs[j] = true; // just one letter in rhs per lhs

                fd_mincover->size++; // one more fd
            }
        }
    }

    // remove redundant attr from lhs
    for(int i = 0; i < fd_mincover->size; i++){
        int count_lhs_attr = 0;
        //count how many letters the lhs of fd has
        for(int k = 0; k < MAX_ATTRS; k++){
            if(fd_mincover->fds[i].lhs[k])
                count_lhs_attr++;
        }
        if(count_lhs_attr > 1){ // if lhs has more than one letter
            for(int k = 0; k < MAX_ATTRS; k++){
                //for each lhs letter in a fd, remove a letter and see if this letter is useful
                if(fd_mincover->fds[i].lhs[k]){
                    //fd_mincover->fds[i].lhs[k] = false; //"remove" letter

                    // pick up the lhs ignoring k -> string
                    char lhs_wo_k[MAX_ATTRS+1];
                    int idx = 0;
                    for(int m = 0; m < MAX_ATTRS; m++){
                        if(fd_mincover->fds[i].lhs[m] && m != k)
                            lhs_wo_k[idx++] = 'A' + m; 
                    }
                    lhs_wo_k[idx] = '\0';

                    // calculate closure of lhs without k
                    bool attr_clos[MAX_ATTRS] = {false};
                    closure(fd_mincover, lhs_wo_k, attr_clos);

                    bool lhs_attr_useless = false;
                    for(int l = 0; l < MAX_ATTRS; l++){
                        // if rhs is in closure, this means that k is useless for lhs
                        if(fd_mincover->fds[i].rhs[l] && attr_clos[l]){
                            lhs_attr_useless = true; // if i can reach rhs without the removed letter, this letter is useless
                            break;
                        }
                            
                    }
                    if(lhs_attr_useless){
                        fd_mincover->fds[i].lhs[k] =false; // k is useless, so we can remove k 
                    } 
                }
            }
        }
    }


    // remove redundant fd
    for(int i = 0; i < fd_mincover->size; i++){
        fd_t temp_fd = fd_mincover->fds[i];
        //remove fd and save lhs to do the closure
        char lhs[MAX_ATTRS+1];
        int idx=0;
        for(int j = 0; j < MAX_ATTRS; j++){
            if(temp_fd.lhs[j] ){
                lhs[idx++] = 'A' +j;
            }
        }
        lhs[idx] = '\0';

        if (idx == 0) continue; // if lhs empty

        for (int j = 0; j < MAX_ATTRS; j++) {
            fd_mincover->fds[i].lhs[j] = false;
            fd_mincover->fds[i].rhs[j] = false;
        }

        // calculate the closure
        bool attr_clos[MAX_ATTRS] = {false};
        closure(fd_mincover, lhs, attr_clos);

        bool fd_useless = false;
        for(int l = 0; l < MAX_ATTRS; l++){
            // if rhs is in closure, this means that this fd is useless 
            if(temp_fd.rhs[l] && attr_clos[l]){
                fd_useless = true; // if i can reach rhs without the removed letter, this letter is useless
                break;
            }
        }
        if(!fd_useless){
            fd_mincover->fds[i] = temp_fd; //if not useless, give back the fd
        } else{
            // if i removed reduntant fds, i must update size
            //pull the fds 
            for (int k = i; k < fd_mincover->size - 1; k++) {
                fd_mincover->fds[k] = fd_mincover->fds[k + 1];
            }
            fd_mincover->size--;
            i--;
        }
    }

    

    return OK;
}

// Verifca se o parâmetro test_set é uma superchave a partir do calculo do fecho de sys
static bool is_superkey(const fd_set_t *sys, const bool *test_set){
    // Converte test_set para string de atributos
    char attr_str[MAX_ATTRS+1];
    int idx = 0;
    for (int i = 0; i < MAX_ATTRS; i++){
        if (test_set[i]){
            attr_str[idx] = 'A' + i;
            idx++;
        }
    }
    attr_str[idx] = '\0';
    
    // Calculo do fecho
    bool attr_closure[MAX_ATTRS] = {false};
    closure(sys, attr_str, attr_closure);

    // Verifica se eh superchave
    for (int i = 0; i < MAX_ATTRS; i++){
        if (sys->u[i] && !attr_closure[i]){ // Atributo esta em U, mas nao esta no fecho de test_set
            return false;
        }
    }
    return true;
}

// Verifica minimalidade de chaves candidatas, verificando se o parâmetro test_set contém alguma chave candidada de keys descobreta anteriormente.
static bool has_prev_key(bool *test_set, bool keys[][MAX_ATTRS], int num_keys){
    for (int i = 0; i < num_keys; i++){
        bool is_subset = true;

        for (int j = 0; j < MAX_ATTRS; j++){
            if (keys[i][j] && !test_set[j]){
                is_subset = false;
                break;
            }
        }

        if (is_subset){
            return true;
        }
    }

    return false;
}

// Gera combinacoes de tamanho target_size de atributos a partir de sys.
// Calcula a quantidade de chaves em *num_keys.
static void generate_combinations(const fd_set_t *sys, 
                                  const int *remaning_idx, int num_remaning, 
                                  int start_idx, int chosen_count, int target_size, 
                                  bool *test_set, bool keys[][MAX_ATTRS], int *num_keys){

    // Caso base
    if (chosen_count == target_size){
        if (!has_prev_key(test_set, keys, *num_keys) && is_superkey(sys, test_set)){
            memcpy(keys[*num_keys], test_set, MAX_ATTRS * sizeof(bool));
            (*num_keys)++;
        }
        return;
    }

    // Passo recursivo
    for (int i = start_idx; i < num_remaning; i++){
        int attr_idx = remaning_idx[i];

        test_set[attr_idx] = true;
        generate_combinations(sys, remaning_idx, num_remaning, i + 1, chosen_count + 1, target_size, test_set, keys, num_keys);
        test_set[attr_idx] = false;
    }
}

// Calcula todas as chaves candidadas de um conjunto de DFs a partir do parâmetro sys, salvando todas as chaves candidatas em keys.
// Retorna a quantidade de chaves candidatas encontradas ou -1 em caso de erro.
int candidate_keys(const fd_set_t *sys, bool keys[][MAX_ATTRS]){
    if (!sys || !keys) return -1;

    bool essentials[MAX_ATTRS] = {false};
    bool has_essentials = false;
    int remaning_idx[MAX_ATTRS];
    int num_remaning = 0;

    // Separa atributos essenciais dos restantes
    for (int i = 0; i < MAX_ATTRS; i++){
        if (sys->u[i]){
            bool appears_at_rhs = false;

            for (int j = 0; j < sys->size; j++){
                if (sys->fds[j].rhs[i] && !sys->fds[j].lhs[i]){ // DF existe no lado direito e não existe no esquerdo
                    appears_at_rhs = true;
                    break;
                }
            }

            // Atributo sys->u[i] é determinado por alguem
            if (appears_at_rhs){
                remaning_idx[num_remaning] = i;
                num_remaning++;
            }
            // Atributo sys->u[i] nao eh determinado por nenhum outro atributo
            // Existe atributo essencial
            else {
                has_essentials = true;
                essentials[i] = true;
            }
        }
    }

    int num_keys = 0;

    // Testa se essentials já forma uma superchave
    if (has_essentials && is_superkey(sys, essentials)){
        memcpy(keys[0], essentials, MAX_ATTRS * sizeof(bool));
        num_keys = 1;

        return num_keys;
    }

    // Testa combinacoes dos atributos restantes, em ordem crescente
    for (int tam = 1; tam <= num_remaning; tam++){
        bool test_set[MAX_ATTRS];
        memcpy(test_set, essentials, MAX_ATTRS * sizeof(bool));
        generate_combinations(sys, remaning_idx, num_remaning, 0, 0, tam, test_set, keys, &num_keys);
    }

    return num_keys;
}

// Verifica se o conjunto de DFs respeitam (ou nao) a 3FN e BCNF.
// Retorna OK se foi concluido com sucesso e ERROR em caso de erro.
int normal_form(const fd_set_t *sys){
    if (!sys) return ERROR;

    bool keys[MAX_FDS][MAX_ATTRS];
    int num_keys = candidate_keys(sys, keys);

    // Verifica atributos primos
    bool is_prime[MAX_ATTRS] = {false};
    for (int i = 0; i < num_keys; i++){
        for (int j = 0; j < MAX_ATTRS; j++){
            if (keys[i][j]) is_prime[j] = true;
        }
    }

    bool has_bcnf_violated = false;
    bool has_tnf_violated = false;
    bool bcnf_violated[sys->size];
    bool tnf_violated[sys->size];
    memset(bcnf_violated, false, sys->size * sizeof(bool));
    memset(tnf_violated, false, sys->size * sizeof(bool));

    // Verifica violacoes de 3NF e BCNF
    for (int i = 0; i < sys->size; i++){
        
        // Verifica existencia de atributos triviais
        bool is_trivial = true;
        for (int j = 0; j < MAX_ATTRS; j++){
            if (sys->fds[i].rhs[j] && !sys->fds[i].lhs[j]){
                is_trivial = false;
                break;
            }
        }

        // Violacoes das normas
        // Violacao da BCNF pela definicao
        if (!is_trivial && !(is_superkey(sys, sys->fds[i].lhs))){
            bcnf_violated[i] = true;
            has_bcnf_violated = true;
    
            for (int j = 0; j < MAX_ATTRS; j++){
                // Violacao da 3NF pela definicao
                if (sys->fds[i].rhs[j] && !sys->fds[i].lhs[j] && !is_prime[j]){
                    tnf_violated[i] = true;
                    has_tnf_violated = true;
                    break;
                }
            }
        }
    }

    // Informacoes de violacoes de alguma norma junto dos motivos (caso violem)
    if (has_bcnf_violated) printf("BCNF: VIOLATIONS\n");
    else printf("BCNF: OK\n");

    if (has_tnf_violated) printf("3NF: VIOLATIONS\n");
    else printf("3NF: OK\n");

    for (int i = 0; i < sys->size; i++){
        if (bcnf_violated[i]){
            printf("VIOLATION BCNF: ");
            print_attrs(sys->fds[i].lhs);
            printf("->");
            print_attrs(sys->fds->rhs);
            printf(" (");
            print_attrs(sys->fds[i].lhs);
            printf(" not superkey)\n");
        }

        if (tnf_violated[i]){
            // Verifica todos os atributos nao-primos
            bool non_primes[MAX_ATTRS] = {false};
            for (int j = 0; j < MAX_ATTRS; j++){
                if (sys->fds[i].rhs[j] && !sys->fds[i].lhs[j] && !is_prime[j]){
                    non_primes[j] = true;
                }
            }

            printf("VIOLATION 3FN: ");
            print_attrs(sys->fds[i].lhs);
            printf("->");
            print_attrs(sys->fds[i].rhs);
            printf(" (");
            print_attrs(non_primes);
            printf(" not prime, )");
            print_attrs(sys->fds[i].lhs);
            printf(" not superkey)\n");
        }
    }

    return OK;
}

// Imprime um conjunto qualquer no formato:
// X = {Y1, Y2, ..., Yn, }
void print_set (const char *attrs, bool *s, int s_size) {
    printf("%s = { ", attrs);
    for (int i = 0; i < s_size; i++) {
        if (s[i]){
            printf("%c", 'A' + i);
        }
    }
    printf(" }\n");
}

// Imprime todos os atributos de um lado de uma DF no formato:
// ATTRS
void print_attrs(const bool *attrs){
    for (int i = 0; i < MAX_ATTRS; i++){
        if (attrs[i]) printf("%c", 'A' + i);
    }
}

// Imprime todas as chaves candidatas de um conjunto de DFs no formato:
// K11K12...K1n
// K21K22...K2n
// ...
// Kn1Kn2...Knn
void print_keys(bool keys[][MAX_ATTRS], int num_keys){
    for (int i = 0; i < num_keys; i++){
        print_attrs(keys[i]);
        printf("\n");
    }
}

// Imprime uma dependência funcional no formato:
// lhs -> rhs
void print_single_fd(const fd_t *fd){
    print_attrs(fd->lhs);
    printf(" -> ");
    print_attrs(fd->rhs);
}

// Imprime o conjunto de dependências funcionais no formato:
// lhs1 -> rhs1
// lhs2 -> rhs2
// ...
void print_fd(fd_set_t *sys){
    if (!sys) {
        fprintf(stderr, "ERROR SYS SET DO NULL");
        return;
    }

    for(int i = 0; i < sys->size; i++){
        print_single_fd(&sys->fds[i]);
        printf("\n");
    }
}

// Imprime o conjunto de dependências funcionais no formato:
// F = {fds1 -> fds1, fds2 -> fds2, ..., fdsn -> fdsn, }
void print_fd_set (fd_set_t *sys) {
    if (!sys) {
        fprintf(stderr, "ERROR SYS SET DO NULL");
        return;
    }

    
    print_set("U", sys->u, MAX_ATTRS);

    printf("F = { ");
    for (int i = 0; i < sys->size; i++) {
        print_single_fd(&sys->fds[i]);
        printf(", ");
    }
    printf("}\n");
}
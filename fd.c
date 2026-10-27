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
    bool attr_closure[MAX_ATTRS];
    for (int i = 0; i < MAX_ATTRS; i++){
        attr_closure[i] = false;
    }
    closure(sys, attr_str, attr_closure);

    // Verifica se eh superchave
    for (int i = 0; i < MAX_ATTRS; i++){
        if (sys->u[i] && !attr_closure[i]){ // Atributo esta em U mas nao esta no fecho de test_set
            return false;
        }
    }
    return true;
}

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

int candidate_keys(const fd_set_t *sys, bool keys[][MAX_ATTRS]){
    int num_keys = 0;
    bool essentials[MAX_ATTRS];
    bool has_essentials = false;
    for (int i = 0; i < MAX_ATTRS; i++){
        essentials[i] = false;
    }

    int remaning_idx[MAX_ATTRS];
    int num_remaning = 0;

    // Separa atributos essenciais dos restantes
    for (int i = 0; i < MAX_ATTRS; i++){
        if (sys->u[i]){
            bool appears_at_rhs = false;

            for (int j = 0; j < sys->size; j++){
                if (sys->fds[j].rhs[i] && !sys->fds[j].lhs[i]){
                    appears_at_rhs = true;
                    break;
                }
            }

            if (appears_at_rhs){
                remaning_idx[num_remaning] = i;
                num_remaning++;
            }
            else {
                has_essentials = true;
                essentials[i] = true;
            }
        }
    }

    // Testa se essentials já forma uma superchave
    if (has_essentials && is_superkey(sys, essentials)){
        for (int i = 0; i < MAX_ATTRS; i++){
            keys[0][i] = essentials[i];
        }

        num_keys = 1;
        return num_keys;
    }

    // Testa combinacoes dos atributos restantes, em ordem crescente
    int total_mask = (1 << num_remaning);

    for (int tam = 1; tam <= num_remaning; tam++){
        for(int mask = 1; mask < total_mask; mask++){
            bool test_set[MAX_ATTRS];
            memcpy(test_set, essentials, MAX_ATTRS);
            int num_bits = 0;

            for (int bit = 0; bit < num_remaning; bit++){
                if (mask & (1 << bit)){
                    num_bits++;
                    int attr_idx = remaning_idx[bit];
                    test_set[attr_idx] = true;
                }
            }

            if (num_bits == tam){
                if (!(has_prev_key(test_set, keys, num_keys))){
                    if (is_superkey(sys, test_set)){
                        for (int i = 0; i < MAX_ATTRS; i++){
                            keys[num_keys][i] = test_set[i];
                        }
                        num_keys++;
                    }
                }
            }
        }
    }

    return num_keys;
}

void print_keys(bool keys[][MAX_ATTRS], int num_keys){
    for (int i = 0; i < num_keys; i++){
        for (int j = 0; j < MAX_ATTRS; j++){
            if (keys[i][j]){
                printf("%c", 'A' + j);
            }
        }
        printf("\n");
    }
}

int normal_form(const fd_set_t *sys){
    bool keys[MAX_FDS][MAX_ATTRS];
    int num_keys = candidate_keys(sys, keys);

    bool is_prime[MAX_ATTRS];
    for (int i = 0; i < MAX_ATTRS; i++){
        is_prime[i] = false;
    }

    for (int i = 0; i < num_keys; i++){
        for (int j = 0; j < MAX_ATTRS; j++){
            if (keys[i][j]) is_prime[j] = true;
        }
    }


    bool bcnf_violated[sys->size];
    bool tf_violated[sys->size];
    for (int i = 0; i < sys->size; i++){
        tf_violated[i] = bcnf_violated[i] = false;
    }

    bool has_bcnf_violated = false;
    bool has_tf_violated = false;

    for (int i = 0; i < sys->size; i++){
        bool is_trivial = true;

        for (int j = 0; j < MAX_ATTRS; j++){
            if (sys->fds[i].rhs[j] && !sys->fds[i].lhs[j]){
                is_trivial = false;
                break;
            }
        }

        if (is_trivial || (is_superkey(sys, sys->fds[i].lhs))) continue;

        bcnf_violated[i] = true;
        has_bcnf_violated = true;

        for (int j = 0; j < MAX_ATTRS; j++){
            if (sys->fds[i].rhs[j] && !sys->fds[i].lhs[j] && !is_prime[j]){
                tf_violated[i] = true;
                has_tf_violated = true;
                break;
            }
        }
    }

    if (has_bcnf_violated){
        printf("BCNF: VIOLATIONS\n");
    }
    else{
        printf("BCNF: OK\n");
    }

    if (has_tf_violated){
        printf("3NF: VIOLATIONS\n");
    }
    else {
        printf("3NF: OK\n");
    }

    for (int i = 0; i < sys->size; i++){
        if (bcnf_violated[i]){
            printf("VIOLATION BCNF: ");
            for (int j = 0; j < MAX_ATTRS; j++){
                if (sys->fds[i].lhs[j]){
                    printf("%c", 'A' + j);
                }
            }
            printf("->");
            for (int j = 0; j < MAX_ATTRS; j++){
                if(sys->fds[i].rhs[j]){
                    printf("%c", 'A' + j);
                }
            }

            printf(" (");
            for (int j = 0; j < MAX_ATTRS; j++){
                if(sys->fds[i].lhs[j]){
                    printf("%c", 'A'+j);
                }
            }
            printf(" not superkey)\n");
        }

        if (tf_violated[i]){
            printf("VIOLATION 3FN: ");
            for (int j = 0; j < MAX_ATTRS; j++){
                if (sys->fds[i].lhs[j]){
                    printf("%c", 'A' + j);
                }
            }
            printf("->");
            for (int j = 0; j < MAX_ATTRS; j++){
                if(sys->fds[i].rhs[j]){
                    printf("%c", 'A' + j);
                }
            }

            printf(" (");
            for (int j = 0; j < MAX_ATTRS; j++){
                if(sys->fds[i].rhs[j] && !sys->fds[i].lhs[j] && !is_prime[j]){
                    printf("%c", 'A'+j);
                }
            }
            printf(" not prime, )");
            for (int j = 0; j < MAX_ATTRS; j++){
                if(sys->fds[i].lhs[j]){
                    printf("%c", 'A'+j);
                }
            }
            printf(" not superkey)\n");
        }
    }

    return OK;
}

void print_set (const char *attrs, bool *s, int s_size) {
    printf("%s = { ", attrs);
    for (int i = 0; i < s_size; i++) {
        if (s[i])
            printf("%c, ", 'A' + i);
    }
    printf("}\n");
}

void print_fd(fd_set_t *sys){
    if (!sys) {
        fprintf(stderr, "ERROR SYS SET DO NULL");
        return;
    }
    for(int i = 0; i < sys->size; i++){
        for (int j = 0; j < MAX_ATTRS; j++) {
            if (sys->fds[i].lhs[j])
                printf("%c", 'A' + j);
        }
        printf(" -> ");
        for (int j = 0; j < MAX_ATTRS; j++) {
            if (sys->fds[i].rhs[j])
                printf("%c", 'A' + j);
        }
        printf("\n");
    }
}


void print_fd_set (fd_set_t *sys) {
    if (!sys) {
        fprintf(stderr, "ERROR SYS SET DO NULL");
        return;
    }
    char u = 'U';
    print_set(&u, sys->u, MAX_ATTRS);

    printf("F = { ");
    for (int i = 0; i < sys->size; i++) {
        
        for (int j = 0; j < MAX_ATTRS; j++) {
            if (sys->fds[i].lhs[j])
                printf("%c", 'A' + j);
        }
        printf(" -> ");
        for (int j = 0; j < MAX_ATTRS; j++) {
            if (sys->fds[i].rhs[j])
                printf("%c", 'A' + j);
        }
        printf(", ");
    }
    printf("}\n");

}

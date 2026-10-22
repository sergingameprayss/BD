#include <stdio.h>
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

void print_set (const char *attrs, bool *s, int s_size) {
    printf("%s = { ", attrs);
    for (int i = 0; i < s_size; i++) {
        if (s[i])
            printf("%c, ", 'A' + i);
    }
    printf("}\n");
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

#include <stdio.h>
#include "fd.h"

void init_fd_set (fd_set_t *sys) {
    for (int i = 0; i < MAX_ATTRS; i++) {
        sys->u[i] = false;
    }
    sys->size = 0;
}

void print_fd_set (fd_set_t *sys) {

    printf("U = { ");
    for (int i = 0; i < MAX_ATTRS; i++) {
        if (sys->u[i])
            printf("%c, ", 'A' + i);
    }
    printf("}\n");

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

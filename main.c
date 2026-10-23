// #include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
// #include <unistd.h>
#include <string.h>
#include <getopt.h>

#include "fd.h"
#include "file_handler.h"

#define path "exemplos/"

typedef enum {
    MIN_COVER,
    KEYS, 
    NORMAL,
    CLOSURE
} operations;

char *p_name;


void print_usage () {
    printf("Usage:\n");
    printf("%s mincover / keys / normalform --fds <arquivo>\n", p_name);
    printf("%s closure --fds <arquivo> --X <atributo>\n", p_name);
}

int handle_closure (int argc, char **argv, fd_set_t *sys) {
    char *file_name = NULL;
    char *atributes = NULL;
    int opt;

    if (strcmp(argv[1], "closure")) {
        fprintf(stderr, "ERROR");
        return ERROR;
    }
    init_fd_set(sys);

    struct option long_options[] = {
        {"fds", required_argument, NULL, 'f'},
        {"X", required_argument, NULL, 'x'},
        {NULL, 0, NULL, 0},
    };
    
    optind = 2;

    while ((opt = getopt_long(argc, argv, "", long_options, NULL)) != -1) {
        switch (opt){
            case 'f':
                file_name = optarg;
                break;
            case 'x':
                atributes = optarg;
                break;
            case '?':
                return 1;
            default:
                break;
        }
    }

    if (!file_name || !atributes) {
        fprintf(stderr, "ERROR: 'closure' requires the both flags!\n");
        print_usage(argv[0]);
        return 1;
    }

    //make what we need boy
    printf("closure pae\n");
    parse_fds_file(file_name, sys);
    print_fd_set(sys);

    bool atributes_closure[MAX_ATTRS];

    closure(sys, atributes, atributes_closure);

    print_set(atributes, atributes_closure, MAX_ATTRS);

    return OK;
}
// we can merge this function with closure or just let all operations functions separeted and let this code in main...
// TODO review switch case 
int handle_standards (const char *op_name, int argc, char **argv, fd_set_t *sys) {
    char *file_name = NULL;
    int opt, operation;

    if (!strcmp(op_name, "mincover"))
        operation = MIN_COVER;
    else if (!strcmp(op_name, "keys"))
        operation = KEYS;
    else if (!strcmp(op_name, "normalform"))
        operation = NORMAL;
    else {
        fprintf(stderr, "ERROR: operation dont exists");
        print_usage();
        return ERROR;
    }


    struct option long_options[] = {
        {"fds", required_argument, NULL, 'f'},
        {NULL, 0, NULL, 0},
    };

    optind = 2;

    while ((opt = getopt_long(argc, argv, "", long_options, NULL)) != -1) {
        switch (opt){
            case 'f':
                file_name = optarg;
                break;
            default:
                print_usage(argv[0]);
                return ERROR;
        }
    }

    if (!file_name) {
        fprintf(stderr, "ERROR: requires the --fds flag\n");
        print_usage(argv[0]);
        return ERROR;
    }

    printf("boa dogao!  operação escolhida foi: %s\n", op_name);
    parse_fds_file(file_name, sys);
    print_fd_set(sys);

    switch (operation) {
        case MIN_COVER:{
            struct fd_set fd_mincover;
            mincover(sys, &fd_mincover);
            print_fd(&fd_mincover);
        }
        case KEYS:
            //TODO implement this too
            break;
        case NORMAL: 
            //TODO implement this too too
            break;
        default:
            return ERROR;
        }


    return OK;
}


int main (int argc, char *argv[]) {
    char *op = NULL;
    fd_set_t sys;

    p_name = argv[0];
    if (argc < 2) { 
        print_usage();
        return ERROR;
    }

    op = argv[1];


    if (!strcmp(op, "closure"))
        handle_closure(argc, argv, &sys);
    else 
        handle_standards(op, argc, argv, &sys);

    return OK;
}
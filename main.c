// #include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
// #include <unistd.h>
#include <string.h>
#include <getopt.h>

#define path "exemplos/"

void print_usage (char *p_name) {
    printf("Usage:\n");
    printf("%s mincover / keys / normalform --fds <arquivo>\n", p_name);
    printf("%s closure --fds <arquivo> --X <atributo>\n", p_name);
}

int handle_closure (int argc, char **argv) {
    char *file_name = NULL;
    char *atributes = NULL;
    int opt;

    struct option long_options[] = {
        {"fds", required_argument, NULL, 'f'},
        {"x", required_argument, NULL, 'x'},
        {NULL, 0, NULL, 0},
    };
    
    optind = 2;

    while ((opt = getopt_long(argc, argv, "", long_options, NULL)) != -1) {
        switch (opt){
            case 'f':
                file_name = optarg;
                break;
            case 'X':
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

    return 0;
}

int handle_standards (const char *op_name, int argc, char **argv) {
    char *file_name = NULL;
    int opt;

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
                return 1;
        }
    }

    if (!file_name) {
        fprintf(stderr, "ERROR: requires the --fds flag\n");
        print_usage(argv[0]);
        return 1;
    }

    printf("boa dogao!  operação escolhida foi: %s\n", op_name);

    return 0;
}


int main (int argc, char *argv[]) {
    char *op = NULL;


    if (argc < 2) { 
        print_usage(argv[0]);
        return 1;
    }

    op = argv[1];

    if (!strcmp(op, "closure")) {
        handle_closure(argc, argv);
    } else if (!strcmp(op, "mincover") || !strcmp(op, "keys") || !strcmp(op, "normalform")) {
        handle_standards(op, argc, argv);
    } else {
        fprintf(stderr, "ERROR: Unknown operation: %s\n", op);
        print_usage(argv[0]);
        return 1;
    }

    return 0;
}
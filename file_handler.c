#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/socket.h>

#include "file_handler.h"


//Abrir o arquivo
FILE *file_open (const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "ERROR: Could not open file '%s'\n", filename);
        return NULL;
    }
    return file;
}
 
int read_file (FILE *file, unsigned char *buffer, unsigned long int start_read, unsigned long int size_content) {
    if (!file || !buffer || size_content == 0)
        return ERROR;
    
    /*Ler conteúdo*/
    if (fseek(file, start_read, SEEK_SET) != 0)
        return ERROR;

    return (int) fread(buffer, 1, size_content, file); // how many bytes are readed
}   

long long int get_file_size (const char *filename) {
    struct stat info;
    if(stat(filename, &info) != 0){
        printf("Falha ao pegar tamanho do arquivo\n");
        return ERROR;
    }
    return info.st_size;
}

//new functions

int reading_u (bool *u, FILE *file) {
    if (!u || !file) {
        return ERROR;
    }

    char c;
    int index = 0;
    while ((c = fgetc(file)) != EOF && c != '}' && index < MAX_ATTRS) { // the teacher doesnt limited the size of the u set...
        
        if (!(c < 'A' || c > 'Z')){ 
            u[c - 'A'] = true;  
            index++;
        }
        
    }
    if (index >= MAX_ATTRS) {
        fprintf(stderr, "ERROR: Expected %d attributes in u set, but got %d\n", MAX_ATTRS, index);
        return ERROR;
    }

    return OK; // Success :D
}

int reading_f (fd_set_t *sys, FILE *file) {
    if (!sys || !file) {
        return ERROR;
    }
    char c;
    int index = 0;
    bool reading_lhs = true; // Start by reading the lhs

    while ((c = fgetc(file)) != EOF && c != '}') {
        if (!(c < 'A' || c > 'Z')) {
            if (reading_lhs) 
                sys->fds[index].lhs[c - 'A'] = true;
            else 
                sys->fds[index].rhs[c - 'A'] = true;
            
        } else if (c == '-') {
            reading_lhs = false;
        } else if (c == ',') {
            reading_lhs = true;
            index++; // each common is a fd calculated
        }   
    }
    
    sys->size = index + 1; //TODO ver necessidade da variavel index
    if (c == EOF) {
        fprintf(stderr, "ERROR: reach the end of the file before the ending character '}'");
        return ERROR;
    }

    return OK;
}

int parse_fds_file (const char *filename, fd_set_t *fd_set) {
    FILE *file = file_open(filename);
    if (!file) {
        fprintf(stderr, "ERROR: Could not open file '%s'\n", filename);
        return ERROR;  
    }

    char c;
    

    while ((c = fgetc(file)) != EOF) {
        if (c == 'U') {
            if (reading_u(fd_set->u, file) == ERROR) {
                fclose(file);
                return ERROR;
            }
        } else if (c == 'F') {
            if (reading_f(fd_set, file) == ERROR) {
                fclose(file);
                return ERROR;
            }
        }
    }

    fclose(file);

    return OK;
}


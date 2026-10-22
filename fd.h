#ifndef FD
#define FD

#define bool int
#define true 1
#define false 0

#define MAX_ATTRS 26
#define MAX_FDS 1000

#define ERROR 1
#define OK 0


typedef struct fd {
    bool lhs[MAX_ATTRS];
    bool rhs[MAX_ATTRS];
} fd_t;

typedef struct fd_set {
    bool u[MAX_ATTRS];
    fd_t fds[MAX_FDS]; //maybe a maximum of 1000?
    int size;
} fd_set_t;

void init_fd_set (fd_set_t *sys);

void print_fd_set (fd_set_t *sys);

#endif //FD
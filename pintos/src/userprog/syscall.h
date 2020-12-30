#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

#include "lib/user/syscall.h"

void syscall_init (void);

bool check_address(void *);
bool check_argv(void *, int);
bool get_argument(void *, int *, int);

void halt (void);
void exit (int status);
pid_t exec (const char *cmd_line);
int wait (pid_t pid);
int read (int fd, void *buffer, unsigned size);
int write (int fd, const void *buffer, unsigned size);
int fibonacci(int n);
int max_of_four_int(int a, int b, int c, int d);
bool create (const char *file, unsigned initial_sizse);

// fibonacci
// max_of_four_int

#endif /* userprog/syscall.h */

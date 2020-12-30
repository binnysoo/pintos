#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

#include "lib/user/syscall.h"

void syscall_init (void);

bool check_address(void *);
bool check_argv(void *, int);
bool check_file(const char *file);
void * argv_by_index(void *, int);
bool get_argument(void *, int *, int);
struct file * get_file(int fd);

/* project 1 */
void halt (void);
void exit (int status);
pid_t exec (const char *cmd_line);
int wait (pid_t pid);
int read (int fd, void *buffer, unsigned size);
int write (int fd, const void *buffer, unsigned size);
int fibonacci(int n);
int max_of_four_int(int a, int b, int c, int d);
bool create (const char *file, unsigned initial_sizse);

/* project 2 */
bool create (const char *file, unsigned initial_size);
bool remove (const char *file);
int open (const char *file);
int filesize (int fd);
void seek (int fd, unsigned position);
unsigned tell (int fd);
void close (int fd);

struct semaphore mutex, wrt;
int readcount;
struct lock filesys_lock;

#endif /* userprog/syscall.h */

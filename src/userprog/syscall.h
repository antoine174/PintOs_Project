#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

#include <stdbool.h>
#include "threads/synch.h"

extern struct lock filesys_lock;

void syscall_init (void);
bool sys_create (const char *file, unsigned initial_size);
bool sys_remove (const char *file);
int sys_open (const char *file);                       // Returns int fd
int sys_filesize (int fd);                             // Takes int fd
int sys_read (int fd, void *buffer, unsigned size);    // Takes int fd
int sys_write (int fd, const void *buffer, unsigned size); // Takes int fd
void sys_seek (int fd, unsigned position);             // Takes int fd
unsigned sys_tell (int fd);                            // Takes int fd
void sys_close (int fd);                               // Takes int fd

#endif /* userprog/syscall.h */
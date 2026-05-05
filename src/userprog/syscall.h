#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

#include <stdbool.h>

struct lock filesys_lock;

void syscall_init (void);
bool file_create (const char *file, unsigned initial_size);
bool file_remove (const char *file);
struct file *file_open (const char *file);
int file_filesize (struct file *fd);   
int file_read (struct file *fd, void *buffer, unsigned size);
int file_write (struct file *fd, const void *buffer, unsigned size);
void file_seek (struct file *fd, unsigned position);
unsigned file_tell (struct file *fd);
void file_close (struct file *fd);

#endif /* userprog/syscall.h */

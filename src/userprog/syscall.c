#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"

static void syscall_handler (struct intr_frame *);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
  lock_init(&filesys_lock);
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
  printf ("system call!\n");
  thread_exit ();
}


bool file_create (const char *file, unsigned initial_size){
  lock_acquire(&filesys_lock);
  bool result = filesys_create(file, initial_size);
  lock_release(&filesys_lock);
  return result;
}

bool file_remove (const char *file){
  lock_acquire(&filesys_lock);
  bool result = filesys_remove(file);
  lock_release(&filesys_lock);
  return result;
}

struct file *file_open (const char *file){
  lock_acquire(&filesys_lock);
  struct file *result = filesys_open(file);
  lock_release(&filesys_lock);
  return result;
}

int file_filesize (struct file *fd){
  lock_acquire(&filesys_lock);
  int result = file_length(fd);
  lock_release(&filesys_lock);
  return result;
}

int file_fileread (struct file *fd, void *buffer, unsigned size){
  lock_acquire(&filesys_lock);
  int result = file_read(fd, buffer, size);
  lock_release(&filesys_lock);
  return result;
}

int file_filewrite (struct file *fd, const void *buffer, unsigned size){
  lock_acquire(&filesys_lock);
  deny_write(fd);
  int res = file_write(fd, buffer, size);
  allow_write(fd);
  lock_release(&filesys_lock);
  return res;
}

void file_seek (struct file *fd, unsigned position){
  lock_acquire(&filesys_lock);
  file_seek(fd, position);
  lock_release(&filesys_lock);
}

unsigned file_tell (struct file *fd){
  lock_acquire(&filesys_lock);
  unsigned result = file_tell(fd);
  lock_release(&filesys_lock);
  return result;
}

void file_close (struct file *fd){
  lock_acquire(&filesys_lock);
  file_close(fd);
  lock_release(&filesys_lock);
}
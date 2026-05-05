#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "filesys/filesys.h"
#include "filesys/file.h"
#include "devices/input.h" // Needed for input_getc()
#include "lib/user/syscall.h"
#include "threads/vaddr.h"

struct lock filesys_lock;

static void syscall_handler(struct intr_frame *);

void syscall_init(void)
{
  intr_register_int(0x30, 3, INTR_ON, syscall_handler, "syscall");
  lock_init(&filesys_lock);
}

static void
syscall_handler(struct intr_frame *f UNUSED)
{
  if (f->esp == NULL || !is_user_vaddr(f->esp)) {
      exit(-1); 
  }
  int *args = (int *)f->esp;
  int sys_call_type = args[0];
  if (sys_call_type == SYS_HALT)
  {
    halt();
  }
  else if (sys_call_type == SYS_EXIT)
  {
    int status = args[1];
    exit(status);
  }

  else if (sys_call_type == SYS_EXEC)
  {
    char *cmd_line = (char *)args[1];
    if (cmd_line == NULL || !is_user_vaddr(cmd_line)) {
        exit(-1);
    }
    f->eax = exec(cmd_line);
  }

  else if (sys_call_type == SYS_WAIT)
  {
    pid_t pid = (pid_t)args[1];
    f->eax = wait(pid);
  }

  else if (sys_call_type == SYS_CREATE)
  {
    char *file = (char *)args[1];
    unsigned intial_size = (unsigned)args[2];
    if (file == NULL || !is_user_vaddr(file)) {
        exit(-1);
    }

    f->eax = sys_create(file, intial_size);
  }

  else if (sys_call_type == SYS_REMOVE)
  {
    char *file = (char *)args[1];
   if (file == NULL || !is_user_vaddr(file)) {
        exit(-1);
    }
      f->eax = sys_remove(file);
    }

  else if (sys_call_type == SYS_OPEN)
  {
    char *file = (char *)args[1];
   if (file == NULL || !is_user_vaddr(file)) {
        exit(-1);
    }
    f->eax = sys_open(file);
  }

  else if (sys_call_type == SYS_FILESIZE)
  {
    int fd = args[1];
    f->eax = sys_filesize(fd);
  }

  else if (sys_call_type == SYS_READ)
  {
    int fd = args[1];
    void *buffer = (void *)args[2];
    unsigned size = (unsigned)args[3];
    if (buffer == NULL || !is_user_vaddr(buffer))
    {
      exit(-1);
    }
    if (fd == 0)
    {
      // hndlt STDIN
      unsigned i;
      uint8_t *local_buffer = (uint8_t *)buffer;
      for (i = 0; i < size; i++)
      {
        local_buffer[i] = input_getc();
      }
      f->eax = size;
    }
    else if (fd == 1)
    {
      // from the screen
      exit(-1);
    }
    else
    {
      f->eax = sys_read(fd, buffer, size);
    }
  }

  else if (sys_call_type == SYS_WRITE)
  {
    int fd = args[1];
    void *buffer = (void *)args[2];
    unsigned size = (unsigned)args[3];
   if (buffer == NULL || !is_user_vaddr(buffer))
    {
      exit(-1);
    }
    if (fd == 1)
    {
      // hndlt STDout
      putbuf((const char *)buffer, size);
      f->eax = size;
    }
    else if (fd == 0)
    {
      // to keyboard
      exit(-1);
    }
    else
    {
      f->eax = sys_write(fd, buffer, size);
    }
  }

  else if (sys_call_type == SYS_SEEK)
  {
    int fd = args[1];
    unsigned position = (unsigned)args[2];
    sys_seek(fd, position);
  }

  else if (sys_call_type == SYS_TELL)
  {
    int fd = args[1];
    f->eax = sys_tell(fd);
  }

  else if (sys_call_type == SYS_CLOSE)
  {
    int fd = args[1];
    sys_close(fd);
  }

  // thread_exit();
}

void exit(int status)
{
  // TODO
}

void halt()
{
  shutdown_power_off();
}

pid_t exec(const char *cmd_line){
  // TODO
}

int wait(pid_t pid)
{
  // TODO
}

/* --- HELPER FUNCTION: Convert FD to struct file * --- */
static struct file *get_file_from_fd(int fd)
{
  struct thread *t = thread_current();

  // Safety check: 0 and 1 are not valid file pointers, and array limit is 128
  if (fd < 2 || fd >= 128)
  {
    return NULL;
  }
  return t->fd_table[fd];
}
/* ---------------------------------------------------- */

bool sys_create(const char *file, unsigned initial_size)
{
  if (file == NULL)
    return false;
  lock_acquire(&filesys_lock);
  bool result = filesys_create(file, initial_size);
  lock_release(&filesys_lock);
  return result;
}

bool sys_remove(const char *file)
{
  if (file == NULL)
    return false;
  lock_acquire(&filesys_lock);
  bool result = filesys_remove(file);
  lock_release(&filesys_lock);
  return result;
}

int sys_open(const char *file)
{
  if (file == NULL)
    return -1;

  lock_acquire(&filesys_lock);
  struct file *f = filesys_open(file);
  lock_release(&filesys_lock);

  if (f == NULL)
    return -1; // File not found

  struct thread *t = thread_current();

  // Check if we have room in the FD table
  if (t->fd_next >= 128)
  {
    lock_acquire(&filesys_lock);
    file_close(f);
    lock_release(&filesys_lock);
    return -1;
  }

  // Save the file in the table and return the FD
  int fd = t->fd_next;
  t->fd_table[fd] = f;
  t->fd_next++;

  return fd;
}

int sys_filesize(int fd)
{
  struct file *f = get_file_from_fd(fd);
  if (f == NULL)
    return -1;

  lock_acquire(&filesys_lock);
  int result = file_length(f);
  lock_release(&filesys_lock);
  return result;
}

int sys_read(int fd, void *buffer, unsigned size)
{
  // Handle STDIN (Keyboard)
  // if (fd == 0)
  // {
  //   uint8_t *buf = (uint8_t *)buffer;
  //   for (unsigned i = 0; i < size; i++)
  //   {
  //     buf[i] = input_getc();
  //   }
  //   return size;
  // }

  struct file *f = get_file_from_fd(fd);
  if (f == NULL)
    return -1;

  lock_acquire(&filesys_lock);
  int result = file_read(f, buffer, size);
  lock_release(&filesys_lock);
  return result;
}

int sys_write(int fd, const void *buffer, unsigned size)
{
  // Handle STDOUT (Console)
  // if (fd == 1) {
  //     putbuf(buffer, size);
  //     return size;
  // }

  struct file *f = get_file_from_fd(fd);
  if (f == NULL)
    return -1;

  lock_acquire(&filesys_lock);
  int res = file_write(f, buffer, size);
  lock_release(&filesys_lock);
  return res;
}

void sys_seek(int fd, unsigned position)
{
  struct file *f = get_file_from_fd(fd);
  if (f == NULL)
    return;

  lock_acquire(&filesys_lock);
  file_seek(f, position);
  lock_release(&filesys_lock);
}

unsigned sys_tell(int fd)
{
  struct file *f = get_file_from_fd(fd);
  if (f == NULL)
    return 0;

  lock_acquire(&filesys_lock);
  unsigned result = file_tell(f);
  lock_release(&filesys_lock);
  return result;
}

void sys_close(int fd)
{
  struct file *f = get_file_from_fd(fd);
  if (f == NULL)
    return;

  lock_acquire(&filesys_lock);
  file_close(f);
  lock_release(&filesys_lock);

  thread_current()->fd_table[fd] = NULL;
}
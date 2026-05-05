#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "lib/user/syscall.h"
static void syscall_handler (struct intr_frame *);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
  lock_init(&filesys_lock);
}

static void
syscall_handler (struct intr_frame *f UNUSED) {
  printf ("system call!\n");
  int *args = (int *) f->esp;
  int sys_call_type = args[0];
  if( sys_call_type== SYS_HALT){
    halt();
  }
  else if(sys_call_type == SYS_EXIT){
    int status = args[1]; 
    exit(status);
  }

  else if(sys_call_type== SYS_EXEC){
  char *cmd_line = (char *) args[1];   
   f->eax = exec(cmd_line);
  }

  else if(sys_call_type ==SYS_WAIT){
  pid_t pid = (pid_t) args[1];    
  f->eax = wait(pid);
  }

  else if(sys_call_type== SYS_CREATE){
    char *file=(char *)args[1];
    unsigned intial_size=(unsigned)args[2];
    if (file == NULL){
      exit(-1);}
    
    f->eax=file_create(file,intial_size);
  }

  else if(sys_call_type==SYS_REMOVE){
    char *file=(char *)args[1];
    if (file == NULL){
      exit(-1);}
    f->eax=file_remove(file);  
  }

  else if(sys_call_type==SYS_OPEN){
    char *file=(char *)args[1];
    if (file == NULL){
      exit(-1);}
    f->eax=file_open(file);  
  }

  else if( sys_call_type== SYS_FILESIZE){
    int fd = args[1]; 
    f->eax=file_filesize(fd);
  }

  else if(sys_call_type==SYS_READ){
    int fd=args[1];
    void *buffer=(void *)args[2];
    unsigned size=(unsigned)args[3];
    if (buffer== NULL) {
        exit(-1);
    }
    if (fd == 0) {
      // hndlt STDIN
        unsigned i;
        uint8_t *local_buffer = (uint8_t *) buffer;
        for (i = 0; i < size; i++) {
            local_buffer[i] = input_getc(); 
        }
        f->eax = size; 
    }
    else if (fd == 1) {
     // from the screen
      exit(-1); 
    }
    else{
    f->eax=file_fileread(fd,buffer,size); 
    }
  }

  else if(sys_call_type==SYS_WRITE){
    int fd=args[1];
    void *buffer=(void *)args[2];
    unsigned size=(unsigned)args[3];
    if (buffer== NULL) {
        exit(-1);
    }
    if (fd == 1) {
      // hndlt STDout
      putbuf((const char *)buffer, size);
      f->eax = size;
        }
    else if (fd == 0) {
     // to keyboard
      exit(-1); 
    }
    else{
    f->eax=file_filewrite(fd,buffer,size); 

    }
  }

  else if(sys_call_type==SYS_SEEK){
    int fd=args[1];
    unsigned position=(unsigned)args[2];
    file_seek(fd,position);  
  }

  else if(sys_call_type== SYS_TELL){
    int fd=args[1];
    f->eax=file_tell(fd);
  }
  
   else if(sys_call_type== SYS_CLOSE){
    int fd=args[1];
    file_close(fd);
  }


  

  thread_exit ();
}

void exit(int status){
  //TODO
}

void halt(){
  shutdown_power_off();
}

pid_t exec(int status){
  //TODO
}

int wait(pid_t pid){
  //TODO
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

int file_filesize (int fd){
  lock_acquire(&filesys_lock);
  int result = file_length(fd);
  lock_release(&filesys_lock);
  return result;
}

int file_fileread (int fd, void *buffer, unsigned size){
  lock_acquire(&filesys_lock);
  int result = file_read(fd, buffer, size);
  lock_release(&filesys_lock);
  return result;
}

int file_filewrite (int fd, const void *buffer, unsigned size){
  lock_acquire(&filesys_lock);
  deny_write(fd);
  int res = file_write(fd, buffer, size);
  allow_write(fd);
  lock_release(&filesys_lock);
  return res;
}

void file_seek (int fd, unsigned position){
  lock_acquire(&filesys_lock);
  file_seek(fd, position);
  lock_release(&filesys_lock);
}

unsigned file_tell (int fd){
  lock_acquire(&filesys_lock);
  unsigned result = file_tell(fd);
  lock_release(&filesys_lock);
  return result;
}

void file_close (int fd){
  lock_acquire(&filesys_lock);
  file_close(fd);
  lock_release(&filesys_lock);
}
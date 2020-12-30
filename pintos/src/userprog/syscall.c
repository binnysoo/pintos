#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"

#include "devices/shutdown.h"
#include "devices/input.h"
#include "filesys/filesys.h"
#include "threads/vaddr.h"
#include <string.h>
#include "filesys/file.h"
#include "threads/synch.h"
#include "userprog/pagedir.h"
#include "userprog/process.h"

static void syscall_handler (struct intr_frame *);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
  sema_init(&mutex, 1);
  sema_init(&wrt, 1);
  readcount = 0;
  lock_init(&filesys_lock);
}

void * argv_by_index(void *esp, int i) {
	return (void *)(esp + 4 + i * 4);
}

struct file * get_file(int fd) {
	return thread_current()->fd[fd];
}

static void
syscall_handler (struct intr_frame *f) 
{
  if (f==NULL) {
	  exit(-1);
  }
  void *tmp_esp = (void *)f->esp;
  int syscall_num = *(uint32_t *)f->esp;
  int argc[] = { 0, 1, 1, 1, 2, 1, 1, 1, 3, 3, 2, 1, 1, 1, 4 }; 
  /*** check if system call number is valid ***/
  if (syscall_num < 0) {
	  exit(-1);
  }
  /*** check if the stack pointer(*esp) is in valid user space ***/
  if (!check_address(tmp_esp) || 
		  !check_argv(tmp_esp, argc[syscall_num])) {
	  exit(-1);
  }
  else {
	  /*** system call numbers defined in <syscall-nr.h> ***/
	  switch(syscall_num) {
		/*** project 2 ***/
		case SYS_HALT: // 0
			halt();
			break;
		case SYS_EXIT: // 1
			exit((int)*(uint32_t *)argv_by_index(tmp_esp, 0));
			break;
		case SYS_EXEC: // 2
			f->eax = exec(*(const char **)argv_by_index(tmp_esp, 0));
			break;
		case SYS_WAIT: // 3
			f->eax = wait(*(pid_t *)argv_by_index(tmp_esp, 0));
			break;
		case SYS_CREATE: // 4
			f->eax = create(*(const char **)argv_by_index(tmp_esp, 0), 
							*(unsigned *)argv_by_index(tmp_esp, 1));
			break;
		case SYS_REMOVE: // 5
			f->eax = remove(*(const char **)argv_by_index(tmp_esp, 0));
			break;
		case SYS_OPEN: // 6
			f->eax = open(*(const char **)argv_by_index(tmp_esp, 0));
			break;
		case SYS_FILESIZE: // 7
			f->eax = filesize((int)*(uint32_t *)argv_by_index(tmp_esp, 0));
			break;
		case SYS_READ: // 8
			f->eax = read(*(int *)argv_by_index(tmp_esp, 0),
						  (void*)*(uint32_t *)argv_by_index(tmp_esp, 1), 
			         	  *(size_t *)argv_by_index(tmp_esp, 2));
			break;
		case SYS_WRITE: // 9
			f->eax = write(*(int *)argv_by_index(tmp_esp, 0),
						   (void*)*(uint32_t *)argv_by_index(tmp_esp, 1), 
			         	   *(size_t *)argv_by_index(tmp_esp, 2));
			break;
		case SYS_SEEK: // 10
			seek((int)*(uint32_t *)argv_by_index(tmp_esp, 0),
				 *(unsigned *)argv_by_index(tmp_esp, 1));
			break;
		case SYS_TELL: // 11
			f->eax = tell((int)*(uint32_t *)argv_by_index(tmp_esp, 0));
			break;
		case SYS_CLOSE: // 12
			close((int)*(uint32_t *)argv_by_index(tmp_esp, 0));
			break;
		case SYS_FIBONACCI: //13
			f->eax = fibonacci(*(int *)argv_by_index(tmp_esp, 0));
			break;
		case SYS_MAX4INT: //14
			f->eax = max_of_four_int(*(int *)argv_by_index(tmp_esp, 0),
					*(int *)argv_by_index(tmp_esp, 1),
					*(int *)argv_by_index(tmp_esp, 2),
					*(int *)argv_by_index(tmp_esp, 3));
			break;
		/*** project 3 ***/
		/*** project 4 ***/
		default:
			exit(-1);	
			break;
	  }
  }
}

/*** check if valid user address
     exit process when not valid 
     user memory space is between 0x8048000, 0xc0000000 ***/
bool check_address (void *addr) {
	// check if valid
	// should be user memory space and not null
	if (is_user_vaddr(addr) && addr != NULL) {
		// if valid, then dereference it and check if valid data
		if (pagedir_get_page(thread_current()->pagedir, addr)) {
			return true;
		}
	}
	// if not valid, then free the page of memory and exit
	return false;
}

bool check_argv (void *addr, int argc) {
	for (int i = 0; i<argc; i++) {
		if (!check_address(argv_by_index(addr, i))) {
			return false;
		}
	}
	return true;
}

/*** check if file name is valid 
	 file name is not valid if:
	 1) null
	 2) longer than 14 
     3) same file name exists ***/
bool check_file (const char *file) {
	if (file == NULL) {
		return false;
	}
	return true;
}

/*** system call functions 
     these are different from those defined in /lib/user/syscall.c .
     actual functions for each case are defined here 
     based on the pintos manual '3.3.4 System Calls'. ***/

/*** halt function
     terminates pintos ***/
void halt (void) {
	shutdown_power_off();
}

/*** exit function 
     terminates the current user program 
	 and returns "status" to the kernel ***/
void exit (int status) {
	// get the currently running thread
	struct thread *cur_thread = thread_current();

	// print out execution message
	printf("%s: exit(%d)\n", cur_thread->name, status);
	
	// save the exit status to the thread
	cur_thread->exit_status = status;

	// exit thread
	thread_exit();
}

/*** exec function
     runs the executable whose name is given in "cmd_line" ***/
pid_t exec (const char *cmd_line) {
	int tid = process_execute(cmd_line);
/*	
	struct list_elem *e;
	struct thread *t;
	for (e=list_begin(&(thread_current()->children)); e!=list_end(&(thread_current()->children)); e=list_next(e)) {
		t = list_entry(e, struct thread, child_elem);
		if (t->tid == tid) {
			sema_down(&(t->s_load));
			break;
		}
	}
*/
	return tid; 
}

int wait (pid_t pid) {
	return process_wait(pid);
}

/*** read function ***/
int read (int fd, void *buffer, unsigned size) {
	int i = 0;
	// STDIN
	if (!check_address(buffer)) {
		exit(-1);
	}
	//lock_acquire(&filesys_lock);
	if (fd == 0) {
		while(*(char *)(buffer + i) != '\0' 
				&& *(char *)(buffer + i) != '\n'
				&& i < (int)size) {
			*(char*)(buffer + (i++)) = input_getc();
		}
		i = size;
	}
	else if (fd > 2) {
		if (get_file(fd) == NULL || !check_address(buffer)) {
			exit(-1);
			i = -1;
		}
		else {
			sema_down(&mutex);
			readcount++;
			if (readcount == 1) sema_down(&wrt);
			sema_up(&mutex);
			i = (int)file_read(get_file(fd), buffer, (off_t)size);
			sema_down(&mutex);
			readcount--;
			if (readcount ==0) sema_up(&wrt);
			sema_up(&mutex);
		}
	}
	//lock_release(&filesys_lock);
	return i;
}

/*** write function (e.g. "echo x")
	 : writes "size" bytes from "buffer" to the file "fd".
     : returns the number of bytes actually written ***/
int write (int fd, const void *buffer, unsigned size) {
	// TODO check EOF
	/*** if end of file, stop writing 
	     returns 0 if already EOF and couldn't be written at all ***/
	//lock_acquire(&filesys_lock);
	sema_down(&wrt);
	/*** if fd == 1, write to the console ***/
	// STDOUT
	if (fd == 1) {
		/*** putbuf(const char *buffer, size_t n)
		     : writes "n" characters from "buffer" to the console
			 : defined in lib/kernel/console.c ***/
		putbuf((char *)buffer, (size_t)size);
	}
	else if (fd > 2) {
		if (get_file(fd) == NULL || !check_address((void *)buffer)) {
			exit(-1);
			return -1;
		}
		size = file_write(get_file(fd), buffer, size);
	}
	sema_up(&wrt);
	//lock_release(&filesys_lock);
	return size;
}

int fibonacci (int n) {
	int x1 = 0, x2 = 1;
	int result = 0;
	for (int i = 0; i<n; i++) {
		if (i==0) result = 1;
		else {
			result = x1 + x2;
			x1 = x2;
			x2 = result;
		}
	}
	return result;
}

int max_of_four_int (int a, int b, int c, int d) {
	int max = a;
	if (max < b) max = b;
	if (max < c) max = c;
	if (max < d) max = d;
	return max;
}

/*** create function
	 : creates file "file" sized "initial_size"
	 : only creates, does not open ***/
bool create (const char *file, unsigned initial_size) {
	if (!check_file(file)) {
		exit(-1);
	}
	return filesys_create(file, (off_t)initial_size);
}

/*** remove function
	 : deletes "file" regardless of whether it is open or closed 
	 : only removes, does not close ***/ 
bool remove (const char *file) {
	if (!check_file(file)) {
		exit(-1);
	}
	return filesys_remove(file);
}

/*** open function 
	 : returns fd of the opened file, -1 if failed to open ***/
int open (const char *file) {
	struct file *fp;
	if (!check_file(file)) {
		exit(-1);
	}
	//lock_acquire(&filesys_lock);
	sema_down(&mutex);
	readcount++;
	if (readcount == 1) sema_down(&wrt);
	sema_up(&mutex);

	fp = filesys_open(file);
	if (fp == NULL) {
		//lock_release(&filesys_lock);
		sema_down(&mutex);
		readcount--;
		if (readcount == 0) sema_up(&wrt);
		sema_up(&mutex);
		return -1;
	}
	else if (!strcmp(thread_current()->name, file)) {
		file_deny_write(fp);
	}
	// fd initialized to NULL
	for (int i=3;i<MAX_FD;i++) {
		// find empty fd space
		if (get_file(i) == NULL) {
			thread_current()->fd[i] = fp;
			//lock_release(&filesys_lock);
			sema_down(&mutex);
			readcount--;
			if (readcount == 0) sema_up(&wrt);
			sema_up(&mutex);
			return i;
		}
	}
	//lock_release(&filesys_lock);
	sema_down(&mutex);
	readcount--;
	if (readcount == 0) sema_up(&wrt);
	sema_up(&mutex);
	return -1;
}

/*** filesize function
	 : returns the size of the file "fd"  ***/
int filesize (int fd) {
	return file_length(get_file(fd));
}

/*** seek function
	 : Changes the next byte to be read/write in "fd" to "position" 
	 : position can past the current eof ***/
void seek (int fd, unsigned position) {
	return file_seek(get_file(fd), (off_t)position);
}

/*** tell function
	 : returns the position of the next byte to be read/write in "fd"
	 : expressed in bytes ***/
unsigned tell (int fd) {
	return file_tell(get_file(fd));
}

/*** close function 
     : ***/
void close (int fd) {
	// already closed
	if (get_file(fd) == NULL)
		exit(-1);
	file_close(get_file(fd));
	thread_current()->fd[fd] = NULL;
}

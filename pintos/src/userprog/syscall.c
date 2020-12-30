#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"

#include "devices/shutdown.h"
#include "filesys/filesys.h"
#include "threads/vaddr.h"
#include <string.h>


static void syscall_handler (struct intr_frame *);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

void * argv_by_index(void *esp, int i) {
	return (void *)(esp + 4 + i * 4);
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
			break;
		case SYS_REMOVE: // 5
			break;
		case SYS_OPEN: // 6
			break;
		case SYS_FILESIZE: // 7
			break;
		case SYS_READ: // 8
			f->eax = read(*(int *)argv_by_index(tmp_esp, 0),
						  *(uint32_t *)argv_by_index(tmp_esp, 1), 
			         	  *(size_t *)argv_by_index(tmp_esp, 2));
			break;
		case SYS_WRITE: // 9
			f->eax = write(*(int *)argv_by_index(tmp_esp, 0),
						   *(uint32_t *)argv_by_index(tmp_esp, 1), 
			         	   *(size_t *)argv_by_index(tmp_esp, 2));
			break;
		case SYS_SEEK: // 10
			break;
		case SYS_TELL: // 11
			break;
		case SYS_CLOSE: // 12
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
	return process_execute(cmd_line);
}

int wait (pid_t pid) {
	return process_wait(pid);
}

/*** read function ***/
int read (int fd, void *buffer, unsigned size) {
	int i = 0;
	if (fd == 0) {
		while(*(char *)(buffer + i) != '\0' 
				&& *(char *)(buffer + i) != '\n'
				&& i < size) {
			*(char*)(buffer + (i++)) = input_getc();
		}
		return i;
	}
	return -1;
}

/*** write function (e.g. "echo x")
	 : writes "size" bytes from "buffer" to the file "fd".
     : returns the number of bytes actually written ***/
int write (int fd, const void *buffer, unsigned size) {
	// TODO check EOF
	/*** if end of file, stop writing 
	     returns 0 if already EOF and couldn't be written at all ***/

	/*** if fd == 1, write to the console ***/
	if (fd == 1) {
		/*** putbuf(const char *buffer, size_t n)
		     : writes "n" characters from "buffer" to the console
			 : defined in lib/kernel/console.c ***/
		putbuf((char *)buffer, (size_t)size);
	}

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

/** Compilation: gcc -o reader reader.c -lrt -lpthread **/

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>       
#include <fcntl.h>          
#include <unistd.h>
#include <semaphore.h>
#include <string.h>
#include "shmem.h"

void report_and_exit(const char* msg) {
  perror(msg);
  exit(-1);
}

int main() 
{
  int fd = shm_open(BackingFile, O_RDWR, AccessPerms); 
  if (fd < 0) 
    report_and_exit("Can't get file descriptor...");

  /* get a pointer to memory */
  caddr_t memptr = mmap(NULL, ByteSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);         
  if ((caddr_t) -1 == memptr) 
    report_and_exit("Can't access segment...");

  /* create a semaphore  */
  sem_t* semptr = sem_open(SemaphoreName, O_CREAT, AccessPerms, 0);            
  if (semptr == (void*) -1) 
    report_and_exit("sem_open");

  /* use semaphore as a mutex (lock) by waiting for writer to increment it */
  /* wait until semaphore != 0 */
  if (!sem_wait(semptr)) 
  { 
    int i;
    for (i = 0; i < strlen(MemContents); i++)
      write(STDOUT_FILENO, memptr + i, 1); 
    sem_post(semptr);
  }

  /* cleanup */
  munmap(memptr, ByteSize);
  close(fd);
  sem_close(semptr);
  unlink(BackingFile);
  return 0;
}
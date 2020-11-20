#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define FileName        "data.dat"
#define DataString       "Now is the winter of our discontent\nMade glorious summer by this sun of York\n"

void report_and_exit(const char* msg)
{
    perror(msg);
    exit(-1);
}

int main(int argc, char const *argv[])
{
    struct flock lock;
    lock.l_type = F_WRLCK;          /* read/write (exclusive) lock */
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;                 /* 0 = EOF */ 
    lock.l_pid = getpid();

    int fd;
    if((fd = open(FileName, O_RDWR | O_CREAT, 0666)) < 0)
    {
        report_and_exit("OPEN FAILED!");
    }

    /* get lock */
    if(fcntl(fd, F_SETLK, &lock) < 0)
    {
        report_and_exit("FCNTL FAILED TO GET LOCK");
    }
    else
    {
        write(fd, DataString, strlen(DataString));
        fprintf(stderr, "Process %d has written to data file\n", lock.l_pid);
    }
    
    /* release lock */
    lock.l_type = F_UNLCK;
    if(fcntl(fd, F_SETLK, &lock) < 0)
    {
        report_and_exit("FCNTL FAILED TO RELEASE LOCK");
    }

    close(fd);

    return 0;
}

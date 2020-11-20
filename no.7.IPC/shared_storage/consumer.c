#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#define FileName        "data.dat"

void report_and_exit(const char* msg)
{
    perror(msg);
    exit(-1);
}

int main(int argc, char const *argv[])
{
    struct flock lock;

    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start  = 0;
    lock.l_len = 0;
    lock.l_pid = getpid();

    int fd;
    if((fd = open(FileName, O_RDONLY)) < 0)
    {
        report_and_exit("OPEN TO READ FAILED");
    }

    /*get lock*/
    fcntl(fd, F_GETLK, &lock);
    if(lock.l_type != F_UNLCK)
    {
        report_and_exit("FILE STILL WRITE LOCKED");
    }

    lock.l_type = F_RDLCK;      /*prevents any writing*/
    if(fcntl(fd, F_SETLK, &lock) < 0)
    {
        report_and_exit("CAN'T GET READ-ONLY LOCK");
    }

    /*read data*/
    int c;
    while (read(fd, &c, 1) > 0)
    {
        write(STDOUT_FILENO, &c, 1);            /*write to standard output*/
    }
    
    /*release lock*/
    lock.l_type = F_UNLCK;
    if(fcntl(fd, F_SETLK, &lock) <0 )
    {
        report_and_exit("UNLOCKING FAILED");
    }

    close(fd);
    return 0;
}

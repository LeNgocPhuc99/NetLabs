/* parent process write data to pipe and child read data from pipe */


#include <sys/wait.h>
#include <stdio.h>
/*exit*/
#include <stdlib.h>
/*read, write, pipe, _exit*/
#include <unistd.h>
#include <string.h>

#define ReadEnd     0
#define WriteEnd    1

void report_and_exit(const char* msg)
{
    perror(msg);
    exit(-1);
}


int main(int argc, char const *argv[])
{
    int pipeFDs[2];
    char buf;
    const char* msg = "Hello world!\n";

    /* create pipe */
    if(pipe(pipeFDs) < 0)
    {
        report_and_exit("pipeFD");
    }

    /* create child process */
    pid_t cpid = fork();
    if(cpid < 0)
    {
        report_and_exit("fork");
    }

    /* child process (read only) 
       read data from pipe and echo to standard output
    */
    if (cpid == 0)
    {
        close(pipeFDs[WriteEnd]);

        while (read(pipeFDs[ReadEnd], &buf, 1))
        {
            write(STDOUT_FILENO, &buf, sizeof(buf));
        }

        close(pipeFDs[ReadEnd]);
        _exit(0);
    }
    else
    {
        /* parent: write only */
        close(pipeFDs[ReadEnd]);

        write(pipeFDs[WriteEnd], msg, strlen(msg));
        close(pipeFDs[WriteEnd]);

        /* wait child process*/
        wait(NULL);
        exit(0);
    }

    return 0;
}


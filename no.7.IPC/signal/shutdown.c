#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void graceful(int signum)
{
    printf("\tChild confirming received signal: %i\n", signum);
    puts("\tChild about to terminal gracefully");
    sleep(1);
    puts("\tChild terminating now");
    _exit(0);
}

void set_handler()
{
    struct sigaction current;
    sigemptyset(&current.sa_mask);              /* clear the signal set */
    current.sa_flags = 0;
    current.sa_handler = graceful;
    sigaction(SIGTERM, &current, NULL);         /* register the handler */
}

void child_code()
{
    set_handler();

    while (1)
    {
        sleep(1);
        puts("\tChild just woke up, but going back to sleep.");
    }
}

void parent_code(pid_t cpid)
{
    puts("Parent sleeping for a time");
    sleep(5);

    /* Terminate child */
    if(kill(cpid, SIGTERM) == -1)
    {
        perror("kill");
        exit(-1);
    }

    wait(NULL);                                    /* wait for child to terminate */
    puts("My child terminated");
}

int main(int argc, char const *argv[])
{
    /* generate child process */
    pid_t pid = fork();
    if(pid < 0)
    {
        perror("fork");
        return -1;
    }
    
    /* child process */
    if(pid == 0)
    {
        child_code();
    }
    else
    {
        parent_code(pid);
    }
    return 0;
}

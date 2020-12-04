#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>


#define     MaxLoos             1200    /* outer loop */
#define     ChunkSize           16      /* number of written at a time */
#define     IntsPerChunk        4       /* 4-bytes ints per chunk */
#define     MaxZs               250     /* max microsecond to sleep */

int main(int argc, char const *argv[])
{
    
    const char* pipeName = "./fifoChannel";         /* backing file */
    mkfifo(pipeName, 0666);
    int fd = open(pipeName, O_CREAT | O_WRONLY);    /* write only */
    if(fd < 0)
    {
        return -1;
    }

    for(int i = 0; i < MaxLoos; i++)
    {
        for(int j = 0; j < ChunkSize; j++)          /* each time, write ChunkSize bytes */
        {
            int chunk[IntsPerChunk];
            /* random data */
            for(int k = 0; k < IntsPerChunk; k++)
            {
                chunk[k] = rand();
            }
            write(fd, chunk, sizeof(chunk));
        }
        usleep((rand() % MaxZs) + 1);
    }

    close(fd);
    unlink(pipeName);
    printf("%i ints sent to the pipe\n", MaxLoos*ChunkSize*IntsPerChunk);

    return 0;
}

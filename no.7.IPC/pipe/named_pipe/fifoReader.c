#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

unsigned is_prime(unsigned n) 
{ 
    if(n < 3)
    {
        return n > 1;
    }
    if((n%2) == 0 || (n % 3) == 0)
    {
        return 0;
    }

    for(int i = 5; (i*i) <= n; i += 6)
    {
        if((n % i) == 0 || (n % (i + 2)) == 0)
        {
            return 0;
        }
    }
    return 1;
}

int main(int argc, char const *argv[])
{
    const char* file = "./fifoChannel";
    int fd = open(file, O_RDONLY);
    if(fd < 0)
    {
        return -1;
    }

    unsigned count = 0, total = 0, primes_count = 0;

    while (1)
    {
        int next;
        int i;

        ssize_t count = read(fd, &next, sizeof(int));
        if(count == 0)
        {
            break;
        }
        else
        {
            total++;
            if(is_prime(next))
            {
                primes_count++;
            }
        }
        
    }
    
    close(fd);
    unlink(file);
    printf("Received ints: %u, primes: %u\n", total, primes_count);
    return 0;
}

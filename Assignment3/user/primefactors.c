#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int 
primefactors(int argc, char* argv[])
{
    int primes[] = {2,3,5,7,11,13,17,19,23,29,31,37,41,43,47,53,59,61,67,71,73,79,83,89,97};

    if(argc != 2)
    {
        fprintf(2, "Usage : primefactors n\n");
        exit(1);
    }

    int i = 0;
    while(argv[1][i] != '\0') 
    {
        if(argv[1][i] < '0' || argv[1][i] > '9')
        {
            fprintf(2, "n has to be between 2 and 100\n");
            exit(1);
        }
        i++;
    }
    if(atoi(argv[1]) < 2 || atoi(argv[1]) > 100)
    {
        fprintf(2, "n has to be between 2 and 100\n");
        exit(1);
    }

    int n = atoi(argv[1]);
    int pipefd[2];
    int ind = 0;
    int ret_val = 0;

    while(1)
    {
        if(pipe(pipefd) < 0)
        {
            fprintf(2, "Pipe could not be created. Aborting ... \n");
            exit(1);
        }

        int f = fork();
        if(f < 0)
        {
            fprintf(2, "Error: cannot fork. Aborting...\n");
		    exit(1);
        }

        if(f == 0)
        {
            if (read(pipefd[0], &n, 1) < 0) {
                fprintf(2, "Error: cannot read. Aborting...\n");
                exit(1);
            }
            close(pipefd[0]);
            close(pipefd[1]);

            if(n == 1)
                exit(0);
        }
        else
        {
            int cnt = 0;
            while(n % primes[ind] == 0)
            {
                n /= primes[ind];
                fprintf(1, "%d, ", primes[ind]);
                cnt++;
            }

            if(cnt != 0)
            fprintf(1, "[%d]\n", getpid());

            if (write(pipefd[1], &n, 1) < 0) {
                fprintf(2, "Error: cannot write. Aborting...\n");
                exit(1);
            }
            close(pipefd[0]);
            close(pipefd[1]);
            
            wait(&ret_val);
            exit(ret_val);
        }
        ind++;
    }

    exit(0);
}
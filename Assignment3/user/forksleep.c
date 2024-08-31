#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
forksleep(int argc, char* argv[])
{
    if(argc != 3)
    {
        fprintf(2, "Usage: forksleep m n\n");
        exit(1);
    }

    int i = 0;
    while(argv[1][i] != '\0') 
    {
        if(argv[1][i] < '0' || argv[1][i] > '9')
        {
            fprintf(2, "m must be a positive integer\n");
            exit(1);
        }
        i++;
    }
    if(atoi(argv[1]) <= 0)
    {
        fprintf(2, "m must be a positive integer\n");
        exit(1);
    }

    if((argv[2][0] != '0' && argv[2][0] != '1') || argv[2][1] != '\0')
    {
        fprintf(2, "n must be either 0 or 1\n");
        exit(1);
    }

    int pid = fork();

    if(pid < 0)
    {
        fprintf(2, "Fork not created successfully\n");
        exit(1);
    }

    if(pid == 0)
    {
        if(atoi(argv[2]) == 0) 
        {
            sleep(atoi(argv[1]));
            fprintf(1, "%d: Child\n", getpid());
        }
        else
        {
            fprintf(1, "%d: Child\n", getpid());
        }
    }
    else
    {
        if(atoi(argv[2]) == 0)
        {
            fprintf(1, "%d: Parent\n", getpid());
            wait((int*) 0);
        }
        else
        {
            sleep(atoi(argv[1]));
            fprintf(1, "%d: Parent\n", getpid());
            wait((int*) 0);
        }
    }

    exit(0);
}
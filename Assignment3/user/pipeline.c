#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int 
pipeline(int argc, char* argv[])
{
    if(argc != 3)
    {
        fprintf(2, "Usage : pipeline n x\n");
        exit(1);
    }
    int sign = 1;
    int i = 0;
    while(argv[1][i] != '\0') 
    {
        if(argv[1][i] < '0' || argv[1][i] > '9')
        {
            fprintf(2, "n must be a positive integer\n");
            exit(1);
        }
        i++;
    }
    if(atoi(argv[1]) <= 0)
    {
        fprintf(2, "n must be a positive integer\n");
        exit(1);
    }

    if(!((argv[2][0] >= '0' && argv[2][0] <= '9') ||  argv[2][0] == '-'))
    {
        fprintf(2, "x should be a integer\n");
        exit(1);
    }
    if(argv[2][0]=='-')
    {
        sign = -1;
    }
    i = 1;
    while(argv[2][i] != '\0') 
    {
        if(argv[2][i] < '0' || argv[2][i] > '9')
        {
            fprintf(2, "x should be a integer\n");
            exit(1);
        }
        i++;
    }

    int n = atoi(argv[1]);
    int x;
    if(sign==1)
        x = atoi(argv[2]);
    else 
        x = -1*atoi(argv[2]+1);
    int ret_val = 0;
    int pipefd[2];

    while(1) {
        n--;
        if(n == -1)
            break;

        if(pipe(pipefd) < 0)
        {
            fprintf(2, "Pipe could not be created. Aborting ... \n");
            exit(1);
        }

        int f = fork();
        if(f < 0){
            fprintf(2, "Error: cannot fork. Aborting...\n");
		    exit(1);
        }
        else
        if(f == 0) {
            if (read(pipefd[0], &x, sizeof(int)) < 0) {
                fprintf(2, "Error: cannot read. Aborting...\n");
                exit(1);
            }
            close(pipefd[0]);
            close(pipefd[1]);
        }
        else {
            x += getpid();
            fprintf(1, "%d : %d\n", getpid(), x);
            if (write(pipefd[1], &x, sizeof(int)) < 0) {
                fprintf(2, "Error: cannot write. Aborting...\n");
                exit(1);
            }
            close(pipefd[1]);
            close(pipefd[0]);
            wait(&ret_val);
            exit(ret_val);
        }
    }

    exit(0);
}

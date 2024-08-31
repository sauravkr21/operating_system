#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char* argv[]) 
{
    if(argc != 1)
    {
        fprintf(2, "Usage: uptime\n");
        exit(1);
    }

    int up_time = uptime();

    if(up_time < 0)
    {
        fprintf(2, "uptime failed\n");
        exit(1);
    }
    else
    {
        fprintf(2, "Up time = %d\n", up_time);
        exit(0);
    }
}
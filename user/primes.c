#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void 
handle_process(int fd_father){\
lab_handle_process:
    int prime = 0;
    if(read(fd_father, &prime, 4) != 4){
        printf("primes: pipe read failed\n");
        exit(1);
        return;
    }
    if(prime==-1){
        close(fd_father);
        exit(0);
        return;
    }
    printf("prime %d\n", prime);
    int fds_son[2];
    if(pipe(fds_son) < 0){
        printf("primes: pipe failed\n");
        exit(1);
    }
    while (1==1){
        int num=0;
        if(read(fd_father, &num, 4) != 4){
            printf("primes: pipe read failed\n");
            exit(1);
        }
        if(num==-1){
            if(write(fds_son[1], &num, 4) != 4){
                printf("primes: pipe write failed\n");
                exit(1);
            }
            break;
        }
        if(num%prime==0){
            continue;
        }
        if(write(fds_son[1], &num, 4) != 4){
            printf("primes: pipe write failed\n");
            exit(1);
            return;
        }
    }
    close(fds_son[1]);
    close(fd_father);
    int pid = fork();
    if(pid == 0){
        fd_father = fds_son[0];
        goto lab_handle_process;
        exit(0);
        return;
    } else if(pid < 0){
        printf("primes: fork failed\n");
        exit(1);
    } else{
        wait(0);
        exit(0);
        return;
    }
}

int
main(int argc, char *argv[])
{
    
    if(argc != 1){
        fprintf(2, "Usage: primes \n");
        exit(1);
    }
    int fds_son[2];
    if(pipe(fds_son) < 0){
        printf("primes: pipe failed\n");
        exit(1);
    }
    int pid = fork();
    if(pid == 0){
        handle_process(fds_son[0]);
        exit(0);
    } else if(pid < 0){
        printf("primes: fork failed\n");
        exit(1);
    } else{
        int i;
        for(i=2; i<=35; i++){
            if(write(fds_son[1], &i, 4) != 4){
                printf("primes: pipe write failed\n");
                exit(1);
            }
        }
        i=-1;
        if(write(fds_son[1], &i, 4) != 4){
            printf("primes: pipe write failed\n");
            exit(1);
        }
        close(fds_son[1]);
        wait(0);
        exit(0);
    }
}

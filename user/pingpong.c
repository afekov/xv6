#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
    if(argc != 1){
        fprintf(2, "Usage: pingpong \n");
        exit(1);
    }
    int fds_father_child[2];
    int fds_child_father[2];
    if(pipe(fds_child_father) < 0){
        printf("pingpong: pipe failed\n");
        exit(1);
    }
    if (pipe(fds_father_child)<0)
    {
        printf("pingpong: pipe failed\n");
        exit(1);
    }
    int pid = fork();
    int real_pid = getpid();
    
    if(pid == 0){
        if(write(fds_father_child[1], "x", 1) != 1){
            printf("pingpong: pipe father write failed\n");
            exit(1);
        }
        char c;
        if(read(fds_child_father[0], &c, 1) != 1){
            printf("pingpong: pipe father read failed\n");
            exit(1);
        }
        sleep(10);
        printf("%d: received ping\n", real_pid);
        exit(0);
    } else if(pid < 0){
        printf("pingpong: fork failed\n");
        exit(1);
    } else{
        if(write(fds_child_father[1], "x", 1) != 1){
            printf("pingpong: pipe child write failed\n");
            exit(1);
        }
        char c;
        if(read(fds_father_child[0], &c, 1) != 1){
            printf("pingpong: pipe child read failed\n");
            exit(1);
        }
        wait(0);
        printf("%d: received pong\n", real_pid);
        close(fds_child_father[0]);
        close(fds_child_father[1]);
        close(fds_father_child[0]);
        close(fds_father_child[1]);
        exit(0);
    }
}

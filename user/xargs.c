#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"
#include "kernel/fs.h"

int MAXARGLEN = 64;

int main(int argc, char *argv[])
{
    char **new_command;
    new_command = malloc((MAXARG + 1) * sizeof(char *));
    char *buf;
    buf = malloc((MAXARGLEN + 1) * sizeof(char));
    char *rbuf;
    rbuf = buf;
    int i;
    for (i = 0; i < argc - 1; i++)
    {
        new_command[i] = argv[i + 1];
    }
    while (1 == 1)
    {
        char c;
        if (read(0, &c, 1) != 1)
        {
            exit(0);
        }
        if (c == ' ' || c == '\n')
        {
            *buf = '\x00';
            new_command[i] = rbuf;
            buf = malloc((MAXARGLEN + 1) * sizeof(char));
            rbuf = buf;
            i++;
            if (c == '\n')
            {
                *buf = '\x00';
                new_command[i] = rbuf;
                i++;
                int pid = fork();
                if (pid == 0)
                {
                    int status = exec(new_command[0], new_command);
                    exit(status);
                }
                else if (pid < 0)
                {
                    fprintf(2, "xargs: faild to fork\n");
                }
                else
                {
                    wait(0);
                    int j;
                    for (j = argc - 1; j < i; j++)
                    {
                        free(new_command[j]);
                    }
                    i = argc - 1;
                    buf = malloc((MAXARGLEN + 1) * sizeof(char));
                    rbuf = buf;
                }
            }
        }
        else
        {
            *buf = c;
            buf++;
        }
    }
}

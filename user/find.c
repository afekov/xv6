#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

char *
fmtname(char *path)
{
    char *p;

    // Find first character after last slash.
    for (p = path + strlen(path); p >= path && *p != '/'; p--)
        ;
    p++;
    return p;
}

void find(char *path, char *name)
{
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    if ((fd = open(path, 0)) < 0)
    {
        printf("find: cannot open %s\n", path);
        return;
    }

    if (fstat(fd, &st) < 0)
    {
        printf("find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    if (st.type != T_DIR)
    {
        printf("find: %s is not a directory\n", path);
        close(fd);
        return;
    }
    if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf)
    {
        printf("ls: path too long\n");
        close(fd);
        return;
    }
    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';
    while (read(fd, &de, sizeof(de)) == sizeof(de))
    {
        if (de.inum == 0)
            continue;
        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = 0;
        if (stat(buf, &st) < 0)
        {
            printf("ls: cannot stat %s\n", buf);
            continue;
        }
        char *regulate_name = fmtname(buf);
        if (st.type == T_DIR && (strcmp(".", regulate_name) != 0 && strcmp("..", regulate_name) != 0))
        {
            find(buf, name);
            continue;
        }
        if (strcmp(regulate_name, name) == 0)
        {
            printf("%s\n", buf);
        }
    }
    close(fd);
    return;
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(2, "Usage: find <path> <name>\n");
        exit(1);
    }
    find(argv[1], argv[2]);
    exit(0);
}

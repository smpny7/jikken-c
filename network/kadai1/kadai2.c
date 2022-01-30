#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(void)
{
    int fd;
    int r;
    char buf[1024 + 1];

    fd = open("input.txt", O_RDONLY);
    if (fd == -1)
    {
        printf("open err\n");
        return 1;
    }

    r = read(fd, buf, 1024);
    if (r == -1)
    {
        printf("read err\n");
    }
    else
    {
        buf[r] = '\0';
        printf("%s\n", buf);
    }
    close(fd);

    return 0;
}
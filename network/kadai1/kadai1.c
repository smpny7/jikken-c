#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(void)
{
    int fd;
    int wc = 0, w;
    char buf[1024 + 1] = {0};

    read(0, buf, 1024);

    while (buf[wc])
        wc++;
    buf[wc - 1] = '\0';

    fd = open("output.txt", O_WRONLY|O_CREAT|O_TRUNC);
    if (fd == -1)
    {
        printf("open err\n");
        return 1;
    }

    w = write(fd, buf, strlen(buf));
    if (w == -1)
    {
        printf("write err\n");
    }
    else
    {
        printf("done\n");
    }
    close(fd);

    return 0;
}
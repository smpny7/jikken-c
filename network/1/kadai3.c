#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(void)
{
    int wc = 0;
    char buf[1024 + 1] = {0};

    read(0, buf, 1024);

    while (buf[wc])
        wc++;
    buf[wc - 1] = '\0';

    printf("%s\n", buf);

    return 0;
}
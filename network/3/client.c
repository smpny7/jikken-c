#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <unistd.h>

#define BUF_SIZE 9216
#define SEND_SIZE 30000
#define FILE_BUF_SIZE 507860
#define FILE_LINE_SIZE 10000

bool show_network_log = false; /* Whether to show network logs */
bool is_loop = true;
bool is_reading = false;
bool is_writing = false;
int fd_w;
char file_buf[FILE_BUF_SIZE + 1];

/*
* Overview: Read data and register in array.
* @argument: {char} cmd - Command alphabet.
* @argument: {char *} param - Command argument.
* @return: No return
*/
void cmd_read(char *param)
{
    int fd, r;

    fd = open(param, O_RDONLY);
    if (fd == -1)
    {
        printf("[Error] File Open Error\n");
        is_reading = false;
        return;
    }

    r = read(fd, file_buf, FILE_BUF_SIZE);

    // ファイル末尾が改行でない場合
    if (file_buf[strlen(file_buf) - 1] != '\n')
        file_buf[strlen(file_buf)] = '\n';

    if (r == -1)
    {
        printf("[Error] File Read Error\n");
        is_reading = false;
        return;
    }
    close(fd);
    is_reading = true;
    return;
}

/*
* Overview: Read data and register in array.
* @argument: {char} cmd - Command alphabet.
* @argument: {char *} param - Command argument.
* @return: No return
*/
void cmd_write(char *param)
{
    fd_w = open(param, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH); // 644 Permission
    if (fd_w == -1)
    {
        printf("[Error] File Open Error\n");
        is_writing = false;
        exit(1);
    }

    is_writing = true;
    return;
}

/*
* Overview: Calls functions when the command is input.
* @argument: {char} cmd - Command alphabet.
* @argument: {char *} param - Command argument.
* @return: No return
*/
void exec_command(char cmd, char *param)
{
    switch (cmd)
    {
    case 'Q':
    case 'D':
        is_loop = false;
        break;
    // case 'C':
    //     return cmd_check(cmd);
    //     break;
    // case 'P':
    //     return cmd_print(cmd, param);
    //     break;
    case 'R':
        cmd_read(param);
        break;
    case 'W':
        cmd_write(param);
        break;
        // case 'F':
        //     return cmd_find(cmd, param);
        //     break;
        // case 'S':
        //     return cmd_sort(cmd, param);
        //     break;
        // case 'M':
        //     cmd_match(cmd, param);
        //     break;
        // default:
        //     printf(">> Unregistered Command is Entered.\n");
        //     break;
    }
}

/*
* Overview: Replaces c1 in the string with c2.
* @argument: {char *} str - String.
* @argument: {char} c1 - Replaced.
* @argument: {char} c2 - Replace.
* @return: {int} diff - Number of replacements.
*/
int subst(char *str, char c1, char c2)
{
    int diff = 0;
    char *p;

    p = str;
    while (*p != '\0')
    {
        if (*p == c1)
        {
            *p = c2;
            diff++;
        }
        p++;
    }
    return diff;
}

/*
* Overview: Separate string by the specified number of characters/times.
* @argument: {char *} str - String.
* @argument: {char *} ret[] - Separated string.
* @argument: {char} sep - Delimiter.
* @argument: {int} max - Maximum number to divide.
* @return: Number of divisions
*/
int split(char *str, char *ret[], char sep, int max)
{
    int count = 1;
    ret[0] = str;

    while (*str)
    {
        if (count >= max)
            break;
        if (*str == sep)
        {
            *str = '\0';
            ret[count++] = str + 1;
        }
        str++;
    }

    return count;
}

/*
* Overview: Get line from file or standard input.
* @argument: {char *} line - Full text.
* @return: Whether there is next line.
*/
int get_line(char *line)
{
    if (fgets(line, BUF_SIZE + 1, stdin) == NULL)
    {
        return 0;
    }
    else
    {
        subst(line, '\n', '\0');
        return 1;
    }
}

int main(int argc, char **argv)
{
    int sock;
    struct addrinfo hints, *result;
    struct in_addr addr;

    /*
    * ================================
    *  0. 今回接続するサーバ情報
    * ================================
    */
    char *hostname = "localhost";
    char *port = "8080";

    /*
    * ================================
    *  1. 通信相手のIPアドレスの取得
    * ================================
    */

    /*
    * ------------------------------------
    *  ▼ gethostbyname を使用した接続方法 ▼
    * ------------------------------------
    *
    *  struct hostent *hp;
    *
    *  hp = gethostbyname("www.edu.cs.okayama-u.ac.jp");
    *
    *  if (hp == NULL)
    *  {
    *      printf("[Error] Gethostbyname Error Occurred.\n");
    *      return 1;
    *  }
    *
    *  addr = (struct in_addr *)(hp->h_addr);
    *  printf("> IP Address: %s\n", inet_ntoa(*addr));
    *
    * ------------------------------------
    *  ▲ gethostbyname を使用した接続方法 ▲
    * ------------------------------------
    */

    bzero((char *)&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(hostname, port, &hints, &result) != 0)
    {
        printf("[Error] GetAddrInfo Error Occurred.\n");
        return 1;
    }

    addr.s_addr = ((struct sockaddr_in *)(result->ai_addr))->sin_addr.s_addr;

    if (show_network_log)
        printf("> IP Address: %s\n", inet_ntoa(addr));

    /*
    * ================================
    *  2. ソケットの作成
    * ================================
    */

    sock = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (sock == -1)
    {
        printf("[Error] Socket Error Occurred.\n");
        freeaddrinfo(result);
        return -1;
    }

    if (show_network_log)
        printf("> Socket Created Successfully.\n");

    /*
    * ================================
    *  3. 接続の確立
    * ================================
    */

    if (connect(sock, (struct sockaddr *)result->ai_addr, result->ai_addrlen) == -1)
    {
        printf("[Error] Connect Error Occurred.\n");
        close(sock);
        freeaddrinfo(result);
        return -1;
    }

    if (show_network_log)
        printf("> Connect Successfully.\n");

    char line[BUF_SIZE + 1] = {0};
    while (is_loop && get_line(line))
    {
        /*
        * ================================
        *  4. 要求メッセージを送信
        * ================================
        */

        printf("> %s\n", line);

        if (*line == '%')
        {
            exec_command(line[1], &line[3]);
        }

        int line_count = 1;
        char *ret[FILE_LINE_SIZE] = {0}, sep_line = '\n';

        if (is_reading)
            line_count = split(file_buf, ret, sep_line, FILE_LINE_SIZE) - 1;

        for (int i = 0; i < line_count; i++)
        {

            if (is_reading)
                strcpy(line, ret[i]);

            if (send(sock, (const void *)line, strlen(line), 0) == -1)
            {
                printf("[Error] Send Error Occurred.\n");
                close(sock);
                freeaddrinfo(result);
                return -1;
            }

            if (show_network_log)
                printf("> Message Sent Successfully.\n");

            /*
            * ================================
            *  5. 応答メッセージを受信
            * ================================
            */

            while (1)
            {
                char buf[SEND_SIZE] = {0};
                int recv_size = recv(sock, (void *)buf, sizeof(buf), 0);
                if (recv_size == -1)
                {
                    printf("[Error] Receive Error Occurred.\n");
                    close(sock);
                    freeaddrinfo(result);
                    return -1;
                }

                if (show_network_log)
                    printf("> Message Received Successfully.（%d bytes）\n\n", recv_size);
                if (is_writing)
                {
                    // int buf_size = buf[strlen(buf) - 1] == '\0' || buf[strlen(buf) - 1] == '\n' ? strlen(buf) - 2 : strlen(buf);
                    int w = write(fd_w, buf, strlen(buf));
                    if (w == -1)
                    {
                        printf("[Error] File Write Error\n");
                        is_writing = false;
                        exit(1);
                    }
                }
                else
                {
                    if (!is_reading || i == line_count - 1)
                        printf("%s", buf);
                }
                if (strlen(buf) < SEND_SIZE)
                    break;
            }
            if (is_writing)
            {
                close(fd_w);
                printf(">> Exported Successfully.\n\n");
            }
            is_writing = false;
        }
        is_reading = false;
    }

    close(sock);
    freeaddrinfo(result);

    return 0;
}
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

#define BUF_SIZE 9216

bool show_network_log = false; /* Whether to show network logs */
bool is_loop = true;

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
        // case 'R':
        //     cmd_read(cmd, param);
        //     break;
        // case 'W':
        //     cmd_write(cmd, param);
        //     break;
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

        // int wc = 0;
        // char message[BUF_SIZE + 1] = {0};

        // while (line[wc])
        //     wc++;
        // line[wc] = '\0';

        printf(">> %s\n", line);

        if (*line == '%')
        {
            exec_command(line[1], &line[3]);
        }

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

        char buf[BUF_SIZE] = {0};
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
        printf("%s\n", buf);
    }

    close(sock);
    freeaddrinfo(result);

    return 0;
}
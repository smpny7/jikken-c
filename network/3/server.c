#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <netdb.h>
#include <unistd.h>

#define BUF_SIZE 9216

int main(void)
{
    int sock;
    struct addrinfo hints, *result;
    struct in_addr addr;
    struct sockaddr_in sa;

    /*
    * ================================
    *  1. ソケットを作成する
    * ================================
    */

    /*
    * ----------------------
    *  1.1 サーバ情報
    * ----------------------
    */

    char *hostname = "localhost";
    char *port = "80";

    /*
    * ----------------------
    *  1.2 IPアドレスの取得
    * ----------------------
    */

    bzero((char *)&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if ((getaddrinfo(hostname, port, &hints, &result)) != 0)
    {
        printf("[Error] GetAddrInfo Error Occurred.\n");
        return 1;
    }

    addr.s_addr = ((struct sockaddr_in *)(result->ai_addr))->sin_addr.s_addr;

    printf("> IP Address: %s\n", inet_ntoa(addr));

    /*
    * ----------------------
    *  1.3 IPアドレスの取得
    * ----------------------
    */

    sock = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (sock == -1)
    {
        printf("[Error] Socket Error Occurred.\n");
        freeaddrinfo(result);
        return -1;
    }

    printf("> Socket Created Successfully.\n");

    /*
    * ================================
    *  2. ソケットに名前を付ける
    * ================================
    */

    memset((char *)&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = htonl(INADDR_ANY);
    sa.sin_port = htons((uint16_t)80);

    int yes = 1;

    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char *)&yes, sizeof(yes)) < 0)
    {
        perror("[Error] SetSockOpt Error Occurred.\n");
        close(sock);
        freeaddrinfo(result);
        return -1;
    }

    if (bind(sock, (struct sockaddr *)&sa, sizeof(sa)) == -1)
    {
        printf("[Error] Bind Error Occurred.\n");
        close(sock);
        freeaddrinfo(result);
        return -1;
    }

    printf("> Socket Bound Successfully.\n");

    /*
    * ================================
    *  3. 接続要求を待つ
    * ================================
    */

    if (listen(sock, 5) == -1)
    {
        printf("[Error] Listen Error Occurred.\n");
        close(sock);
        freeaddrinfo(result);
        return -1;
    }

    printf("> Listen Started.\n");

    /*
    * ================================
    *  4. 接続要求を受け付ける
    * ================================
    */

    socklen_t len = sizeof(struct sockaddr);
    int new_sock = accept(sock, (struct sockaddr *)&sa, &len);

    if (new_sock == -1)
    {
        printf("[Error] Accept Error Occurred.\n");
        close(sock);
        freeaddrinfo(result);
        return -1;
    }

    /*
    * ================================
    *  5. メッセージを受信する
    * ================================
    */

    char buf[BUF_SIZE] = {0};
    int recv_size = recv(new_sock, (void *)buf, sizeof(buf), 0);
    if (recv_size == -1)
    {
        printf("[Error] Receive Error Occurred.\n");
        close(sock);
        freeaddrinfo(result);
        return -1;
    }

    printf("> Message Received Successfully.（%d bytes）\n", recv_size);

    /*
    * ================================
    *  @. サーバ内部処理
    * ================================
    */

    int i = 0;
    while (buf[i])
    {
        buf[i] = toupper(buf[i]);
        i++;
    }

    /*
    * ================================
    *  6. メッセージを送信する
    * ================================
    */

    if (send(new_sock, (const void *)buf, strlen(buf), 0) == -1)
    {
        printf("[Error] Send Error Occurred.\n");
        close(sock);
        close(new_sock);
        freeaddrinfo(result);
        return -1;
    }

    printf("> Message Sent Successfully.\n");

    close(sock);
    close(new_sock);
    freeaddrinfo(result);

    return 0;
}
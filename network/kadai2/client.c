#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

#define BUF_SIZE 9216

int main(void)
{
    int sock;
    struct addrinfo hints, *result;
    struct in_addr addr;

    /*
    * ================================
    *  0. 今回接続するサーバ情報
    * ================================
    */
    char *hostname = "www.edu.cs.okayama-u.ac.jp";
    char *port = "80";

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

    printf("> Connect Successfully.\n");

    /*
    * ================================
    *  4. 要求メッセージを送信
    * ================================
    */

    char message[] = "GET /index.html\r\n";
    if (send(sock, (const void *)message, strlen(message), 0) == -1)
    {
        printf("[Error] Send Error Occurred.\n");
        close(sock);
        freeaddrinfo(result);
        return -1;
    }

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

    printf("> Message Received Successfully.（%d bytes）\n", recv_size);
    printf("%s\n", buf);

    close(sock);
    freeaddrinfo(result);

    return 0;
}
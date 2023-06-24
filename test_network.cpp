#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctype.h>
#include <strings.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <assert.h>
#include <string>
#include <iostream>

#include "httpparser.hpp"

void recvAll(int sockfd, std::string &data)
{
    char buffer[1024];
    bzero(buffer, sizeof(buffer));
    int ret = 0;
    while ((ret = recv(sockfd, buffer, sizeof(buffer), 0)) > 0)
    {
        data += buffer;
        bzero(buffer, sizeof(buffer));
    }
}

int main()
{
    int client_sock;
    int http_socket = 0;
    int http_port = 9999;
    struct sockaddr_in addr;
    http_socket = socket(AF_INET, SOCK_STREAM, 0);
    assert(http_socket);

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(http_port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY); // INADDR_ANY是一个 IPV4通配地址的常量
    if (bind(http_socket, (struct sockaddr *)&addr, sizeof(addr)))
        assert(false);
    if (listen(http_socket, 5) < 0)
        assert(false);

    printf("running on port %d\n", http_port);

    struct sockaddr_in client_addr;
    int client_addr_len = sizeof(client_addr);

    while (1)
    {
        // 阻塞等待客户端连接
        printf("wait accept...\n");
        client_sock = accept(http_socket, (struct sockaddr *)&client_addr, (socklen_t *)&client_addr_len);
        if (client_sock == -1)
            assert(false);
        std::string ss;
        recvAll(client_sock, ss);
        std::cout << "接受到的请求++++++++++++++++++++++++++" << std::endl;
        std::cout << ss;
        std::cout << "解析后的请求++++++++++++++++++++++++++" << std::endl;
        HttpRequest req;
        bool ret = req.tryDecode(ss);
        if (!ret)
        {
            std::cout << "parser error!" << std::endl;
            return 0;
        }

        std::cout << "[method] " << req.getMethod() << std::endl;
        std::cout << "[url] " << req.getUrl() << std::endl;
        std::cout << "[request params] " << std::endl;
        for (auto &p : req.getRequestParams())
        {
            std::cout << "   " << p.first << " => " << p.second << std::endl;
        }
        std::cout << "[protocol] " << req.getProtocol() << std::endl;
        std::cout << "[version] " << req.getVersion() << std::endl;
        std::cout << "[request headers] " << std::endl;
        for (auto &h : req.getHeaders())
        {
            std::cout << "   " << h.first << " => " << h.second << std::endl;
        }
        std::cout << "[body] " << std::endl
                  << "   " << req.getBody() << std::endl;
    }
}

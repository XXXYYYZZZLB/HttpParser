#include "httpparser.hpp"
#include <iostream>
int main()
{

    std::string str1 = "POST / HTTP/12.1.0\r\n"
                      "Accept: image/gif, application/msword, application//x-shockwave-flash\r\n"
                      "AAA: BBBB\r\n"
                      "Referer: http://www.google.cn\r\n"
                      "Accept-Language: zh-cn\r\n"
                      "Accept-Encoding: gzip, deflate\r\n"
                      "User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1; .NET CLR 2.0.50727; TheWorld)\r\n"
                      "Content-Length:28\r\n"
                      "Host: www.google.cn\r\n"
                      "Connection: Keep-Alive\r\n"
                      "Cookie: PREF=ID=80a0;;;\r\n"
                      "\r\n"
                      "hl=zh-CN&source=hp&q=domety";
    std::string str ="GET / HTTP/1.1\r\n"
                      "Accept-Language: zh-cn\r\n"
                      //"Content-Length:28\r\n"
                      "\r\n"
                      "hl=zh-CN&source=hp&q=domety";;

    HttpRequest req;
    bool ret = req.tryDecode(str);
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

    return 0;
}

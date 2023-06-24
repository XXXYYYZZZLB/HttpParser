# HttpParser

HttpParser是基于C++编写的http解析工具 主要使用有限状态机思想

### 使用方法

```c++
#include "httpparser.hpp"


HttpRequest req;
bool ret = req.tryDecode(str);	
```

解析本地字符串 `test_local.cpp`

```C++
#include "httpparser.hpp"
#include <iostream>
int main()
{

    std::string str = "POST /audiolibrary/music?ar=1595301089068&br=888&nn=1p1 HTTP/12.1.0\r\n"
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

```

解析网络字符串 `test_network.cpp`

```C++
```


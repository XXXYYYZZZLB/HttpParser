# HttpParser

HttpParser是基于C++编写的http解析工具 主要使用有限状态机思想

### 使用方法

```c++
#include "httpparser.hpp"

/**
 * bool tryDecode(const std::string &buf);
 * @param buf 需要解析的字符串
 * @return 解析成功返回true，失败返回false
 */

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

### 目录结构

```c++
/*
HttpParser
	├── a.out
	├── httpparser.cpp			 
	├── httpparser.hpp
	├── README.md
	├── test_local.cpp
	└── test_network.cpp
*/
```

### 编程思想

Http请求报文主要由4部分组成，分别为**请求行**、**请求头**、**空行**、**请求体**。
请求报文格式如下图所示：

![http请求报文](https://img-blog.csdnimg.cn/20210107163242738.png#pic_center)

利用有限状态机模型编程，一般编写程序时都要画出流程图，按照流程图结构来编程，如果编写一个比较繁琐，容易思维混乱的程序时，我们可以利用有限状态机模型画出一个状态转移图，这样便可以利用画出的逻辑图来编写程序，简洁且不易出错。有限状态机，（英语：Finite-state machine, FSM），又称有限状态自动机，简称状态机，是表示有限个状态以及在这些状态之间的转移和动作等行为的数学模型。

<img src="https://cdn.nlark.com/yuque/0/2023/jpeg/29181576/1687508338719-4c900572-4d7c-4bea-8798-7dc699f03021.jpeg?x-oss-process=image%2Fresize%2Cw_457%2Climit_0%2Finterlace%2C1" alt="img" style="zoom:33%;" />

**请求方法**
请求方法包括GET、HEAD、PUT、POST、TRACE、OPTIONS、DELETE等

**协议**
协议版本的格式为：HTTP/主版本号.次版本号，常用的有HTTP/1.0和HTTP/1.1

**请求头**
请求头为一系列键值对形式，格式为键:值
常见的请求头如下：

| 请求头         | 说明                               |
| -------------- | ---------------------------------- |
| Accept         | 告诉服务端客户端接受什么类型的响应 |
| Content-Length | 表示请求体的长度                   |
| cookie         | 传递的cookie                       |
| User-Agent     | 发送请求的应用程序信息             |

**解析HTTP协议的有限状态自动机**

简略图像如下所示，具体见代码及注释

![image.png](https://cdn.nlark.com/yuque/0/2023/png/29181576/1687572595105-2e8479b1-0bc4-4cb9-94c6-ede9dd1169ed.png)

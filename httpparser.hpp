#pragma once
#include <map>
#include <string>

#define CR '\r'
#define LF '\n'
#define BK ' '

/**
 * 字符串的buf，只存储对应指针，不存储实际内容
 */
struct StringBuffer
{
    char *begin = nullptr;
    char *end = nullptr; // 最后一个字符的下一个位置

    // operator std::string()
    // 任何定义此类型的类都可以在需要std::string任何地方使用
    operator std::string() const
    {
        return std::string(begin, end);
    }
};

/**
 * Http请求行的状态
 */
enum class HttpRequestDecodeState
{
    /* 状态 */
    START,  // 请求行开始，去除无效的 回车 换行 空格
    METHOD, // 请求方法
    URL,
    URL_PARAM,
    PROTOCOL,
    REQUEST_LINE_END,
    HEADER,
    HEADER_LINE_END,
    BODY,

    INVALID, // 无效
    INVALID_METHOD,
    INVALID_URI,
    INVALID_VERSION,
    INVALID_HEADER,
    COMPLETE,
};

/**
 * http的请求类
 */
class HttpRequest
{
public:
    /**
     * 解析http协议
     */
    bool tryDecode(const std::string &buf);

    const std::string &getMethod() const;

    const std::string &getUrl() const;

    const std::map<std::string, std::string> &getRequestParams() const; // url中的显示参数

    const std::string &getProtocol() const;

    const std::string &getVersion() const;

    const std::map<std::string, std::string> &getHeaders() const;

    const std::string &getBody() const;

private:
    bool parseInternal(const char *buf, int size);

    std::string _method;                                                 // 请求方法
    std::string _url;                                                    // 请求路径[不包含请求参数]
    std::map<std::string, std::string> _requestParams;                   // 请求参数
    std::string _protocol;                                               // 协议
    std::string _version;                                                // 版本
    std::string _temp_header_key;
    std::string _temp_header_val;
    std::map<std::string, std::string> _headers;                         // 所有的请求头
    std::string _body;                                                   // 请求体
    int _nextPos = 0;                                                    // 下一个位置的
    HttpRequestDecodeState _decodeState = HttpRequestDecodeState::START; // 解析状态
};
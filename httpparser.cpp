#include "httpparser.hpp"
#include <string>
#include <vector>
#include <algorithm>
#include <functional>
#include <iterator>

// DEBUG
//#include <stdio.h>

namespace
{
    bool isInvalidCharacter(char ch)
    {
        if (ch == CR || ch == LF || ch == BK)
            return true;
        return false;
    }

    void rmUselessSpace(std::string &str)
    {
        remove_if(str.begin(), str.end(), isspace);
    }
}

// 协议解析
bool HttpRequest::tryDecode(const std::string &buf)
{
    return this->parseInternal(buf.c_str(), buf.size());
}

// 解析请求行
bool HttpRequest::parseInternal(const char *buf, int size)
{
    StringBuffer method;
    StringBuffer url;

    StringBuffer requestParamKey;
    StringBuffer requestParamValue;

    StringBuffer protocol;
    StringBuffer version;

    StringBuffer headerKey;
    StringBuffer headerValue;

    int bodyLength = 0;

    bool urlBeginFlag = false; // false:未设置begin true:设置了begin
    bool urlParamKeyFlag = false;
    bool urlParamValFlag = false;
    bool urlParamKeyDoneFlag = false;
    bool protocolFalg = false;
    bool protocolDoneFalg = false;
    bool versionFalg = false;
    bool headerParamKeyFlag = false;
    bool headerParamValFlag = false;
    bool headerParamKeyDoneFlag = false;
    bool colonFlag = false;

    char *p0 = const_cast<char *>(buf + _nextPos); // 去掉const限制

    while (_decodeState != HttpRequestDecodeState::INVALID 
            && _decodeState != HttpRequestDecodeState::INVALID_METHOD 
            && _decodeState != HttpRequestDecodeState::INVALID_URI 
            && _decodeState != HttpRequestDecodeState::INVALID_VERSION 
            && _decodeState != HttpRequestDecodeState::INVALID_HEADER 
            && _decodeState != HttpRequestDecodeState::COMPLETE 
            && _nextPos < size)
    {
        char ch = *p0;            // 当前字符
        char *p = p0++;           // 指针偏移
        int scanPos = _nextPos++; // 下一个指针往后移动

        switch (_decodeState)
        {
        case HttpRequestDecodeState::START:
            if (isInvalidCharacter(ch))
                ;
            else if (isupper(ch)) // 方法是大写字符
            {
                _decodeState = HttpRequestDecodeState::METHOD;
                method.begin = p;
            }
            else
            {
                _decodeState = HttpRequestDecodeState::INVALID_METHOD;
            }
            break;
        case HttpRequestDecodeState::METHOD:
            if (isupper(ch))
                ;
            else if (isblank(ch))
            { // 遇到空格解析结束
                method.end = p;
                _method = method;
                _decodeState = HttpRequestDecodeState::URL;
            }
            break;
        case HttpRequestDecodeState::URL:
            if (ch == '/')
            {
                if (!urlBeginFlag)
                {
                    url.begin = p;
                    urlBeginFlag = true;
                }
            }
            else if (isblank(ch))
            {
                if (urlBeginFlag)
                {
                    url.end = p;
                    _url = url;
                    _decodeState = HttpRequestDecodeState::PROTOCOL;
                }
            }
            else if (ch == '?')
            {
                if (urlBeginFlag)
                {
                    url.end = p;
                    _url = url;
                    _decodeState = HttpRequestDecodeState::URL_PARAM;
                }
            }
            break;
        case HttpRequestDecodeState::URL_PARAM:
            if (isInvalidCharacter(ch) && !urlParamKeyFlag && !urlParamValFlag)
                _decodeState = HttpRequestDecodeState::INVALID_URI;
            else if (ch == '=')
            {
                requestParamKey.end = p;
                urlParamKeyDoneFlag = true;
            }
            else if (isblank(ch))
            {
                requestParamValue.end = p;
                _requestParams.insert({requestParamKey, requestParamValue});
                _decodeState = HttpRequestDecodeState::PROTOCOL;
            }
            else if (ch == '&')
            {
                requestParamValue.end = p;
                _requestParams.insert({requestParamKey, requestParamValue});
                urlParamKeyFlag = false;
                urlParamValFlag = false;
            }
            else
            {
                if (!urlParamKeyFlag)
                {
                    requestParamKey.begin = p;
                    urlParamKeyFlag = true;
                }
                if (urlParamKeyDoneFlag && !urlParamValFlag)
                {
                    requestParamValue.begin = p;
                    urlParamValFlag = true;
                    urlParamKeyDoneFlag = false;
                }
            }
            break;
        case HttpRequestDecodeState::PROTOCOL:
            if (isblank(ch) && !protocolFalg)
                ;
            else if (ch == '/')
            {
                protocol.end = p;
                _protocol = protocol;
                protocolDoneFalg = true;
            }
            else if (protocolDoneFalg)
            {
                if (isdigit(ch) && !versionFalg)
                {
                    version.begin = p;
                    versionFalg = true;
                }
                else if (ch == CR)
                {
                    version.end = p;
                    _version = version;
                    _decodeState = HttpRequestDecodeState::REQUEST_LINE_END;
                }
                else if (!(isdigit(ch) || ch == '.' || CR) && versionFalg)
                {
                    _decodeState = HttpRequestDecodeState::INVALID_VERSION;
                }
            }
            else
            {
                if (!protocolFalg)
                {
                    protocol.begin = p;
                    protocolFalg = true;
                }
            }
            break;
        case HttpRequestDecodeState::REQUEST_LINE_END:
            if (ch == LF)
            {
                _decodeState = HttpRequestDecodeState::HEADER;
            }
            else
            {
                _decodeState = HttpRequestDecodeState::INVALID;
            }

        case HttpRequestDecodeState::HEADER:
            if (isInvalidCharacter(ch) && !headerParamKeyFlag)
                ;
            else if (ch == ':')
            {
                if (headerParamKeyFlag && !colonFlag)
                {
                    headerKey.end = p;
                    headerParamKeyDoneFlag = true;
                    colonFlag = true;
                }
            }
            else if (ch == '\r')
            {
                headerValue.end = p;
                _temp_header_key = headerKey;
                _temp_header_val = headerValue;
                rmUselessSpace(_temp_header_key);
                rmUselessSpace(_temp_header_val);
                _headers.insert({_temp_header_key, _temp_header_val});
                headerParamKeyFlag = false;
                headerParamValFlag = false;
                colonFlag = false;
                _decodeState = HttpRequestDecodeState::HEADER_LINE_END;
            }
            else
            {
                if (!headerParamKeyFlag)
                {
                    headerKey.begin = p;
                    headerParamKeyFlag = true;
                }
                if (headerParamKeyDoneFlag && !headerParamValFlag)
                {
                    headerValue.begin = p;
                    headerParamValFlag = true;
                    headerParamKeyDoneFlag = false;
                }
            }
            break;
        case HttpRequestDecodeState::HEADER_LINE_END:
            if (ch == LF)
            {
                _decodeState = HttpRequestDecodeState::HEADER;
                if (*(p + 1) == CR && *(p + 2) == LF) // 请求头结束
                {
                    if (_headers.count("Content-Length") > 0)
                    {
                        bodyLength = atoi(_headers["Content-Length"].c_str());
                        if (bodyLength > 0) // 存在body
                        {
                            _decodeState = HttpRequestDecodeState::BODY;
                        }
                        else
                        {
                            _decodeState = HttpRequestDecodeState::COMPLETE;
                        }
                    }
                }
            }
            else
            {
                _decodeState = HttpRequestDecodeState::INVALID;
            }
            break;
        case HttpRequestDecodeState::BODY:
            _body.assign(p, bodyLength);
            _body.erase(0, 2); // 删除CR LF
            _decodeState = HttpRequestDecodeState::COMPLETE;
            break;
        default:
            break;
        }
    }
    if(_decodeState == HttpRequestDecodeState::COMPLETE)
        return true;
    return false;
}

const std::string &HttpRequest::getMethod() const
{
    return _method;
}

const std::string &HttpRequest::getUrl() const
{
    return _url;
}

const std::map<std::string, std::string> &HttpRequest::getRequestParams() const
{
    return _requestParams;
}

const std::string &HttpRequest::getProtocol() const
{
    return _protocol;
}

const std::string &HttpRequest::getVersion() const
{
    return _version;
}

const std::map<std::string, std::string> &HttpRequest::getHeaders() const
{
    return _headers;
}

const std::string &HttpRequest::getBody() const
{
    return _body;
}

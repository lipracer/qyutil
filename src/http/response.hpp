#ifndef __RESPONSE_HPP__
#define __RESPONSE_HPP__

#include <string>
#include <map>
#include <iostream>
#include <vector>
#include "SocketStream.hpp"

namespace qyutil
{
using namespace std;

extern void split_string(const string& str, const string target, vector<string>& result);

class response_exception : std::exception
{
public:
    response_exception(const char * msg, int code = 0) : m_msg(msg), m_code(code)
    {
        
    }
    const char * what() noexcept
    {
        cerr << "error msg:" << m_msg << "\n" << "error code:" << m_code << endl;
        return m_msg.c_str();
    }
private:
    string m_msg;
    int m_code;
};

inline string lstrip(const string& str)
{
    size_t pos = 0;
    while (string::npos != str.find(' ', pos))
    {
        ++pos;
    }
    return str.substr(pos);
}

inline string rstrip(const string& str)
{
    size_t pos = string::npos;
    while (string::npos != (pos = str.rfind(' ', pos)))
    {
        --pos;
    }
    return str.substr(0, pos);
}

inline string strip(const string& str)
{
    string result = lstrip(str);
    return rstrip(result);
}

template<typename Stream>
class response : BaseResponse
{
public:
    response(string& body);
    int status_code()
    {
        return m_status_code;
    }
    int parse_header()
    {
        vector<string> headers;
        split_string(m_header, headers);
        for(auto & it : headers)
        {
            if(string::npos !=  it.find(':'))
            {
                auto pair = parse_http_line(it);
                if(pair.first.size() && pair.second.size())
                {
                    m_resp_header.insert(parse_http_line(it));
                }
            }
        }
    }
    pair<string, string> parse_http_line(string& line)
    {
        auto result = make_pair(string(), string());
        if(line.size())
        {
            size_t pos = line.find(':');
            if(pos != string::npos)
            {
                string key = line.substr(0, pos); 
                key = strip(key);     
                
                if(pos + 1 > line.size())
                {
                    throw response_exception("data format error");
                }
                string value = line.substr(pos + 1, line.size() - pos);
                value = strip(value);
                result.first = key;
                result.second = value;
            }
        }
    }
private:
    int m_status_code;
    map<string, string> m_resp_header;
    string m_response_boday;
};

    
} // namespace qyutil

#endif

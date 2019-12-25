#include "response.hpp"
#include "global_func.h"
#include "net_common/net_common.h"

#include <string>
#include <vector>

namespace qyutil
{

extern void split_string(const string& str, const string target, vector<string>& result);

response::response(string& boday) : m_response_boday(boday)
{
    vector<string> result;
    split_string(m_response_boday, HTTP_HEADER_EOF, result);
    
    if(result.size())
    {
        vector<string> code_info;
        auto first_line = result.begin();
        split_string(*first_line, string(" "), code_info);
        if(code_info.size() >= 2)
        {
            m_status_code = atoi(code_info[1].c_str());
        }
    }    
}

}

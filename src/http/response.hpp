#ifndef __RESPONSE_HPP__
#define __RESPONSE_HPP__

#include <string>
#include <map>

namespace qyutil
{
using namespace std;
class response
{
public:
    response(string& body);
    int status_code()
    {
        return m_status_code;
    }
private:
    int m_status_code;
    map<string, string> m_resp_header;
    string m_response_boday;
};

    
} // namespace qyutil

#endif

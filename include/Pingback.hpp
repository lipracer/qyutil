#ifndef __PINGBACK_H__
#define __PINGBACK_H__

#include <list>
#include <string>
#include <utility>

namespace qyutil
{
using namespace std;

using ss_pair = pair<string, string>;
using pb_params = list<ss_pair>;

class Pingback
{
public:
    
    static Pingback& shared()
    {
        static Pingback __instacnce;
        return __instacnce;
    }
    
    void init_public_params(list<ss_pair>& params)
    {
        std::copy(params.begin(), params.end(), public_params.begin());
    }
    
    void send(pb_params& pb_params)
    {
        
    }

private:
    string m_url;
    list<ss_pair> public_params;
};
}
#endif

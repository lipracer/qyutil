#include "qylog.h"
#include "qyutil.h"

extern "C" {

void OC_NetworkDiagnosis(const char* host, const char* dnsSer)
{
    function<int(void)> fun_1 = std::bind(QyUtil::NetworkDiagnosis, string(host), string("10.16.169.127"), 10);
    decltype(fun_1) fun_2 = nullptr;
    auto task = make_pair(fun_1, fun_2);
    QyUtil::qyutil<1>::getInstance().put_task(task);
}
    
}

#include "net_common.h"
#include <stdarg.h>

#include "qylog.h"

void __DefauleOutput(const char* result)
{
#if __ANDROID__
    LOGE("%s", result);
#else
    fprintf(stdout, "%s", result); 
#endif
}

result_output::result_output()
{
    _buf = new char[4096];
}

result_output::~result_output()
{
    delete [] _buf;
    _qyerro(_ss.str());
}

void result_output::operator()(const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int n = vprintf(fmt, ap);
    vsprintf(_buf, fmt, ap);
    va_end(ap);
    _ss << _buf;
}

const char* result_output::result()
{
    return _ss.str().c_str();
}

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

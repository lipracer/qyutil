//
//  main.cpp
//  TestQyutil
//
//  Created by 陈龙 on 2019/6/26.
//  Copyright © 2019 陈龙. All rights reserved.
//

#include <iostream>
#include "../src/dnsquery/dnsquery.h"
#include "../src/tracerouter/tracerouter.hpp"
#include "../include/qyutil.h"

#include "../src/tracerouter/tracerouter.hpp"

void TestDNSQuery(const char* host, const char* dnsSer);

int main(int argc, const char * argv[])
{
#if 0
    function<void(void)> func_1 = std::bind(TestDNSQuery, "pizza.iqiyi.com", "10.110.95.204");
    function<void(void)> func_2 = nullptr;
    for (int i = 0; i < 10; ++i)
    {
        cout << "-------------------------------------------------------put task" << endl;
        QyUtil::qyutil::getInstance().put_task(make_pair(func_1, func_2));
        
    }
    this_thread::sleep_for(chrono::milliseconds(10000000000));
#endif
    TraceRouter<NetCommon::TraceRouterType::UDP> tRouter(string("115.239.210.27"));

    return 0;
}

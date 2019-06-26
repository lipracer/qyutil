//
//  main.cpp
//  TestQyutil
//
//  Created by 陈龙 on 2019/6/26.
//  Copyright © 2019 陈龙. All rights reserved.
//

#include <iostream>
#include "../src/dnsquery/dnsquery.h"

extern void TestDNSQuery(char* host, char* dnsSer);

int main(int argc, const char * argv[])
{
    TestDNSQuery("pizza.iqiyi.com", "10.110.95.204");
    return 0;
}

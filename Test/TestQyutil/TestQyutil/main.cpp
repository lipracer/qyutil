//
//  main.cpp
//  TestQyutil
//
//  Created by 陈龙 on 2019/6/26.
//  Copyright © 2019 陈龙. All rights reserved.
//

/*
 // 创建一个命令行解析器
 cmdline::parser a;
 // 加入指定类型的输入參数
 // 第一个參数：长名称
 // 第二个參数：短名称（‘\0‘表示没有短名称）
 // 第三个參数：參数描写叙述
 // 第四个參数：bool值，表示该參数是否必须存在（可选。默认值是false）
 // 第五个參数：參数的默认值（可选，当第四个參数为false时该參数有效）
 a.add<string>("host", 'h', "host name", true, "");
 // 第六个參数用来对參数加入额外的限制
 // 这里端口号被限制为必须是1到65535区间的值，通过cmdline::range(1, 65535)进行限制
 a.add<int>("port", 'p', "port number", false, 80, cmdline::range(1, 65535));
 // cmdline::oneof() 能够用来限制參数的可选值
 a.add<string>("type", 't', "protocol type", false, "http", cmdline::oneof<string>("http", "https", "ssh", "ftp"));
 // 也能够定义bool值
 // 通过调用不带类型的add方法
 a.add("gzip", '\0', "gzip when transfer");
 // 执行解析器
 // 仅仅有当全部的參数都有效时他才会返回
 //  假设有无效參数，解析器会输出错误消息。然后退出程序
 // 假设有‘--help‘或-?
 //这种帮助标识被指定，解析器输出帮助信息。然后退出程序
 
 a.parse_check(argc, argv);
 // 获取输入的參数值
 cout << a.get<string>("type") << "://" << a.get<string>("host") << ":" << a.get<int>("port") << endl;
 // bool值能够通过调用exsit()方法来推断
 if (a.exist("gzip")) cout << "gzip" << endl;
 */

#include <iostream>
#include <string>
#include <map>
#include "qylog.h"
#include "../src/dnsquery/dnsquery.h"
#include "../src/tracerouter/tracerouter.hpp"
#include "../include/qyutil.h"
#include "cmdline.h"
#include <future>
#include "../src/http/request.hpp"
#include "Pingback.hpp"
#include "global_func.h"
#include "Semaphore.hpp"

using namespace std;
using namespace qyutil;


int main(int argc, char * argv[])
{
    //inq_list_test();
    struct PingbackParam pb;
    memset(&pb, 0, sizeof(pb));
    init_qyutil("", "http://msg.qy.net/v5/alt/act", &pb);
    
    map<string, string> params;
    try
    {

        Request request;
        request.get("http://www.baidu.com", params, 1000LL);
        
    }
    catch (std::exception &e)
    {
        _qyerro(e.what());
    }

    return 0;
}

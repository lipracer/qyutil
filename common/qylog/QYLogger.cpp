#include "QYLogger.h"
#include <iomanip>
#include <ctime>
#include <iostream>
#include <chrono>
#include <sstream>

#ifdef __ANDROID__
#include <android/log.h>
#endif

using namespace std;

QYCStr LogLevelInfo[] =
{
    "UNKNOWN",
    "DEFAULT",
    "VERBOSE",
    "DEBUG",
    "INFO",
    "WARN",
    "ERROR",
    "FATAL",
    "SILENT"
    
};

time_t GetTimePoint()
{
    chrono::system_clock::time_point now = chrono::system_clock::now();
    return chrono::system_clock::to_time_t(now);
}

thread::id GetThreadId()
{
    return this_thread::get_id();
}

string format_msg(QYCStr module, QYLogPriority level, QYCStr msg)
{
    stringstream ss;
    ss << "[m:" << module << "]" << "[ts:" << GetTimePoint() << "]" << "[th:" << this_thread::get_id() << "]" << "[" << LogLevelInfo[(int)level] << "]" << msg << endl;
    return ss.str();
}

QYBaseLogger::QYBaseLogger(QYLogPriority level):m_level(level) {}

QYBaseLogger::~QYBaseLogger() {}


int QYBaseLogger::Log(QYCStr module, QYLogPriority level, QYCStr msg)
{
    //cout << "QYBaseLogger:" << msg << endl;
    return 0;
}

QYLoggerConsole::QYLoggerConsole(QYLogPriority level) : QYBaseLogger(level)
{
}

QYLoggerConsole::~QYLoggerConsole() {}
int QYLoggerConsole::Log(QYCStr module, QYLogPriority level, QYCStr msg)
{
    
    if (level > m_level)
    {       
        chrono::system_clock::time_point now = chrono::system_clock::now();
        time_t now_c = chrono::system_clock::to_time_t(now);
        //auto cstime = localtime(&now_c);
        string tmp(std::move(format_msg(module, level, msg)));
        const char* msg__ = tmp.c_str();//直接取c_str此时返回的str已经释放
#ifdef __ANDROID__        
        //__android_log_print((int)QYLogPriority::ERROR, "QYUtil-Log", "level:%d m_level:%d %s",level, m_level, msg__);
        __android_log_print((int)level, module, "%s", msg__);
#else
        //fprintf(stdout, "%s", msg__);
        cout << msg__ << "\n";
#endif

//#ifdef _WINDOWS
//        fprintf(stdout, "%s", ss.str().c_str());
//#elif ANDROID
//        __android_log_print(level, "QYLOG-", "%s", ss.str().c_str());
//#elif _IOS
//        LOG_CONSOLE("%s", ss.str().c_str());
//#endif
        
    }
    
    return 0;
}



QYLoggerFile::QYLoggerFile(QYLogPriority level, QYCStr filepath) : QYBaseLogger(level)
{
    time_t timep;
    struct tm *p;
    time(&timep);
    p = gmtime(&timep);

    //printf("%d\n", p->tm_sec); /*ȡǰ*/
    //printf("%d\n", p->tm_min); /*ȡǰ*/
    //printf("%d\n", 8 + p->tm_hour);/*ȡǰʱ,ȡʱ,պ˸Сʱ*/
    //printf("%d\n", p->tm_mday);/*ȡǰ·,Χ1-31*/
    //printf("%d\n", 1 + p->tm_mon);/*ȡǰ·,Χ0-11,Ҫ1*/
    //printf("%d\n", 1900 + p->tm_year);/*ȡǰ,1900ʼҪ1900*/
    //printf("%d\n", p->tm_yday); /*ӽ11ΧΪ0-365*/
    stringstream ss;
    ss << filepath << "/" << 1900 + p->tm_year << "-" << \
        1 + p->tm_mon << "-" << \
        p->tm_mday << "-" << \
        8 + p->tm_hour << "-" << \
        p->tm_min << "-" << p->tm_sec;
    ss << ".log";

    m_fout.open(ss.str(), ios::out);
}

QYLoggerFile::~QYLoggerFile() { m_fout.close(); }
int QYLoggerFile::Log(QYCStr module, QYLogPriority level, QYCStr msg)
{
    if (level > m_level)
    {
        if (m_fout.is_open()) 
        {
            if (msg) 
            {
                m_fout << format_msg(module, level, msg) << endl;
            }
        }
    }

    return 0;
}

QYLoggerMix::QYLoggerMix(QYLogPriority level, QYCStr filename) : QYBaseLogger(level),
                                                                    m_console_log(new QYLoggerConsole(level)),
                                                                    m_file_log(new QYLoggerFile(level, filename))
{

}

QYLoggerMix::~QYLoggerMix()
{
    
}

int QYLoggerMix::Log(QYCStr module, QYLogPriority level, QYCStr msg)
{
    m_console_log->Log(module, level, msg);
    m_file_log->Log(module, level, msg);
    return 0;
}

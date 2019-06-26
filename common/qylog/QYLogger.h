#ifndef _QYUTIL_QYLOGGER_H_
#define _QYUTIL_QYLOGGER_H_

#include <fstream>
#include <thread>

enum class QYLogPriority : int
{
    UNKNOWN = 0,
    DEFAULT,
    VERBOSE,
    LDEBUG,
    INFO,
    WARN,
    ERROR,
    FATAL,
    SILENT
};

typedef const char* QYCStr;

using namespace std;
class QYBaseLogger 
{
public:
    QYBaseLogger(QYLogPriority level);
    virtual ~QYBaseLogger();
    virtual int Log(QYCStr module, QYLogPriority level, QYCStr msg);

    QYLogPriority m_level;
};

class QYLoggerConsole : public QYBaseLogger
{
public:
    QYLoggerConsole(QYLogPriority level);
    virtual ~QYLoggerConsole();
    virtual int Log(QYCStr module, QYLogPriority level, QYCStr msg);
};

class QYLoggerFile : public QYBaseLogger
{
public:
    QYLoggerFile(QYLogPriority level, QYCStr filename);
    virtual ~QYLoggerFile();
    virtual int Log(QYCStr module, QYLogPriority level, QYCStr msg);

private:
    fstream m_fout;
    mutex m_mtx;
};

class QYLoggerMix : public QYBaseLogger
{
public:
    QYLoggerMix(QYLogPriority level, QYCStr filename);
    virtual ~QYLoggerMix();
    virtual int Log(QYCStr module, QYLogPriority level, QYCStr msg);
private:
    //避免多重继承
    QYBaseLogger* m_console_log;
    QYBaseLogger* m_file_log;
};

#endif

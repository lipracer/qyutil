#ifndef _QYUTIL_QYLOGMGR_H_
#define _QYUTIL_QYLOGMGR_H_

#include <iomanip>
#include <ctime>
#include <iostream>
#include <chrono>
#include <sstream>

#include "QYLogger.h"

using namespace std;

enum class QYLogMode : int
{
    Console = 1,
    File    = 2,
    Mix     = 3,
    deft    = 4,

};

class QYLogMgr 
{

public:
    static QYLogMgr& getInstance() 
    {
        static QYLogMgr log_mgr;
        return log_mgr;        
    }
    int InitLogMgr(QYLogMode mode, QYCStr filename, QYLogPriority level);
    QYBaseLogger& GetLogger();

private:
    QYLogMgr();
    ~QYLogMgr();

    bool m_isInit;
    mutex m_init_mutex;

    QYLogMode m_log_mode;
    string m_filename;
    QYLogPriority m_log_level;
    QYBaseLogger* m_logger;
};

#ifdef   __cplusplus
extern "C" {
#endif

int QYInitLog(int mode, QYCStr filepath, int level);
void QYLog(QYCStr module, int level, QYCStr msg);
void QYLogCWrapper(int level, QYCStr module, QYCStr format, ...);

#ifdef   __cplusplus
}
#endif

#endif
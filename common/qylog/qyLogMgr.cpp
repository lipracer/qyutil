#include "qyLogMgr.h"
#include <stdarg.h>
    
int QYInitLog(QYLogMode mode, QYCStr filepath, QYLogPriority level)
{
    return QYLogMgr::getInstance().InitLogMgr((QYLogMode)mode, filepath, (QYLogPriority)level);
}

void QYLog(QYCStr module, QYLogPriority level, QYCStr msg)
{
    QYLogMgr::getInstance().GetLogger().Log(module, (QYLogPriority)level, msg);
}

void QYLogCWrapper(int level, QYCStr module, QYCStr format, ...)
{
    //TODO
    //unsafe replace nsprintf
    char buf[4096] = { 0 };
    va_list ap;     
    va_start(ap, format);
    vsprintf(buf, format, ap);
    va_end(ap);
    QYLog(module, (QYLogPriority)(level), buf);
}

QYLogMgr::QYLogMgr() : m_isInit(false),m_log_mode(QYLogMode::deft)
{
    m_logger = new QYLoggerConsole(QYLogPriority::INFO);
}

QYLogMgr::~QYLogMgr() 
{
    delete m_logger;
}

int QYLogMgr::InitLogMgr(QYLogMode mode, QYCStr filepath, QYLogPriority level)
{
    if (!m_isInit)
    {
        lock_guard<mutex> _(m_init_mutex);
        if (!m_isInit)
        {            
            m_log_mode = mode;
            //m_filename = filepath;
            m_log_level = level;
            m_isInit = true;
            switch(mode)
            {
                case QYLogMode::Console:
                    m_logger = new QYLoggerConsole(level);
                    break;
                case QYLogMode::File:
                    m_logger = new QYLoggerFile(level, filepath);
                    break;
                case QYLogMode::Mix:
                    m_logger = new QYLoggerMix(level, filepath);
                    break;
                default:
                    m_logger = new QYLoggerConsole(level);
                    break;
            }
            
        }
    }
    return 0;
}

QYBaseLogger& QYLogMgr::GetLogger()
{
    return *m_logger;
}

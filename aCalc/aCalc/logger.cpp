#include "logger.h"
#include <QDebug>

Logger::Logger()
{
    //qDebug() << filename;
    QDir logdir(dirname);
    if(!logdir.exists())
    {
        isError = !logdir.mkdir(dirname);
        if(isError) return;
    }

    logfile.setFileName(filename);
    isError = !logfile.open(QIODevice::Append);
    if(isError) return;
}

Logger::~Logger()
{
    if(logfile.isOpen())
        logfile.close();
}

void Logger::Add(const QString &value)
{
    if(isError) return;
    last_logs.push_back(value);

    QTextStream ts(&logfile);
    ts << "[" << QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss") << "] " << value << endl;
    //qDebug() << value;
}

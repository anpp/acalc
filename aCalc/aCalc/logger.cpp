#include "logger.h"
#include <QDebug>

Logger::Logger(const QString& dirname, const QString& filename)
{
    //qDebug() << filename;    
    this->filename = dirname + "/" + filename;

    QDir logdir(dirname);
    if(!logdir.exists())
    {
        isError = !logdir.mkdir(dirname);
        if(isError) return;
    }

    logfile.setFileName(this->filename);
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
    QString value_to_log = "[" + QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss") + "] " + value;
    last_logs.push_back(value);

    QTextStream ts(&logfile);
    ts << value_to_log << endl;
    //qDebug() << value;
}

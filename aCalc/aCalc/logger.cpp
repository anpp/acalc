#include "logger.h"
#include <QDebug>

Logger::Logger(const QString& dirname, const QString& filename, QIODevice::OpenMode flags)
{
    //qDebug() << filename;    
    this->filename = dirname + "/" + filename;

    QDir logdir(dirname);
    if(!logdir.exists())
    {
        isError = !logdir.mkdir(dirname);
        if(isError) return;
    }
    this->flags = flags;
    logfile.setFileName(this->filename);
    isError = !logfile.open(this->flags);
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

const QStringList &Logger::ReadLast(int rate)
{
  QStringList *sl = new QStringList();
  sl->append("1");
  sl->append("2");
  return *sl;
}

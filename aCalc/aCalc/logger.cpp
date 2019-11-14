#include "logger.h"
#include <QDebug>

Logger::Logger(const QString& dirname, const QString& filename, bool for_read)
{
    //qDebug() << filename;
    this->filename = dirname + "/" + filename;

    QDir logdir(dirname);
    if(!logdir.exists())
    {
        isError = !logdir.mkdir(dirname);
        if(isError) return;
    }
    flags = (for_read? QIODevice::ReadOnly: QIODevice::Append);
    logfile.setFileName(this->filename);
    isError = !logfile.open(flags);
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
    QStringList file_strings;

    if(isError)
        isError = !logfile.open(flags);

    if(!isError)
    {
        logfile.seek(0);
        while(!logfile.atEnd())
            file_strings << logfile.readLine();

        auto count_string = file_strings.count();
        auto fact_rate = count_string < rate? count_string: rate;

        for(auto i = 0; i < fact_rate; ++i)
            sl->append(file_strings[file_strings.count() - i - 1]);
    }
    return *sl;
}

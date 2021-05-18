#include "loger.h"
#include <QDebug>

Loger::Loger(const QString& dirname, const QString& filename)
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

Loger::~Loger()
{
    if(logfile.isOpen())
        logfile.close();
}

void Loger::Add(const QString &value)
{
    if(isError) return;
    QString value_to_log = "[" + QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss") + "] " + value;
    last_logs.push_back(value_to_log);

    QTextStream ts(&logfile);
    ts << value_to_log << Qt::endl;
    //qDebug() << value;
}

std::unique_ptr<QStringList> Loger::ReadLast(int rate)
{
    std::unique_ptr<QStringList> sl = std::make_unique<QStringList>();
    QStringList file_strings;
    QFile file;
    QString item;

    file.setFileName(filename);
    if(file.open(QIODevice::ReadOnly))
    {
        file.seek(0);
        while(!file.atEnd())
            file_strings << file.readLine();

        auto count_string = file_strings.count();
        auto fact_rate = count_string < rate? count_string: rate;

        for(auto i = 0; i < fact_rate; ++i)
        {
            item = file_strings[file_strings.count() - i - 1];
            if(item.size() > 0)
            {
                if((item[item.size() - 1] == '\n') || (item[item.size() - 1] == '\r') || (item[item.size() - 1] == '\t'))
                    item = item.left(item.size() - 1);
            }
            if(item.contains("]"))
                sl->append(item);
        }
    }
    file.close();
    last_pos = last_logs.size();
    return sl;
}

std::unique_ptr<QStringList> Loger::ReadLast()
{
    std::unique_ptr<QStringList> sl = std::make_unique<QStringList>();

    for(auto i = last_pos; i < last_logs.size(); ++i)
        sl->append(last_logs[i]);

    last_pos = last_logs.size();
    return sl;
}

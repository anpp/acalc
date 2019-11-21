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
    last_logs.push_back(value);

    QTextStream ts(&logfile);
    ts << value_to_log << endl;
    //qDebug() << value;
}

const QStringList &Loger::ReadLast(int rate)
{
    QStringList *sl = new QStringList();
    QStringList file_strings;
    QFile file;

    file.setFileName(filename);
    if(file.open(QIODevice::ReadOnly))
    {
        file.seek(0);
        while(!file.atEnd())
            file_strings << file.readLine();

        auto count_string = file_strings.count();
        auto fact_rate = count_string < rate? count_string: rate;

        for(auto i = 0; i < fact_rate; ++i)
            sl->append(file_strings[file_strings.count() - i - 1]);
    }
    file.close();
    return *sl;
}
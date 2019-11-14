#ifndef LOGGER_H
#define LOGGER_H

#include <QApplication>
#include <QTextStream>
#include <QDateTime>
#include <QFile>
#include <QDir>
#include "common.h"

class Logger
{
    QFile logfile;
    std::vector<QString> last_logs;

    QString filename;

    bool isError = false;
    QIODevice::OpenMode flags;
public:
    Logger(const QString& dirname, const QString& filename, bool for_read = false);
    ~Logger();
    void Add(const QString& value);
    const QStringList& ReadLast(int rate);

};

#endif // LOGGER_H

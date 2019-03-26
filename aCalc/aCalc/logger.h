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
public:
    Logger(const QString& dirname, const QString& filename);
    ~Logger();
    void Add(const QString& value);

};

#endif // LOGGER_H

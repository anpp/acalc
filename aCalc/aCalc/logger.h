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

    QString dirname = QDir::homePath() +  "/.acalc";
    QString filename = dirname + "/acalc.log";

    bool isError = false;
public:
    Logger();
    ~Logger();
    void Add(const QString& value);

};

#endif // LOGGER_H

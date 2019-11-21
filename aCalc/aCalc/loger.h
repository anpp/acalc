#ifndef LOGGER_H
#define LOGGER_H

#include <QApplication>
#include <QTextStream>
#include <QDateTime>
#include <QFile>
#include <QDir>
#include "common.h"

class Loger
{
    QFile logfile;
    std::vector<QString> last_logs;

    QString filename;

    bool isError = false;
public:
    Loger(const QString& dirname, const QString& filename);
    ~Loger();
    void Add(const QString& value);
    const QStringList& ReadLast(int rate);

};

#endif // LOGGER_H

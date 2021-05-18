#ifndef LOGGER_H
#define LOGGER_H

#include <QApplication>
#include <QTextStream>
#include <QDateTime>
#include <QFile>
#include <QDir>
#include <memory>
#include "common.h"

class Loger
{
    QFile logfile;
    std::vector<QString> last_logs;

    QString filename;
    unsigned long last_pos{0};

    bool isError = false;
public:
    Loger(const QString& dirname, const QString& filename);
    ~Loger();
    void Add(const QString& value);
    std::unique_ptr<QStringList> ReadLast(int rate);
    std::unique_ptr<QStringList> ReadLast();

};

#endif // LOGGER_H

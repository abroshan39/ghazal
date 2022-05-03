/*
    [Ghazal: The library of persian poetry]
    Publisher: Rosybit
    Url: http://www.rosybit.com
    GitHub: https://github.com/abroshan39/ghazal
    Version: 1.4
    Author: Aboutaleb Roshan [ab.roshan39@gmail.com]
    License: MIT License
*/

#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include "common_functions.h"

class Worker : public QObject
{
    Q_OBJECT

public:
    enum WorkerType
    {
        Importer,
        ImporterZip,
        Exporter,
        Remover,
        Vacuumer,
        Searcher
    };

public:
    Worker(const WorkerType &type, AppSettings *appSettings, const QString &filePath, bool removePreVersion, int speed);
    Worker(const WorkerType &type, AppSettings *appSettings, const QString &databasePath, const QStringList &poetIDs, int speed = 1000);
    Worker(const WorkerType &type, AppSettings *appSettings, const QStringList &poetIDs);
    Worker(const WorkerType &type, AppSettings *appSettings);
    Worker(const WorkerType &type, AppSettings *appSettings, QWidget *widget, const QString &searchQuery);

signals:
    void finished(WorkerType, QVariant);

public slots:
    void process();

private:
    WorkerType type;
    AppSettings *appSettings;
    QWidget *widget = nullptr;
    QString searchQuery;
    QString filePath;
    bool removePreVersion;
    int speed;
    QStringList poetIDs;
};
#endif // WORKER_H

/*
    [Ghazal: The library of persian poetry]
    Publisher: Rosybit
    Url: http://www.rosybit.com
    GitHub: https://github.com/abroshan39/ghazal
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
        Exporter,
        Remover,
        Vacuumer,
        Searcher
    };

public:
    Worker(const WorkerType &type, AppSettings *appSettings, const QStringList &filePathsList, bool removePreVersion, int speed);  // Importer Worker
    Worker(const WorkerType &type, AppSettings *appSettings, const QString &databasePath, const QStringList &poetIDs, int speed);  // Exporter Worker
    Worker(const WorkerType &type, AppSettings *appSettings, const QStringList &poetIDs);  // Remover Worker
    Worker(const WorkerType &type, AppSettings *appSettings);  // Vacuumer Worker
    Worker(const WorkerType &type, AppSettings *appSettings, QStandardItemModel *model, const QString &searchQuery);  // Searcher Worker

signals:
    void finished(WorkerType, QVariant);

public slots:
    void process();

private:
    WorkerType type;
    AppSettings *appSettings;
    QStandardItemModel *model;
    QString searchQuery;
    QString filePath;
    QStringList filePathsList;
    bool removePreVersion;
    int speed;
    QStringList poetIDs;
};
#endif // WORKER_H

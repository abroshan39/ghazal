/*
    [Ghazal: The library of persian poetry]
    Publisher: Rosybit
    Url: http://www.rosybit.com
    GitHub: https://github.com/abroshan39/ghazal
    Version: 1.4
    Author: Aboutaleb Roshan [ab.roshan39@gmail.com]
    License: MIT License
*/

#include "worker.h"
#include <JlCompress.h>

Worker::Worker(const Worker::WorkerType &type, AppSettings *appSettings, const QString &filePath, bool removePreVersion, int speed)
{
    // Importer Worker
    // ImporterZip Worker

    this->type = type;
    this->appSettings = appSettings;
    this->filePath = filePath;
    this->removePreVersion = removePreVersion;
    this->speed = speed;
}

Worker::Worker(const Worker::WorkerType &type, AppSettings *appSettings, const QString &databasePath, const QStringList &poetIDs, int speed)
{
    // Exporter Worker

    this->type = type;
    this->appSettings = appSettings;
    this->filePath = databasePath;
    this->poetIDs = poetIDs;
    this->speed = speed;
}

Worker::Worker(const Worker::WorkerType &type, AppSettings *appSettings, const QStringList &poetIDs)
{
    // Remover Worker

    this->type = type;
    this->appSettings = appSettings;
    this->poetIDs = poetIDs;
}

Worker::Worker(const Worker::WorkerType &type, AppSettings *appSettings)
{
    // Vacuumer Worker

    this->type = type;
    this->appSettings = appSettings;
}

Worker::Worker(const Worker::WorkerType &type, AppSettings *appSettings, QWidget *widget, const QString &searchQuery)
{
    // Searcher Worker

    this->type = type;
    this->appSettings = appSettings;
    this->widget = widget;
    this->searchQuery = searchQuery;
}

void Worker::process()
{
    QVariant result;

    qRegisterMetaType<QVector<int> >("QVector<int>");
    qRegisterMetaType<Qt::Orientation>("Qt::Orientation");
    qRegisterMetaType<WorkerType>("WorkerType");

    if(type == Importer)
    {
        if(isStdGanjoorDB(filePath))
            importDatabase(appSettings->mainDB, filePath, removePreVersion, speed);
        else
            qDebug().noquote() << "Cannot open the input file as a database file!";
    }
    else if(type == ImporterZip)
    {
        QDir qDir(QDir::tempPath());
        QString unzipDirName = Constants::Rosybit.toLower() + "-" + randString();

        if(qDir.mkdir(unzipDirName))
        {
            QStringList list = JlCompress::getFileList(filePath);
            QStringList dbList;

            for(int i = 0; i < list.count(); i++)
                if(dbExtCheck(list[i]))
                    dbList << list[i];

            dbList = JlCompress::extractFiles(filePath, dbList, qDir.path() + "/" + unzipDirName);

            for(int i = 0; i < dbList.count(); i++)
                if(isStdGanjoorDB(dbList[i]))
                    importDatabase(appSettings->mainDB, dbList[i], removePreVersion, speed);

            removeTempDir(unzipDirName);
        }
    }
    else if(type == Exporter)
    {
        SqliteDB secondDB(filePath, "secondDatabase", false);
        exportDatabase(appSettings->mainDB, secondDB.DB(), poetIDs, speed);
        secondDB.remove();
    }
    else if(type == Remover)
    {
        removePoet(appSettings->mainDB, poetIDs);
    }
    else if(type == Vacuumer)
    {
        QSqlQuery query(appSettings->mainDB);
        result = query.exec("VACUUM");
    }
    else if(type == Searcher)
    {
        result = searchTableWidget(appSettings, static_cast<QTableWidget *>(widget), searchQuery);
    }

    emit finished(type, result);
}

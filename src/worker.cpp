/*
    [Ghazal: The library of persian poetry]
    Publisher: Rosybit
    Url: http://www.rosybit.com
    GitHub: https://github.com/abroshan39/ghazal
    Author: Aboutaleb Roshan [ab.roshan39@gmail.com]
    License: MIT License
*/

#include "worker.h"
#include <JlCompress.h>

Worker::Worker(const Worker::WorkerType &type, AppSettings *appSettings, const QStringList &filePathsList, bool removePreVersion, int speed)
{
    // Importer Worker

    this->type = type;
    this->appSettings = appSettings;
    this->filePathsList = filePathsList;
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

Worker::Worker(const Worker::WorkerType &type, AppSettings *appSettings, QStandardItemModel *model, const QString &searchQuery)
{
    // Searcher Worker

    this->type = type;
    this->appSettings = appSettings;
    this->model = model;
    this->searchQuery = searchQuery;
}

void Worker::process()
{
    QVariant result;

    qRegisterMetaType<QVector<int> >("QVector<int>");
    qRegisterMetaType<Qt::Orientation>("Qt::Orientation");
    qRegisterMetaType<WorkerType>("WorkerType");
    qRegisterMetaType<QList<QPersistentModelIndex> >("QList<QPersistentModelIndex>");
    qRegisterMetaType<QAbstractItemModel::LayoutChangeHint>("QAbstractItemModel::LayoutChangeHint");

    if(type == Importer)
    {
        for(int i = 0; i < filePathsList.count(); i++)
        {
            filePath = filePathsList[i];
            if(filePath.endsWith(".zip", Qt::CaseInsensitive))
            {
                QDir tempDir(QDir::tempPath());
                QString unzipDirName = Constants::Rosybit.toLower() + "-" + randString();

                if(tempDir.mkdir(unzipDirName))
                {
                    QStringList list = JlCompress::getFileList(filePath);
                    QStringList dbList;

                    for(int j = 0; j < list.count(); j++)
                        if(dbExtCheck(list[j]))
                            dbList << list[j];

                    dbList = JlCompress::extractFiles(filePath, dbList, tempDir.path() + "/" + unzipDirName);

                    for(int j = 0; j < dbList.count(); j++)
                    {
                        if(isStdGanjoorDB(dbList[j]))
                            importDatabase(appSettings->mainDB, dbList[j], removePreVersion, speed);
                        else
                            qDebug().noquote() << QString("Cannot open the input file as a database file! [%1]").arg(dbList[j]);
                    }
                    removeTempDir(unzipDirName);
                }
            }
            else
            {
                if(isStdGanjoorDB(filePath))
                    importDatabase(appSettings->mainDB, filePath, removePreVersion, speed);
                else
                    qDebug().noquote() << QString("Cannot open the input file as a database file! [%1]").arg(filePath);
            }
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
        if(searchQuery == Constants::MARKER_RADIF)
            searchRadifTableView(model, appSettings->mainDB, appSettings->inSearchSettings, appSettings->searchSettings.allItemsSelected, appSettings->searchSettings.poetIDList, appSettings->searchSettings.searchPhrase, appSettings->searchSettings.skipDiacritics, appSettings->searchSettings.skipCharTypes);
        else if(searchQuery == Constants::MARKER_GHAFIE)
            searchGhafieTableView(model, appSettings->mainDB, appSettings->inSearchSettings, appSettings->searchSettings.allItemsSelected, appSettings->searchSettings.poetIDList, appSettings->searchSettings.searchPhrase, appSettings->searchSettings.skipDiacritics, appSettings->searchSettings.skipCharTypes);
        else
            result = searchTableView(model, appSettings->mainDB, appSettings->inSearchSettings, appSettings->searchSettings.method, appSettings->searchSettings.allItemsSelected, appSettings->searchSettings.poetIDList, searchQuery, appSettings->searchSettings.searchPhrase, appSettings->searchSettings.table, appSettings->searchSettings.skipDiacritics, appSettings->searchSettings.skipCharTypes);
    }

    emit finished(type, result);
}

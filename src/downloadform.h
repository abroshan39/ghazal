/*
    [Ghazal: The library of persian poetry]
    Publisher: Rosybit
    Url: http://www.rosybit.com
    GitHub: https://github.com/abroshan39/ghazal
    Author: Aboutaleb Roshan [ab.roshan39@gmail.com]
    License: MIT License
*/

#ifndef DOWNLOADFORM_H
#define DOWNLOADFORM_H

#include <QMainWindow>
#include <QUrl>
#include <QDir>
#include <QFile>
#include <QTableWidgetItem>
#include "common_functions.h"
#include "filedownloader.h"

namespace Ui {
class DownloadForm;
}

class DownloadForm : public QMainWindow
{
    Q_OBJECT

public:
    enum DownloadType  // If you want to change this enum, do it carefully, because the comboBoxSave items is connected to this enum.
    {
        ImportToMainDB,
        ExportToNewDB,
        OnlyDownloadFiles
    };

    enum DownloadFlag
    {
        None,
        IsDownloading,
        Canceled,
        Finished,
        Error
    };

public:
    explicit DownloadForm(QWidget *parent = nullptr);
    DownloadForm(AppSettings *appSettings, QWidget *parent = nullptr);
    ~DownloadForm();

signals:
    void sigMainDBChanged();
    void sigUpdatePoetList();

public slots:
    void startup();
    void slotStartDownload();
    void slotCancel();
    void slotRedirect(const QUrl &newUrl);
    void slotProgress(const QString &fileName, qint64 total, qint64 received, const QString &sSpeed, int leftHour, int leftMin, int leftSec);
    void slotFinished(const QString &downloadedFilePath, const QString &originalFileName, const QString &renamedFileName, const QString &userFileName);
    void slotErorr(const QString &error);
    void slotTableWidgetItemChanged(QTableWidgetItem *item);
    void startDownload();
    void startDownloadAbort();
    void downloadLoop();
    void downloadLoopEnd();
    void refreshForm();
    void writeToDB(const QString &databasePath, int speed = 1000);
    bool check_DB_DirPath();
    void clearLabels();

private slots:
    void on_btnClose_clicked();
    void on_btnSearchOnGanjoor_clicked();
    void on_btnDownload_clicked();
    void on_btnCancel_clicked();
    void on_selectAllCheckBox_clicked(bool checked);
    void on_notInstalledCheckBox_clicked(bool checked);
    void on_preInstalledCheckBox_clicked(bool checked);
    void on_comboBox_currentIndexChanged(int index);
    void on_selectNoneCheckBox_clicked(bool checked);
    void on_comboBoxSave_currentIndexChanged(int index);
    void on_tableWidget_doubleClicked(const QModelIndex &index);

protected:
    void keyPressEvent(QKeyEvent *e) override;
    void closeEvent(QCloseEvent *e) override;

private:
    Ui::DownloadForm *ui;
    AppSettings *appSettings;
    SqliteDB exportDB;
    bool cancelClose = false;
    DownloadType downloadType;  // If you want to change this enum, do it carefully. Because the comboBoxSave items is connected to this enum.
    QString exportToNewDBPath;
    QString onlyDownloadFilesDirPath;
    FileDownloader *fileDownloader = nullptr;
    bool isXml;
    QList<XmlPoet> xmlPoetList;
    QDir tempDir;
    QString xmlDirName;
    QString dlDirName;
    QString dlFileName;

    DownloadFlag downloadFlag = None;
    QMap<QString, QStringList> selectedPoets;
    int allSelectedCount = 0;
    QList<int> preInstalled, notInstalled;
    bool commonPoetsChecked = false;
    QStringList commonPoets;
    bool removePreVer = false;
    long long int totalSize = 0;
    int downloadedDB = 0;
    long long int downloadedSize = 0;
};

#endif // DOWNLOADFORM_H

/*
                     File Downloader (Qt C++ class):
                          by Aboutaleb Roshan
             7 Tir, 1396 (3 Shawwal, 1438) (28 June, 2017)
                                EDITED:
         29 Tir, 1401 (20 Dhu al-Hijjah, 1443) (20 July, 2022)
                         ab.roshan39@gmail.com
*/

#ifndef FILEDOWNLOADER_H
#define FILEDOWNLOADER_H

#include <QObject>
#include <cmath>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrl>
#include <QFileInfo>
#include <QFile>
#include <QDir>
#include <QElapsedTimer>
#include <QRegularExpression>
#include <QUuid>
#include <QDebug>

class FileDownloader : public QObject
{
    Q_OBJECT

public:
    explicit FileDownloader(QObject *parent = nullptr);
    virtual ~FileDownloader();

    void download(const QUrl &url, const QString &path, bool overwriteIfFileExists = false, const QString &userFileName = QString());
    void cancel();

    static QString byteToHuman(qint64 size);
    static QString speedToHuman(double bytesPerMillisecond);
    static QString randString(int len = 16);

signals:
    void sigStartDownload();
    void sigCancel();
    void sigRedirect(const QUrl &newUrl);
    void sigProgress(const QString &fileName, qint64 total, qint64 received, const QString &sSpeed, int leftHour, int leftMin, int leftSec);
    void sigFinished(const QString &downloadedFilePath, const QString &originalFileName, const QString &renamedFileName, const QString &userFileName);
    void sigErorr(const QString &error);

private slots:
    void streamReceived();
    void updateProgress(qint64 bytesReceived, qint64 bytesTotal);
    void downloadFinished();
    void networkSslErrors(const QList<QSslError> &errors);
    void networkError(QNetworkReply::NetworkError error);

private:
    void preStartDownload(const QUrl &url);
    void startDownload(const QUrl &url);
    void redirectTo(const QUrl &newUrl);
    void clearVariables();

private:
    QNetworkAccessManager *manager = nullptr;
    QNetworkReply *reply = nullptr;
    QUrl url;
    QString path;
    QString originalFileName;
    QString renamedFileName;
    QString userFileName;
    QString fileName;
    QStringList fileNameList;
    QFile *file = nullptr;
    QElapsedTimer eTimer;
    bool overwriteIfFileExists = false;
    bool canceled = false;
    bool errorOccurred = false;
    QString strError;
};

#endif // FILEDOWNLOADER_H

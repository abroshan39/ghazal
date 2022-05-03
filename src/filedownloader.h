/*
                     File Downloader (Qt C++ class):
                          by Aboutaleb Roshan
             7 Tir, 1396 (3 Shawwal, 1438) (28 June, 2017)
                                EDITED:
         18 Mehr, 1400 (3 Rabi' al-Awwal, 1443) (10 October, 2021)
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
#include <QElapsedTimer>
#include <QDebug>

class FileDownloader : public QObject
{
    Q_OBJECT

public:
    explicit FileDownloader(QObject *parent = nullptr);
    virtual ~FileDownloader();

    bool download(const QString &strUrl, const QString &path);
    void cancel();

    static QString byteToHuman(qint64 size);
    static QString speedToHuman(double bytesPerMillisecond);

signals:
    void sigStartDownload();
    void sigCancel();
    void sigProgress(const QString &fileName, qint64 total, qint64 received, const QString &sSpeed, int leftHour, int leftMin, int leftSec);
    void sigFinished();
    void sigErorr(const QString &error);

private slots:
    void streamReceived();
    void updateProgress(qint64 bytesReceived, qint64 bytesTotal);
    void downloadFinished();
    void networkSslErrors(const QList<QSslError> &errors);
    void networkError(QNetworkReply::NetworkError error);

private:
    void startDownload(QUrl url);

private:
    QNetworkAccessManager *manager = nullptr;
    QNetworkReply *reply = nullptr;
    QUrl url;
    QFileInfo fileInfo;
    QFile *file = nullptr;
    QElapsedTimer eTimer;
    bool canceled = false;
    bool errorOccurred = false;
    QString strError;
};

#endif // FILEDOWNLOADER_H

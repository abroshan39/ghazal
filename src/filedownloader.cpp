/*
                     File Downloader (Qt C++ class):
                          by Aboutaleb Roshan
             7 Tir, 1396 (3 Shawwal, 1438) (28 June, 2017)
                                EDITED:
         18 Mehr, 1400 (3 Rabi' al-Awwal, 1443) (10 October, 2021)
                         ab.roshan39@gmail.com
*/

#include "filedownloader.h"

FileDownloader::FileDownloader(QObject *parent) :
    QObject(parent)
{
    manager = new QNetworkAccessManager(this);
}

FileDownloader::~FileDownloader()
{
    cancel();
}

bool FileDownloader::download(const QString &strUrl, const QString &path)
{
    url = QUrl(strUrl);
    fileInfo.setFile(url.path());
    QString fileName(fileInfo.fileName());

    if(fileName.isEmpty())
    {
        QString error("Download Status: Failed! Please check the address!");
        qDebug().noquote() << error;
        emit sigErorr(error);
        return false;
    }
    else
    {
        file = new QFile(path + "/" + fileName);
        if(!file->open((QIODevice::WriteOnly)))
        {
            delete file;
            file = nullptr;
            return false;
        }

        qDebug().noquote() << QString("Download Status: Started [%1]").arg(strUrl);
        emit sigStartDownload();

        startDownload(url);
    }
    return true;
}

void FileDownloader::cancel()
{
    canceled = true;
    if(reply)
        reply->abort();
}

QString FileDownloader::byteToHuman(qint64 size)
{
    int base = 1024;

    if(size < base)
        return QString::number(size) + " Bytes";
    else if(size < pow(base, 2))
        return QString::number((float)size / base, 'f', 2) + " KB";  // Kilobyte
    else if(size < pow(base, 3))
        return QString::number((float)size / pow(base, 2), 'f', 2) + " MB";  // Megabyte
    else if(size < pow(base, 4))
        return QString::number((float)size / pow(base, 3), 'f', 2) + " GB";  // Gigabyte
    else if(size < pow(base, 5))
        return QString::number((float)size / pow(base, 4), 'f', 2) + " TB";  // Terabyte
    else if(size < pow(base, 6))
        return QString::number((float)size / pow(base, 5), 'f', 2) + " PB";  // Petabyte
    else if(size < pow(base, 7))
        return QString::number((float)size / pow(base, 6), 'f', 2) + " EB";  // Exabyte
    else if(size < pow(base, 8))
        return QString::number((float)size / pow(base, 7), 'f', 2) + " ZB";  // Zettabyte

    return QString::number((float)size / pow(base, 8), 'f', 2) + " YB";  // Yottabyte
}

QString FileDownloader::speedToHuman(double bytesPerMillisecond)
{
    double bps = bytesPerMillisecond * 1000;
    int base = 1024;

    if(bps < base)
        return QString::number((int)bps) + " Bytes/sec";
    else if(bps < pow(base, 2))
        return QString::number((float)bps / base, 'f', 3) + " KB/sec";
    else if(bps < pow(base, 3))
        return QString::number((float)bps / pow(base, 2), 'f', 3) + " MB/sec";
    else if(bps < pow(base, 4))
        return QString::number((float)bps / pow(base, 3), 'f', 3) + " GB/sec";
    else if(bps < pow(base, 5))
        return QString::number((float)bps / pow(base, 4), 'f', 3) + " TB/sec";
    else if(bps < pow(base, 6))
        return QString::number((float)bps / pow(base, 5), 'f', 3) + " PB/sec";
    else if(bps < pow(base, 7))
        return QString::number((float)bps / pow(base, 6), 'f', 3) + " EB/sec";
    else if(bps < pow(base, 8))
        return QString::number((float)bps / pow(base, 7), 'f', 3) + " ZB/sec";

    return QString::number((float)bps / pow(base, 8), 'f', 3) + " YB/sec";
}

void FileDownloader::startDownload(QUrl url)
{
    canceled = false;
    errorOccurred = false;
    strError = "";
    reply = manager->get(QNetworkRequest(url));

    connect(reply, &QIODevice::readyRead, this, &FileDownloader::streamReceived);
    connect(reply, &QNetworkReply::downloadProgress, this, &FileDownloader::updateProgress);
    connect(reply, &QNetworkReply::finished, this, &FileDownloader::downloadFinished);
    connect(reply, &QNetworkReply::sslErrors, this, &FileDownloader::networkSslErrors);
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)  // #if QT_VERSION >= 0x050F00
    connect(reply, &QNetworkReply::errorOccurred, this, &FileDownloader::networkError);
#else
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(networkError(QNetworkReply::NetworkError)));
#endif

    // Ignored by networkSslErrors slot
    // reply->ignoreSslErrors();

    eTimer.start();
}

void FileDownloader::streamReceived()
{
    if(file)
        file->write(reply->readAll());
}

void FileDownloader::updateProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    if(!canceled && !errorOccurred)
    {
        QString fileName(fileInfo.fileName());
        qint64 useTime = eTimer.elapsed();

        double speed = bytesReceived / useTime;
        double leftTime = bytesTotal / speed / 1000 - useTime / 1000;

        int intLeftTime = (int)leftTime;
        int leftHour = intLeftTime / 3600;
        int leftMin = (intLeftTime % 3600) / 60;
        int leftSec = intLeftTime % 60;
        QString sSpeed = speedToHuman(speed);

        qDebug().noquote() << QString("%1 | File Size: %2 | Downloaded: %3 | Speed: %4 | Time Left: %5h %6m %7s")
            .arg(fileName)
            .arg(byteToHuman(bytesTotal))
            .arg(byteToHuman(bytesReceived))
            .arg(sSpeed)
            .arg(leftHour)
            .arg(leftMin)
            .arg(leftSec);
        emit sigProgress(fileName, bytesTotal, bytesReceived, sSpeed, leftHour, leftMin, leftSec);
    }
}

void FileDownloader::downloadFinished()
{
    if(file)
    {
        file->close();
        delete file;
        file = nullptr;
    }

    if(reply)
    {
        reply->deleteLater();
        reply = nullptr;
    }

    if(errorOccurred)
    {
        qDebug().noquote() << QString("Download Status: Error Occurred and Download Stopped [%1]").arg(strError);
        emit sigErorr(strError);
        return;
    }

    if(canceled)
    {
        qDebug().noquote() << "Download Status: Canceled";
        emit sigCancel();
        return;
    }

    // if(!errorOccurred)
    // {
    //     qDebug().noquote() << "Download Status: Finished";
    //     emit sigFinished();
    // }

    qDebug().noquote() << "Download Status: Finished";
    emit sigFinished();
}

void FileDownloader::networkSslErrors(const QList<QSslError> &errors)
{
    for(int i = 0; i < errors.count(); i++)
        qDebug().noquote() << errors[i].errorString();

    reply->ignoreSslErrors(errors);

    /*
    Note: Because most SSL errors are associated with a certificate, for
    most of them you must set the expected certificate this SSL error is
    related to (Like self-signed certificate).

    QList<QSslError> expectedSslErrors;
    expectedSslErrors.append(QSslError::SelfSignedCertificate);
    expectedSslErrors.append(QSslError::CertificateUntrusted);
    reply->ignoreSslErrors(expectedSslErrors);

    void QNetworkReply::ignoreSslErrors()
    void QNetworkReply::ignoreSslErrors(const QList<QSslError> &errors)
    Warning: Be sure to always let the user inspect the errors reported by
    the sslErrors() signal, and only call this method upon confirmation
    from the user that proceeding is ok. If there are unexpected errors, the
    reply should be aborted. Calling this method without inspecting the
    actual errors will most likely pose a security risk for your application.
    Use it with great care!
    */
}

void FileDownloader::networkError(QNetworkReply::NetworkError error)
{
    Q_UNUSED(error);  // (void)error;

    if(!canceled)
    {
        errorOccurred = true;
        strError = reply->errorString();
        // qDebug().noquote() << QString("Download Status: Error Occurred and Download Stopped [%1]").arg(strError);
        // emit sigErorr(strError);
    }
}

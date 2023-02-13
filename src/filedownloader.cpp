/*
                     File Downloader (Qt C++ class):
                          by Aboutaleb Roshan
             7 Tir, 1396 (3 Shawwal, 1438) (28 June, 2017)
                                EDITED:
         29 Tir, 1401 (20 Dhu al-Hijjah, 1443) (20 July, 2022)
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

void FileDownloader::download(const QUrl &url, const QString &path, bool overwriteIfFileExists, const QString &userFileName)
{
    this->url = url;
    this->path = path;
    this->userFileName = userFileName;
    this->overwriteIfFileExists = overwriteIfFileExists;
    preStartDownload(url);
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

QString FileDownloader::randString(int len)
{
    return QUuid::createUuid().toString().remove(QRegularExpression("[^A-Za-z0-9]")).left(len).toUpper();
}

void FileDownloader::preStartDownload(const QUrl &url)
{
    this->url = url;  // Assignment again, If redirection occurs.

    if(!userFileName.isEmpty())
    {
        fileName = userFileName;
    }
    else
    {
        fileName = QFileInfo(url.path()).fileName();
        fileNameList << fileName;
        if(fileNameList.count() > 1 && !fileName.contains("."))
        {
            for(int i = fileNameList.count() - 2; i >= 0; i--)
            {
                if(fileNameList[i].contains("."))
                    fileName = fileNameList[i];
            }
        }
    }

    if(fileName.isEmpty())
        fileName = "index.html";

    QString filePath = path + "/" + fileName;
    originalFileName = fileName;
    if(QFileInfo(filePath).isFile())
    {
        if(!overwriteIfFileExists)
        {
            fileName = randString(24) + "-" + fileName;
            renamedFileName = fileName;
        }
    }

    file = new QFile(path + "/" + fileName);
    if(!file->open((QIODevice::WriteOnly)))
    {
        delete file;
        file = nullptr;

        clearVariables();

        QString error("Download Status: Error [Failed to open file!]");
        qDebug().noquote() << error;
        emit sigErorr(error);
        return;
    }

    qDebug().noquote() << QString("Download Status: Started [%1]").arg(url.toString());
    emit sigStartDownload();

    startDownload(url);
}

void FileDownloader::startDownload(const QUrl &url)
{
    canceled = false;
    errorOccurred = false;
    strError.clear();
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

void FileDownloader::redirectTo(const QUrl &newUrl)
{
    disconnect(reply, &QIODevice::readyRead, this, &FileDownloader::streamReceived);
    disconnect(reply, &QNetworkReply::downloadProgress, this, &FileDownloader::updateProgress);
    disconnect(reply, &QNetworkReply::finished, this, &FileDownloader::downloadFinished);
    disconnect(reply, &QNetworkReply::sslErrors, this, &FileDownloader::networkSslErrors);
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)  // #if QT_VERSION >= 0x050F00
    disconnect(reply, &QNetworkReply::errorOccurred, this, &FileDownloader::networkError);
#else
    disconnect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(networkError(QNetworkReply::NetworkError)));
#endif

    if(reply)
    {
        reply->deleteLater();
        reply = nullptr;
    }

    file->remove();
    preStartDownload(newUrl);
}

void FileDownloader::clearVariables()
{
    path.clear();
    originalFileName.clear();
    renamedFileName.clear();
    userFileName.clear();
    fileName.clear();
    fileNameList.clear();
    overwriteIfFileExists = false;
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
        file->flush();
        file->close();
    }

    QVariant redirect = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    if(!redirect.isNull())
    {
        QUrl newUrl = url.resolved(redirect.toUrl());
        qDebug().noquote() << QString("Download Status: Redirect to \"%1\"").arg(newUrl.toString());
        emit sigRedirect(newUrl);
        redirectTo(newUrl);
        return;
    }

    QString downloadedFilePath(QFileInfo(path + "/" + fileName).absoluteFilePath());
    QString oFileName(originalFileName);
    QString rFileName(renamedFileName);
    QString uFileName(userFileName);

    clearVariables();

    if(file)
    {
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

    qDebug().noquote() << QString("%1\nDownload Status: Finished").arg(QDir::toNativeSeparators(downloadedFilePath));
    emit sigFinished(downloadedFilePath, oFileName, rFileName, uFileName);
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

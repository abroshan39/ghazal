/*
    [Ghazal: The library of persian poetry]
    Publisher: Rosybit
    Url: http://www.rosybit.com
    GitHub: https://github.com/abroshan39/ghazal
    Author: Aboutaleb Roshan [ab.roshan39@gmail.com]
    License: MIT License
*/

#ifndef UPDATECHECKFORM_H
#define UPDATECHECKFORM_H

#include <QMainWindow>
#include "common_functions.h"
#include "filedownloader.h"
#include <QUrl>
#include <QLabel>
#include <QTextBrowser>

namespace Ui {
class UpdateCheckForm;
}

class UpdateCheckForm : public QMainWindow
{
    Q_OBJECT

public:
    struct VersionDetails
    {
        QString text;
        QString dirAttr;
        QString alignAttr;
        QString fontFamilyAttr;
        QString fontSizeAttr;
        QString fontWeightAttr;
        QString fontStyleAttr;
        QString colorAttr;
        QString marginAttr;
        QString marginTopAttr;
        QString marginRightAttr;
        QString marginBottomAttr;
        QString marginLeftAttr;
        QString classAttr;
    };

public:
    explicit UpdateCheckForm(QWidget *parent = nullptr);
    UpdateCheckForm(AppSettings *appSettings, QWidget *parent = nullptr);
    ~UpdateCheckForm();

public slots:
    void startup();
    void check(const QString &xmlUrl = Constants::GhazalUpdateXmlUrl);
    QString itemToHtml(const QString &url, const QString &details, const QString &size, const QString &md5, const QString &pgp);
    QString itemToHtmlAllVersions(const QString &url, const QString &ver, const QString &ver_name);
    QString itemsTitleHtml(const QString &title, const QStringList &items);
    QString htmlContent(const QString &bodyContent);
    QString findBestMatchedDistribution(const QStringList &os_linux, const QStringList &os_windows, const QStringList &os_macos);
    void setHtmlLabel(QLabel *label, const QString &html);
    void setHtmlTextBrowser(QTextBrowser *textBrowser, const QString &html);
    void showDetails();
    void showCurrentVersionPage();
    void slotFinished(const QString &downloadedFilePath, const QString &originalFileName, const QString &renamedFileName, const QString &userFileName);
    void slotErorr(const QString &error);

private slots:
    void on_labelCheck_linkActivated(const QString &link);
    void on_labelCurrentDist_linkActivated(const QString &link);
    void on_textBrowser_anchorClicked(const QUrl &arg1);

protected:
    void keyPressEvent(QKeyEvent *e) override;
    void closeEvent(QCloseEvent *e) override;

private:
    Ui::UpdateCheckForm *ui;
    AppSettings *appSettings;
    FileDownloader *fileDownloader = nullptr;
    QDir tempDir;
    bool isDownloading = false;
    QString xmlUrlFile;
    QString xmlDirName;
    QString contentLabel;
    QString contentTextBrowser;
    QList<VersionDetails> versionDetails;

    const QString detailsLink = "<br /><a href=\"details\">جزئیات</a>";
};

#endif // UPDATECHECKFORM_H

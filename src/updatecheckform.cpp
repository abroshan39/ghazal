/*
    [Ghazal: The library of persian poetry]
    Publisher: Rosybit
    Url: http://www.rosybit.com
    GitHub: https://github.com/abroshan39/ghazal
    Author: Aboutaleb Roshan [ab.roshan39@gmail.com]
    License: MIT License
*/

#include "updatecheckform.h"
#include "ui_updatecheckform.h"

#include <QFileInfo>
#include <QFile>
#include <QRegularExpression>

UpdateCheckForm::UpdateCheckForm(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::UpdateCheckForm)
{
    ui->setupUi(this);
}

UpdateCheckForm::UpdateCheckForm(AppSettings *appSettings, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::UpdateCheckForm)
{
    ui->setupUi(this);

    this->appSettings = appSettings;

    resize((int)(705 * appSettings->screenRatio), (int)(390 * appSettings->screenRatio));
    setGeometry(QStyle::alignedRect(Qt::RightToLeft, Qt::AlignCenter, size(), QGuiApplication::primaryScreen()->availableGeometry()));
    setWindowTitle("بررسی بروزرسانی");
    setWindowIcon(QIcon(":/files/images/ghazal-256x256.png"));
    setWindowModality(Qt::WindowModal);

    fileDownloader = new FileDownloader;
    connect(fileDownloader, &FileDownloader::sigFinished, this, &UpdateCheckForm::slotFinished);
    connect(fileDownloader, &FileDownloader::sigErorr, this, &UpdateCheckForm::slotErorr);

    startup();
    check();
}

UpdateCheckForm::~UpdateCheckForm()
{
    delete ui;
}

void UpdateCheckForm::keyPressEvent(QKeyEvent *e)
{
    if(e->key() == Qt::Key_Escape)
        close();
}

void UpdateCheckForm::closeEvent(QCloseEvent *e)
{
    if(isDownloading)
    {
        fileDownloader->cancel();
        removeTempDir(xmlDirName);
        isDownloading = false;
    }

    QWidget::closeEvent(e);
}

void UpdateCheckForm::startup()
{
    tempDir.setPath(QDir::tempPath());
    ui->textBrowser->setOpenLinks(false);
    ui->textBrowser->setOpenExternalLinks(false);
}

void UpdateCheckForm::check(const QString &xmlUrl)
{
    isDownloading = true;
    contentLabel.clear();
    contentTextBrowser.clear();
    versionDetails.clear();
    ui->labelCheck->clear();
    ui->labelCurrentDist->clear();
    ui->textBrowser->clear();
    ui->labelCurrentDist->hide();
    xmlUrlFile = xmlUrl;
    xmlDirName = Constants::Rosybit.toLower() + "-" + randString();
    if(tempDir.mkdir(xmlDirName))
        fileDownloader->download(QUrl(xmlUrlFile), tempDir.path() + "/" + xmlDirName);
}

void UpdateCheckForm::slotFinished(const QString &downloadedFilePath, const QString &originalFileName, const QString &renamedFileName, const QString &userFileName)
{
    Q_UNUSED(originalFileName);  // (void)originalFileName;
    Q_UNUSED(renamedFileName);   // (void)renamedFileName;
    Q_UNUSED(userFileName);      // (void)userFileName;

    QDomDocument document;
    QFile file(downloadedFilePath);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    document.setContent(&file);
    file.close();
    removeTempDir(xmlDirName);
    isDownloading = false;
    QDomElement root = document.documentElement();

    if(!root.tagName().contains(Constants::Ghazal, Qt::CaseInsensitive))
    {
        contentLabel = QString("<p dir=\"rtl\" align=\"right\">خطایی رخ داده است. (<a href=\"%1\">بررسی مجدد</a>) (<a href=\"%2\">همهٔ نسخه‌ها</a>)</p>").arg(Constants::GhazalUpdateXmlUrl, Constants::GhazalAllVersionsXmlUrl);
        setHtmlLabel(ui->labelCheck, htmlContent(contentLabel));
        return;
    }

    if(xmlUrlFile == Constants::GhazalAllVersionsXmlUrl)
    {
        QStringList url;
        QStringList ver;
        QStringList ver_name;

        QDomElement eAllVersions = root.firstChildElement("AllVersions");
        QDomNodeList nXmlUrl = eAllVersions.elementsByTagName("XmlUrl");
        for(int i = 0; i < nXmlUrl.count(); i++)
        {
            QDomElement element = nXmlUrl.at(i).toElement();
            url.append(element.text());
            ver.append(element.attribute("Ver"));
            ver_name.append(element.attribute("VerName"));
            contentTextBrowser += itemToHtmlAllVersions(url[i], ver[i], ver_name[i]) + "\n";
        }
        contentTextBrowser = contentTextBrowser.trimmed();

        contentLabel = QString("<p dir=\"rtl\" align=\"right\"><a href=\"%1\">بررسی نسخهٔ جدید</a> | <a href=\"%2\">همهٔ نسخه‌ها</a></p>").arg(Constants::GhazalUpdateXmlUrl, Constants::GhazalAllVersionsXmlUrl);
        setHtmlLabel(ui->labelCheck, htmlContent(contentLabel));
        setHtmlTextBrowser(ui->textBrowser, htmlContent(contentTextBrowser));
    }
    else
    {
        QStringList src;
        QStringList os_linux;
        QStringList os_windows;
        QStringList os_macos;
        QStringList database;
        QStringList total;

        QString ver = root.firstChildElement("Version").toElement().text();
        QString ver_name = root.firstChildElement("VersionName").toElement().text();
        QString date = root.firstChildElement("ReleaseDate").toElement().text();

        QString srcTagName("Source");
        QString srcUrl = root.firstChildElement(srcTagName).firstChildElement("Url").toElement().text();
        QString srcSize = byteToHuman(root.firstChildElement(srcTagName).firstChildElement("Size").toElement().text().toInt(), false);
        QString srcMD5 = root.firstChildElement(srcTagName).firstChildElement("MD5").toElement().text();
        QString srcPGP = root.firstChildElement(srcTagName).firstChildElement("PGPSigUrl").toElement().text();
        src.append(itemToHtml(srcUrl, srcTagName, srcSize, srcMD5, srcPGP));

        QDomElement eLinux = root.firstChildElement("Distribution").firstChildElement("Linux");
        QDomNodeList nListLinux = eLinux.elementsByTagName("Item");
        for(int i = 0; i < nListLinux.count(); i++)
        {
            QDomNode node = nListLinux.at(i);
            QString url = node.namedItem("Url").toElement().text();
            QString details = node.namedItem("Details").toElement().text();
            QString size = byteToHuman(node.namedItem("Size").toElement().text().toInt(), false);
            QString md5 = node.namedItem("MD5").toElement().text();
            QString pgp = node.namedItem("PGPSigUrl").toElement().text();
            os_linux.append(itemToHtml(url, details, size, md5, pgp));
        }

        QDomElement eWindows = root.firstChildElement("Distribution").firstChildElement("Windows");
        QDomNodeList nListWindows = eWindows.elementsByTagName("Item");
        for(int i = 0; i < nListWindows.count(); i++)
        {
            QDomNode node = nListWindows.at(i);
            QString url = node.namedItem("Url").toElement().text();
            QString details = node.namedItem("Details").toElement().text();
            QString size = byteToHuman(node.namedItem("Size").toElement().text().toInt(), false);
            QString md5 = node.namedItem("MD5").toElement().text();
            QString pgp = node.namedItem("PGPSigUrl").toElement().text();
            os_windows.append(itemToHtml(url, details, size, md5, pgp));
        }

        QDomElement eMacOS = root.firstChildElement("Distribution").firstChildElement("macOS");
        QDomNodeList nListMacOS = eMacOS.elementsByTagName("Item");
        for(int i = 0; i < nListMacOS.count(); i++)
        {
            QDomNode node = nListMacOS.at(i);
            QString url = node.namedItem("Url").toElement().text();
            QString details = node.namedItem("Details").toElement().text();
            QString size = byteToHuman(node.namedItem("Size").toElement().text().toInt(), false);
            QString md5 = node.namedItem("MD5").toElement().text();
            QString pgp = node.namedItem("PGPSigUrl").toElement().text();
            os_macos.append(itemToHtml(url, details, size, md5, pgp));
        }

        QString dbTagName("Database");
        QString dbUrl = root.firstChildElement(dbTagName).firstChildElement("Url").toElement().text();
        QString dbSize = byteToHuman(root.firstChildElement(dbTagName).firstChildElement("Size").toElement().text().toInt(), false);
        QString dbMD5 = root.firstChildElement(dbTagName).firstChildElement("MD5").toElement().text();
        QString dbPGP = root.firstChildElement(dbTagName).firstChildElement("PGPSigUrl").toElement().text();
        database.append(itemToHtml(dbUrl, dbTagName, dbSize, dbMD5, dbPGP));

        QDomElement eDetails = root.firstChildElement("Details");
        QDomNodeList nDetails = eDetails.elementsByTagName("Line");
        for(int i = 0; i < nDetails.count(); i++)
        {
            QDomElement element = nDetails.at(i).toElement();
            VersionDetails dLine;
            dLine.text = element.text();
            dLine.dirAttr = element.attribute("dir");
            dLine.alignAttr = element.attribute("align");
            dLine.fontFamilyAttr = element.attribute("fontFamily");
            dLine.fontSizeAttr = element.attribute("fontSize");
            dLine.fontWeightAttr = element.attribute("fontWeight");
            dLine.fontStyleAttr = element.attribute("fontStyle");
            dLine.colorAttr = element.attribute("color");
            dLine.marginAttr = element.attribute("margin");
            dLine.marginTopAttr = element.attribute("marginTop");
            dLine.marginRightAttr = element.attribute("marginRight");
            dLine.marginBottomAttr = element.attribute("marginBottom");
            dLine.marginLeftAttr = element.attribute("marginLeft");
            dLine.classAttr = element.attribute("class");
            versionDetails.append(dLine);
        }

        src.removeAll("");
        os_linux.removeAll("");
        os_windows.removeAll("");
        os_macos.removeAll("");
        database.removeAll("");

        total << itemsTitleHtml("Source", src) << itemsTitleHtml("Linux", os_linux) << itemsTitleHtml("Windows", os_windows) << itemsTitleHtml("macOS", os_macos) << itemsTitleHtml("Database", database);
        total.removeAll("");

        contentTextBrowser = total.join("\n");
        setHtmlTextBrowser(ui->textBrowser, htmlContent(contentTextBrowser));

        QString verNameHtml = ver_name.isEmpty() ? "" : QString("<br />نام نسخه: %1").arg(ver_name);
        QString dateHtml = date.isEmpty() ? "" : QString("<br />تاریخ انتشار: %1").arg(date);
        QString dLink = versionDetails.isEmpty() ? "" : detailsLink;

        if(xmlUrlFile == Constants::GhazalUpdateXmlUrl)
        {
            QString verLatest(ver);
            QString verCurrent(Constants::GhazalVersion);
            if(versionCompare(verLatest, verCurrent) == 0)
            {
                contentLabel = QString("<p dir=\"rtl\" align=\"right\">%1 بروز است. (<a href=\"%2\">بررسی مجدد</a>) (<a href=\"%3\">همهٔ نسخه‌ها</a>)<br />نسخه: %4%5%6%7</p>").arg(Constants::GhazalFa, Constants::GhazalUpdateXmlUrl, Constants::GhazalAllVersionsXmlUrl, persianNumber(ver), verNameHtml, persianNumber(dateHtml), dLink);
                setHtmlLabel(ui->labelCheck, htmlContent(contentLabel));
            }
            else if(versionCompare(verLatest, verCurrent) > 0)
            {
                contentLabel = QString("<p dir=\"rtl\" align=\"right\">نسخهٔ جدیدی از %1 منتشر شده است. (<a href=\"%2\">بررسی مجدد</a>) (<a href=\"%3\">همهٔ نسخه‌ها</a>)<br />نسخه: %4%5%6%7</p>").arg(Constants::GhazalFa, Constants::GhazalUpdateXmlUrl, Constants::GhazalAllVersionsXmlUrl, persianNumber(ver), verNameHtml, persianNumber(dateHtml), dLink);
                QString currentDist = findBestMatchedDistribution(os_linux, os_windows, os_macos);
                setHtmlLabel(ui->labelCheck, htmlContent(contentLabel));
                setHtmlLabel(ui->labelCurrentDist, htmlContent(currentDist));
                ui->labelCurrentDist->show();
            }
            else
            {
                contentLabel = QString("<p dir=\"rtl\" align=\"right\">خطایی رخ داده است. (<a href=\"%1\">بررسی مجدد</a>) (<a href=\"%2\">همهٔ نسخه‌ها</a>)</p>").arg(Constants::GhazalUpdateXmlUrl, Constants::GhazalAllVersionsXmlUrl);
                setHtmlLabel(ui->labelCheck, htmlContent(contentLabel));
                contentTextBrowser.clear();
                ui->textBrowser->clear();
            }
        }
        else
        {
            if(!ver.isEmpty())
            {
                contentLabel = QString("<p dir=\"rtl\" align=\"right\">%1 (<a href=\"%2\">بررسی نسخهٔ جدید</a>) (<a href=\"%3\">همهٔ نسخه‌ها</a>)<br />نسخه: %4%5%6%7</p>").arg(Constants::GhazalFa, Constants::GhazalUpdateXmlUrl, Constants::GhazalAllVersionsXmlUrl, persianNumber(ver), verNameHtml, persianNumber(dateHtml), dLink);
                setHtmlLabel(ui->labelCheck, htmlContent(contentLabel));
            }
            else
            {
                contentLabel = QString("<p dir=\"rtl\" align=\"right\">خطایی رخ داده است. (<a href=\"%1\">بررسی مجدد</a>) (<a href=\"%2\">همهٔ نسخه‌ها</a>)</p>").arg(Constants::GhazalUpdateXmlUrl, Constants::GhazalAllVersionsXmlUrl);
                setHtmlLabel(ui->labelCheck, htmlContent(contentLabel));
                contentTextBrowser.clear();
                ui->textBrowser->clear();
            }
        }
    }
}

QString UpdateCheckForm::findBestMatchedDistribution(const QStringList &os_linux, const QStringList &os_windows, const QStringList &os_macos)
{
    QString currentDist;
    QString architecture;

    if(isBuild64Bit())
        architecture = "x64";
    else
        architecture = "x86";

    if(QSysInfo::productType().toLower() == "windows")
    {
        if(!os_windows.isEmpty())
            currentDist = os_windows[0];
        for(int i = 0; i < os_windows.count(); i++)
        {
            if(os_windows[i].contains(architecture, Qt::CaseInsensitive) && os_windows[i].contains("installer", Qt::CaseInsensitive))
            {
                currentDist = os_windows[i];
                break;
            }
        }
    }
    else if(QSysInfo::productType().toLower() == "macos")
    {
        if(!os_macos.isEmpty())
            currentDist = os_macos[0];
        for(int i = 0; i < os_macos.count(); i++)
        {
            if(os_macos[i].contains(architecture, Qt::CaseInsensitive) && os_macos[i].contains("installer", Qt::CaseInsensitive))
            {
                currentDist = os_macos[i];
                break;
            }
        }
    }
    else
    {
        if(!os_linux.isEmpty())
            currentDist = os_linux[0];
        for(int i = 0; i < os_linux.count(); i++)
        {
            if(os_linux[i].contains(architecture, Qt::CaseInsensitive) && os_linux[i].contains("installer", Qt::CaseInsensitive))
            {
                currentDist = os_linux[i];
                break;
            }
        }
    }

    return currentDist;
}

void UpdateCheckForm::slotErorr(const QString &error)
{
    Q_UNUSED(error);  // (void)error;
    contentLabel = QString("<p dir=\"rtl\" align=\"right\">خطایی رخ داده است. (<a href=\"%1\">بررسی مجدد</a>) (<a href=\"%2\">همهٔ نسخه‌ها</a>)<br />ارتباط نرم‌افزار را با اینترنت بررسی کنید و مجدد تلاش کنید.</p>").arg(Constants::GhazalUpdateXmlUrl, Constants::GhazalAllVersionsXmlUrl);
    setHtmlLabel(ui->labelCheck, htmlContent(contentLabel));
    removeTempDir(xmlDirName);
    isDownloading = false;
}

QString UpdateCheckForm::htmlContent(const QString &bodyContent)
{
    QString html;
    QString textColor = appSettings->isDarkMode ? "white" : "black";
    QFile file(":/files/html/update_check.html");

    if(bodyContent.isEmpty())
        return QString();

    file.open(QIODevice::ReadOnly | QIODevice::Text);
    html = file.readAll();
    file.close();

    html.replace("${FontName}", appSettings->viewFN);
    html.replace("${FontSize}", QString::number(appSettings->viewFS.toDouble()));
    html.replace("${TextColor}", textColor);
    html.replace("${EnglishFontName}", "Open Sans");
    html.replace("${EnglishFontSize}", ratioFontSize(appSettings->viewFS.toDouble(), 0.95));
    html.replace("${MonoFontName}", "Source Code Pro");
    html.replace("${MonoFontSize}", ratioFontSize(appSettings->viewFS.toDouble(), 0.95));
    html.replace("${EnglishTitleFontName}", "Open Sans");
    html.replace("${EnglishTitleFontSize}", ratioFontSize(appSettings->viewFS.toDouble(), 1.25));
    html.replace("${BodyContent}", bodyContent);

    return html;
}

QString UpdateCheckForm::itemToHtml(const QString &url, const QString &details, const QString &size, const QString &md5, const QString &pgp)
{
    if(url.isEmpty() || size.isEmpty() || md5.isEmpty())
        return QString();
    QString str;
    str = QString("<p dir=\"ltr\" align=\"left\" class=\"mono\"><a class=\"mono\" href=\"%1\">%2</a>%3<br />").arg(url, QFileInfo(url).fileName(), (pgp.isEmpty() ? "" : QString(" [<a class=\"mono\" href=\"%1\">Sig</a>]").arg(pgp)));
    str += QString("Size: %1<br />MD5: %2<br /></p><!-- %3 -->").arg(size, md5, details);
    return str;
}

QString UpdateCheckForm::itemToHtmlAllVersions(const QString &url, const QString &ver, const QString &ver_name)
{
    if(url.isEmpty() || ver.isEmpty())
        return QString();
    return QString("<p dir=\"rtl\" align=\"right\"><a href=\"%1\">%2 نسخهٔ %3%4</a>%5</p>").arg(url, Constants::GhazalFa, persianNumber(ver), (ver_name.isEmpty() ? "" : QString(" (%1)").arg(ver_name)), (versionCompare(ver, Constants::GhazalVersion) == 0 ? " (نسخهٔ فعلی)" : ""));
}

QString UpdateCheckForm::itemsTitleHtml(const QString &title, const QStringList &items)
{
    if(items.isEmpty())
        return QString();
    return QString("<p dir=\"ltr\" align=\"left\" class=\"english_title\">%1:</p>\n").arg(title) + items.join("\n");
}

void UpdateCheckForm::setHtmlLabel(QLabel *label, const QString &html)
{
    if(html.isEmpty())
        label->clear();
    else
        label->setText(html);
}

void UpdateCheckForm::setHtmlTextBrowser(QTextBrowser *textBrowser, const QString &html)
{
    if(html.isEmpty())
        textBrowser->clear();
    else
        textBrowser->setHtml(html);
}

void UpdateCheckForm::showDetails()
{
    QString str;
    QString backLink = "<br /><a href=\"back\">بازگشت</a>";
    QRegularExpression regex(QString(QString("%1</p>$").arg(detailsLink)).replace("/", "\\/"), QRegularExpression::NoPatternOption);

    int emptyLine = 0;
    for(int i = 0; i < versionDetails.count(); i++)
    {
        if(versionDetails[i].text.isEmpty())
        {
            emptyLine++;
            continue;
        }

        QString dirAttr, alignAttr, classAttr, styleAttr;
        QString styleFontFamily, styleFontSize, styleFontWeight, styleFontStyle, styleColor, styleMargin, styleMarginTop, styleMarginRight, styleMarginBottom, styleMarginLeft;

        styleFontFamily = versionDetails[i].fontFamilyAttr.isEmpty() ? "" : QString("font-family:%1;").arg(versionDetails[i].fontFamilyAttr);
        styleFontSize = versionDetails[i].fontSizeAttr.isEmpty() ? "" : QString("font-size:%1pt;").arg(ratioFontSize(appSettings->viewFS.toDouble(), versionDetails[i].fontSizeAttr.toDouble()));
        styleFontWeight = versionDetails[i].fontWeightAttr.isEmpty() ? "" : QString("font-weight:%1;").arg(versionDetails[i].fontWeightAttr);
        styleFontStyle = versionDetails[i].fontStyleAttr.isEmpty() ? "" : QString("font-style:%1;").arg(versionDetails[i].fontStyleAttr);
        styleColor = versionDetails[i].colorAttr.isEmpty() ? "" : QString("color:%1;").arg(versionDetails[i].colorAttr);
        styleMargin = versionDetails[i].marginAttr.isEmpty() ? "" : QString("margin:%1;").arg(versionDetails[i].marginAttr);
        styleMarginTop = versionDetails[i].marginTopAttr.isEmpty() ? "" : QString("margin-top:%1;").arg(versionDetails[i].marginTopAttr);
        styleMarginRight = versionDetails[i].marginRightAttr.isEmpty() ? "" : QString("margin-right:%1;").arg(versionDetails[i].marginRightAttr);
        styleMarginBottom = versionDetails[i].marginBottomAttr.isEmpty() ? "" : QString("margin-bottom:%1;").arg(versionDetails[i].marginBottomAttr);
        styleMarginLeft = versionDetails[i].marginLeftAttr.isEmpty() ? "" : QString("margin-left:%1;").arg(versionDetails[i].marginLeftAttr);
        styleAttr = styleFontFamily + styleFontSize + styleFontWeight + styleFontStyle + styleColor + styleMargin + styleMarginTop + styleMarginRight + styleMarginBottom + styleMarginLeft;

        dirAttr = versionDetails[i].dirAttr.isEmpty() ? " dir=\"rtl\"" : QString(" dir=\"%1\"").arg(versionDetails[i].dirAttr);
        alignAttr = versionDetails[i].alignAttr.isEmpty() ? " align=\"right\"" : QString(" align=\"%1\"").arg(versionDetails[i].alignAttr);
        classAttr = versionDetails[i].classAttr.isEmpty() ? "" : QString(" class=\"%1\"").arg(versionDetails[i].classAttr);
        styleAttr = styleAttr.isEmpty() ? "" : QString(" style=\"%1\"").arg(styleAttr);

        str += QString("%1<p%2%3%4%5>%6%7</p>").arg((i ? "\n" : ""), dirAttr, alignAttr, classAttr, styleAttr, QString("<br />").repeated(emptyLine), versionDetails[i].text);
        emptyLine = 0;
    }
    if(emptyLine)
        str += QString("%1<p>%2</p>").arg((!str.isEmpty() ? "\n" : ""), QString("<br />").repeated(emptyLine - 1));

    setHtmlTextBrowser(ui->textBrowser, htmlContent(str));

    str = QString(contentLabel).replace(regex, backLink + "</p>");
    setHtmlLabel(ui->labelCheck, htmlContent(str));
}

void UpdateCheckForm::showCurrentVersionPage()
{
    setHtmlTextBrowser(ui->textBrowser, htmlContent(contentTextBrowser));
    setHtmlLabel(ui->labelCheck, htmlContent(contentLabel));
}

void UpdateCheckForm::on_labelCheck_linkActivated(const QString &link)
{
    if(link.toLower() == "details")
        showDetails();
    else if(link.toLower() == "back")
        showCurrentVersionPage();
    else
        check(link);
}

void UpdateCheckForm::on_labelCurrentDist_linkActivated(const QString &link)
{
    QDesktopServices::openUrl(QUrl(link));
}

void UpdateCheckForm::on_textBrowser_anchorClicked(const QUrl &arg1)
{
    if(xmlUrlFile == Constants::GhazalAllVersionsXmlUrl)
        check(arg1.toString());
    else
        QDesktopServices::openUrl(arg1);
}

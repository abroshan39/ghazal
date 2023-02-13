/*
    [Ghazal: The library of persian poetry]
    Publisher: Rosybit
    Url: http://www.rosybit.com
    GitHub: https://github.com/abroshan39/ghazal
    Author: Aboutaleb Roshan [ab.roshan39@gmail.com]
    License: MIT License
*/

#include "aboutform.h"
#include "ui_aboutform.h"

#include <QFile>

AboutForm::AboutForm(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::AboutForm)
{
    ui->setupUi(this);
}

AboutForm::AboutForm(AppSettings *appSettings, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::AboutForm)
{
    ui->setupUi(this);

    this->appSettings = appSettings;

    resize((int)(665 * appSettings->screenRatio), (int)(370 * appSettings->screenRatio));
    setGeometry(QStyle::alignedRect(Qt::RightToLeft, Qt::AlignCenter, size(), QGuiApplication::primaryScreen()->availableGeometry()));
    setWindowTitle("درباره برنامه");
    setWindowIcon(QIcon(":/files/images/ghazal-256x256.png"));
    setWindowModality(Qt::WindowModal);

    setHtml();
}

AboutForm::~AboutForm()
{
    delete ui;
}

void AboutForm::keyPressEvent(QKeyEvent *e)
{
    if(e->key() == Qt::Key_Escape)
        close();
}

void AboutForm::setHtml()
{
    QString html;
    QString textColor = appSettings->isDarkMode ? "white" : "black";
    QString verNameHtml = Constants::GhazalVersionName.isEmpty() ? "" : QString("\n<p dir=\"rtl\" align=\"right\">نام نسخه: %1</p>").arg(Constants::GhazalVersionName);
    QString persian_date;
    QDate date(QDate::fromString(QString(__DATE__).simplified(), "MMM d yyyy"));
    QFile file(":/files/html/about_ghazal.html");

    if(date.isValid())
        persian_date = gregorianToPersian(date.day(), date.month(), date.year());
    else
        persian_date = QString(__DATE__).simplified();

    file.open(QIODevice::ReadOnly | QIODevice::Text);
    html = file.readAll();
    file.close();

    html.replace("${FontName}", appSettings->viewFN);
    html.replace("${FontSize}", QString::number(appSettings->viewFS.toDouble()));
    html.replace("${TitleFontSize}", ratioFontSize(appSettings->viewFS.toDouble(), 1.5));
    html.replace("${TopicFontSize}", ratioFontSize(appSettings->viewFS.toDouble(), 1.25));
    html.replace("${TextColor}", textColor);
    html.replace("${Rosybit}", Constants::Rosybit);
    html.replace("${RosybitUrl}", Constants::RosybitUrl);
    html.replace("${Ghazal}", Constants::Ghazal);
    html.replace("${GhazalFa}", Constants::GhazalFa);
    html.replace("${GhazalVersion}", Constants::GhazalVersion);
    html.replace("${GhazalVersionName}", verNameHtml);
    html.replace("${GhazalBuildDate}", QString(__TIME__) + "  " + persian_date);
    html.replace("${GhazalUrl}", Constants::GhazalUrl);
    html.replace("${GhazalGitHub}", Constants::GhazalGitHub);
    html.replace("${Email}", Constants::Email);
    html.replace("${QtVersion}", QT_VERSION_STR);

    ui->textBrowser->setHtml(html);
}

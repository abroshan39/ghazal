/*
    [Ghazal: The library of persian poetry]
    Publisher: Rosybit
    Url: http://www.rosybit.com
    GitHub: https://github.com/abroshan39/ghazal
    Version: 1.4
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

    html.replace(QRegularExpression("\\$\\{FontName\\}"), appSettings->viewFN);
    html.replace(QRegularExpression("\\$\\{FontSize\\}"), QString::number(appSettings->viewFS.toDouble()));
    html.replace(QRegularExpression("\\$\\{TitleFontSize\\}"), ratioFontSize(appSettings->viewFS.toDouble(), 1.5));
    html.replace(QRegularExpression("\\$\\{TopicFontSize\\}"), ratioFontSize(appSettings->viewFS.toDouble(), 1.25));
    html.replace(QRegularExpression("\\$\\{TextColor\\}"), textColor);
    html.replace(QRegularExpression("\\$\\{Rosybit\\}"), Constants::Rosybit);
    html.replace(QRegularExpression("\\$\\{RosybitUrl\\}"), Constants::RosybitUrl);
    html.replace(QRegularExpression("\\$\\{AppName\\}"), Constants::AppName);
    html.replace(QRegularExpression("\\$\\{AppNameFa\\}"), Constants::AppNameFa);
    html.replace(QRegularExpression("\\$\\{AppVersion\\}"), Constants::AppVersion);
    html.replace(QRegularExpression("\\$\\{AppBuildDate\\}"), QString(__TIME__) + "  " + persian_date);
    html.replace(QRegularExpression("\\$\\{AppUrl\\}"), Constants::AppUrl);
    html.replace(QRegularExpression("\\$\\{GitHub\\}"), Constants::GitHub);
    html.replace(QRegularExpression("\\$\\{Email\\}"), Constants::Email);
    html.replace(QRegularExpression("\\$\\{QtVersion\\}"), QT_VERSION_STR);

    ui->textBrowser->setHtml(html);
}

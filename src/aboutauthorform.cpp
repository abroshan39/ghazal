/*
    [Ghazal: The library of persian poetry]
    Publisher: Rosybit
    Url: http://www.rosybit.com
    GitHub: https://github.com/abroshan39/ghazal
    Version: 1.4
    Author: Aboutaleb Roshan [ab.roshan39@gmail.com]
    License: MIT License
*/

#include "aboutauthorform.h"
#include "ui_aboutauthorform.h"

#include <QFile>

AboutAuthorForm::AboutAuthorForm(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::AboutAuthorForm)
{
    ui->setupUi(this);
}

AboutAuthorForm::AboutAuthorForm(AppSettings *appSettings, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::AboutAuthorForm)
{
    ui->setupUi(this);

    this->appSettings = appSettings;

    QString backgroundColor = appSettings->isDarkMode ? "background-color:rgb(35,35,35);" : "background-color:rgb(255,255,255);";
    QString hafez = appSettings->isDarkMode ? "image:url(:/files/images/hafez-white.svg);" : "image:url(:/files/images/hafez.svg);";
    resize(1000, 580);
    setGeometry(QStyle::alignedRect(Qt::RightToLeft, Qt::AlignCenter, size(), QGuiApplication::primaryScreen()->availableGeometry()));
    setWindowTitle("درباره نویسنده");
    setWindowIcon(QIcon(":/files/images/ghazal-256x256.png"));
    setWindowModality(Qt::WindowModal);
    setStyleSheet(backgroundColor);
    ui->label->setStyleSheet(hafez);
    ui->label->setFixedSize(ui->label->maximumWidth(), size().height() / 3);

    setHtml();
}

AboutAuthorForm::~AboutAuthorForm()
{
    delete ui;
}

void AboutAuthorForm::keyPressEvent(QKeyEvent *e)
{
    if(e->key() == Qt::Key_Escape)
        close();
}

void AboutAuthorForm::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);  // (void)event;
    ui->label->setFixedSize(ui->label->maximumWidth(), size().height() / 3);
}

void AboutAuthorForm::setHtml()
{
    QString html;
    QString textColor = appSettings->isDarkMode ? "white" : "black";
    QFile file(":/files/html/about_author.html");

    file.open(QIODevice::ReadOnly | QIODevice::Text);
    html = file.readAll();
    file.close();

    html.replace(QRegularExpression("\\$\\{FontName\\}"), appSettings->viewFN);
    html.replace(QRegularExpression("\\$\\{FontSize\\}"), QString::number(appSettings->viewFS.toDouble()));
    html.replace(QRegularExpression("\\$\\{TextColor\\}"), textColor);
    html.replace(QRegularExpression("\\$\\{AppNameFa\\}"), Constants::AppNameFa);

    ui->textBrowser->setHtml(html);
}

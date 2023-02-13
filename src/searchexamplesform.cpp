/*
    [Ghazal: The library of persian poetry]
    Publisher: Rosybit
    Url: http://www.rosybit.com
    GitHub: https://github.com/abroshan39/ghazal
    Author: Aboutaleb Roshan [ab.roshan39@gmail.com]
    License: MIT License
*/

#include "searchexamplesform.h"
#include "ui_searchexamplesform.h"

#include <QFile>

SearchExamplesForm::SearchExamplesForm(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SearchExamplesForm)
{
    ui->setupUi(this);
}

SearchExamplesForm::SearchExamplesForm(AppSettings *appSettings, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SearchExamplesForm)
{
    ui->setupUi(this);

    this->appSettings = appSettings;

    resize((int)(792 * appSettings->screenRatio), (int)(458 * appSettings->screenRatio));
    setGeometry(QStyle::alignedRect(Qt::RightToLeft, Qt::AlignCenter, size(), QGuiApplication::primaryScreen()->availableGeometry()));
    setWindowTitle("نمونه‌های جست‌وجو");
    setWindowIcon(QIcon(":/files/images/ghazal-256x256.png"));
    setWindowModality(Qt::WindowModal);

    setHtml();
}

SearchExamplesForm::~SearchExamplesForm()
{
    delete ui;
}

void SearchExamplesForm::keyPressEvent(QKeyEvent *e)
{
    if(e->key() == Qt::Key_Escape)
        close();
}

void SearchExamplesForm::setHtml()
{
    QString html;
    QString textColor = appSettings->isDarkMode ? "white" : "black";
    QFile file(":/files/html/search_examples.html");

    file.open(QIODevice::ReadOnly | QIODevice::Text);
    html = file.readAll();
    file.close();

    html.replace("${FontName}", appSettings->viewFN);
    html.replace("${FontSize}", QString::number(appSettings->viewFS.toDouble()));
    html.replace("${TopicFontSize}", ratioFontSize(appSettings->viewFS.toDouble(), 1.25));
    html.replace("${TextColor}", textColor);

    ui->textBrowser->setHtml(html);
}

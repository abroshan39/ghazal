/*
    [Ghazal: The library of persian poetry]
    Publisher: Rosybit
    Url: http://www.rosybit.com
    GitHub: https://github.com/abroshan39/ghazal
    Version: 1.4
    Author: Aboutaleb Roshan [ab.roshan39@gmail.com]
    License: MIT License
*/

#include "wordsearchform.h"
#include "ui_wordsearchform.h"
#include "event_functions.h"

WordSearchForm::WordSearchForm(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WordSearchForm)
{
    ui->setupUi(this);
}

WordSearchForm::WordSearchForm(AppSettings *appSettings, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WordSearchForm)
{
    ui->setupUi(this);

    this->appSettings = appSettings;

    appSettings->isOpenWordSearchForm = true;
    setGeometry(QStyle::alignedRect(Qt::RightToLeft, Qt::AlignCenter, size(), QGuiApplication::primaryScreen()->availableGeometry()));
    setWindowTitle("جست‌وجو");
    setWindowIcon(QIcon(":/files/images/ghazal-256x256.png"));

    lineEditDirectionCorrector(ui->lineEdit);
    connect(new ZWNJPress(ui->lineEdit), &ZWNJPress::zwnjPressed, this, &WordSearchForm::lineEditZWNJPressed);
}

WordSearchForm::~WordSearchForm()
{
    delete ui;
}

void WordSearchForm::keyPressEvent(QKeyEvent *e)
{
    if(e->key() == Qt::Key_Return)
        on_pushButton_clicked();
    if(e->key() == Qt::Key_Escape)
        close();
}

void WordSearchForm::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);  // (void)event;
    appSettings->isOpenWordSearchForm = false;
}

void WordSearchForm::on_pushButton_clicked()
{
    QRegularExpression regex("^:{2}([123]\\-\\d+|\\d+)$");  // ^:{2}([123]\-\d+|\d+)$
    QRegularExpressionMatch match = regex.match(ui->lineEdit->text());
    if(match.hasMatch())
    {
        if(match.captured(1).contains("-"))
        {
            QString levelID = match.captured(1);
            QStringList list = levelID.split("-");

            QSqlQuery query;
            if(list[0] == "3")
                query.exec("SELECT * FROM poem WHERE id = " + list[1]);
            else
                query.exec("SELECT * FROM cat WHERE id = " + list[1]);

            if(query.first())
            {
                connect(this, SIGNAL(sigSetTabContent(QString, bool, bool, QStringList, QString)), appSettings->activeTab, SLOT(slotSetTabContent(QString, bool, bool, QStringList, QString)));
                emit sigSetTabContent(levelID, false, false, QStringList(), QString());
                disconnect(this, SIGNAL(sigSetTabContent(QString, bool, bool, QStringList, QString)), appSettings->activeTab, SLOT(slotSetTabContent(QString, bool, bool, QStringList, QString)));

                GanjoorPath gp = recursiveIDs(appSettings->mainDB, levelID);
                int tabIndex = appSettings->tabWidget->indexOf(appSettings->activeTab);
                appSettings->tabWidget->setTabText(tabIndex, "  " + gp.text[gp.text.count() - 1] + "  ");
                return;
            }
        }
        else
        {
            QString level, id = match.captured(1);
            QSqlQuery query;
            query.exec("SELECT * FROM poem WHERE id = " + id);
            if(query.first())
                level = "3";
            else
            {
                query.exec("SELECT * FROM cat WHERE id = " + id);
                if(query.first())
                    level = "2";
            }

            if(!level.isEmpty())
            {
                connect(this, SIGNAL(sigSetTabContent(QString, bool, bool, QStringList, QString)), appSettings->activeTab, SLOT(slotSetTabContent(QString, bool, bool, QStringList, QString)));
                emit sigSetTabContent(level + "-" + id, false, false, QStringList(), QString());
                disconnect(this, SIGNAL(sigSetTabContent(QString, bool, bool, QStringList, QString)), appSettings->activeTab, SLOT(slotSetTabContent(QString, bool, bool, QStringList, QString)));

                GanjoorPath gp = recursiveIDs(appSettings->mainDB, level + "-" + id);
                int tabIndex = appSettings->tabWidget->indexOf(appSettings->activeTab);
                appSettings->tabWidget->setTabText(tabIndex, "  " + gp.text[gp.text.count() - 1] + "  ");
                return;
            }
        }
    }

    QStringList list(textListHighlight(ui->lineEdit->text()));

    connect(this, SIGNAL(sigSetTabContent(QString, bool, bool, QStringList, QString)), appSettings->activeTab, SLOT(slotSetTabContent(QString, bool, bool, QStringList, QString)));
    emit sigSetTabContent(appSettings->tabLastLevelID.value(appSettings->activeTab), false, true, list, QString());
    disconnect(this, SIGNAL(sigSetTabContent(QString, bool, bool, QStringList, QString)), appSettings->activeTab, SLOT(slotSetTabContent(QString, bool, bool, QStringList, QString)));
}

void WordSearchForm::lineEditZWNJPressed(QObject *object, Qt::KeyboardModifier key)
{
    Q_UNUSED(object);  // (void)object;
    Q_UNUSED(key);     // (void)key;
    checkDirection = false;
    ui->lineEdit->insert(Constants::ZWNJ);
}

void WordSearchForm::on_lineEdit_textChanged(const QString &arg1)
{
    if(checkDirection && arg1.contains(QRegularExpression("^:{2}")))
        lineEditDirectionCorrector(ui->lineEdit, Qt::LeftToRight);
    else if(checkDirection)
        lineEditDirectionCorrector(ui->lineEdit);
    checkDirection = true;
}

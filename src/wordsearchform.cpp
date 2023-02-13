/*
    [Ghazal: The library of persian poetry]
    Publisher: Rosybit
    Url: http://www.rosybit.com
    GitHub: https://github.com/abroshan39/ghazal
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
    resize((int)(250 * appSettings->screenRatio), 88);
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
    if(e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter)
        on_pushButton_clicked();
    if(e->key() == Qt::Key_Escape)
        close();
}

void WordSearchForm::closeEvent(QCloseEvent *e)
{
    Q_UNUSED(e);  // (void)e;
    appSettings->isOpenWordSearchForm = false;
}

void WordSearchForm::on_pushButton_clicked()
{
    QRegularExpression regex("^:{2}([123]\\-)?(\\d+)(\\-\\d+([hb])?)?$");  // ^:{2}([123]\-)?(\d+)(\-\d+([hb])?)?$
    QRegularExpressionMatch match = regex.match(ui->lineEdit->text());
    if(match.hasMatch())
    {
        QSqlQuery query;
        QString level(match.captured(1).remove("-"));
        QString id(match.captured(2));
        QString vorder(match.captured(3).remove(QRegularExpression("[\\-hb]")));
        bool highlight = !match.captured(4).isEmpty();

        if(!level.isEmpty())
        {
            if(level == "3")
                query.exec("SELECT * FROM poem WHERE id = " + id);
            else
                query.exec("SELECT * FROM cat WHERE id = " + id);

            if(query.first())
            {
                setContents(appSettings->activeTab, level + "-" + id, false, false, QStringList(), vorder, highlight);

                GanjoorPath gp = recursiveIDs(appSettings->mainDB, level + "-" + id);
                int tabIndex = appSettings->tabWidget->indexOf(appSettings->activeTab);
                appSettings->tabWidget->setTabText(tabIndex, "  " + gp.text[gp.text.count() - 1] + "  ");
                return;
            }
        }
        else
        {
            query.exec("SELECT * FROM poem WHERE id = " + id);
            if(query.first())
            {
                level = "3";
            }
            else
            {
                query.exec("SELECT * FROM cat WHERE id = " + id);
                if(query.first())
                    level = "2";
            }

            if(!level.isEmpty())
            {
                setContents(appSettings->activeTab, level + "-" + id, false, false, QStringList(), vorder, highlight);

                GanjoorPath gp = recursiveIDs(appSettings->mainDB, level + "-" + id);
                int tabIndex = appSettings->tabWidget->indexOf(appSettings->activeTab);
                appSettings->tabWidget->setTabText(tabIndex, "  " + gp.text[gp.text.count() - 1] + "  ");
                return;
            }
        }
    }

    QStringList list(textListHighlight(ui->lineEdit->text()));
    setContents(appSettings->activeTab, appSettings->tabLastLevelID.value(appSettings->activeTab), false, true, list);
}

void WordSearchForm::setContents(QWidget *ptrTab, const QString &levelID, bool setFocusListWidget, bool rememberScrollBarValue, const QStringList &highlightText, const QString &vorder, bool highlightVorder, bool highlightWithUnderline)
{
    connect(this, SIGNAL(sigSetTabContent(QString, bool, bool, QStringList, QString, bool, bool)), ptrTab, SLOT(slotSetTabContent(QString, bool, bool, QStringList, QString, bool, bool)));
    emit sigSetTabContent(levelID, setFocusListWidget, rememberScrollBarValue, highlightText, vorder, highlightVorder, highlightWithUnderline);
    disconnect(this, SIGNAL(sigSetTabContent(QString, bool, bool, QStringList, QString, bool, bool)), ptrTab, SLOT(slotSetTabContent(QString, bool, bool, QStringList, QString, bool, bool)));
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

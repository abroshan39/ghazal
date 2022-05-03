/*
    [Ghazal: The library of persian poetry]
    Publisher: Rosybit
    Url: http://www.rosybit.com
    GitHub: https://github.com/abroshan39/ghazal
    Version: 1.4
    Author: Aboutaleb Roshan [ab.roshan39@gmail.com]
    License: MIT License
*/

#include "abjadformmini.h"
#include "ui_abjadformmini.h"
#include "abjad_class.h"

AbjadFormMini::AbjadFormMini(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::AbjadFormMini)
{
    ui->setupUi(this);
}

AbjadFormMini::AbjadFormMini(AppSettings *appSettings, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::AbjadFormMini)
{
    ui->setupUi(this);

    this->appSettings = appSettings;

    appSettings->isOpenAbjadFormMini = true;
    setGeometry(QStyle::alignedRect(Qt::RightToLeft, Qt::AlignCenter, size(), QGuiApplication::primaryScreen()->availableGeometry()));
    setWindowTitle("حساب ابجد");
    setWindowIcon(QIcon(":/files/images/ghazal-256x256.png"));
    ui->textBrowser->setEnabled(false);

    connect(parent, SIGNAL(sigSelectedText(const QString &)), this, SLOT(slotSelectedText(const QString &)));
}

AbjadFormMini::~AbjadFormMini()
{
    delete ui;
}

void AbjadFormMini::keyPressEvent(QKeyEvent *e)
{
    if(e->key() == Qt::Key_Return)
        close();
}

void AbjadFormMini::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);  // (void)event;
    appSettings->isOpenAbjadFormMini = false;
}

void AbjadFormMini::slotSelectedText(const QString &text)
{
    abjadText = text;
    QString abChars(QString::fromStdWString(abjad::abjadCharacters));
    QRegularExpression regex("[^" + abChars + "]");
    abjadText.remove(regex);
    int n = abjad::abHesab(abjadText.toStdWString());

    ui->lineEdit->setText(QString::number(n));
    if(ui->checkBox->isChecked())
    {
        if(n)
        {
            QStringList list(abjadText.split("", SKIP_EMPTY_PARTS));
            QString strResult;
            for(int i = 0; i < list.size(); i++)
            {
                wchar_t ch = list[i][0].unicode();
                QString value(QString::number(abjad::abChar(ch)));
                strResult += list[i] + "(" + value + ") + ";
            }
            strResult = strResult.left(strResult.size() - 3) + "<br />=<br />" + QString::number(n);
            strResult = QString("<p align=\"center\">%1</p>").arg(strResult);
            ui->textBrowser->setHtml(strResult);
        }
        else
        {
            ui->textBrowser->clear();
        }
    }
}

void AbjadFormMini::on_checkBox_toggled(bool checked)
{
    if(checked)
    {
        ui->textBrowser->setEnabled(true);
        slotSelectedText(abjadText);
    }
    else
    {
        ui->textBrowser->clear();
        ui->textBrowser->setEnabled(false);
    }
}

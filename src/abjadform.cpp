/*
    [Ghazal: The library of persian poetry]
    Publisher: Rosybit
    Url: http://www.rosybit.com
    GitHub: https://github.com/abroshan39/ghazal
    Author: Aboutaleb Roshan [ab.roshan39@gmail.com]
    License: MIT License
*/

#include "abjadform.h"
#include "ui_abjadform.h"
#include "abjad_class.h"
#include "event_functions.h"

AbjadForm::AbjadForm(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::AbjadForm)
{
    ui->setupUi(this);
}

AbjadForm::AbjadForm(AppSettings *appSettings, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::AbjadForm)
{
    ui->setupUi(this);

    this->appSettings = appSettings;

    appSettings->isOpenAbjadForm = true;
    resize((int)(556 * appSettings->screenRatio), (int)(556 * appSettings->screenRatio));
    setGeometry(QStyle::alignedRect(Qt::RightToLeft, Qt::AlignCenter, size(), QGuiApplication::primaryScreen()->availableGeometry()));
    setWindowTitle("محاسبه‌گر ابجد");
    setWindowIcon(QIcon(":/files/images/ghazal-256x256.png"));
    ui->comboBox->setCurrentIndex(ui->comboBox->count() - 1);

    connect(new ZWNJPress(ui->textEdit), &ZWNJPress::zwnjPressed, this, &AbjadForm::textEditZWNJPressed);
}

AbjadForm::~AbjadForm()
{
    delete ui;
}

void AbjadForm::keyPressEvent(QKeyEvent *e)
{
    if(e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter)
        on_btnCalculate_clicked();
    if(e->key() == Qt::Key_Escape)
        on_btnExit_clicked();
}

void AbjadForm::closeEvent(QCloseEvent *e)
{
    Q_UNUSED(e);  // (void)e;
    appSettings->isOpenAbjadForm = false;
}

void AbjadForm::on_btnCalculate_clicked()
{
    wstring text(ui->textEdit->toPlainText().toStdWString());
    abjad ab(text);

    if(option == 1) ui->lineEdit->setText(QString::number(ab.abHesab(ab.KABIR, ab.MOJMAL)));
    else if(option == 2) ui->lineEdit->setText(QString::number(ab.abHesab(ab.KABIR, ab.MOFASSAL)));
    else if(option == 3) ui->lineEdit->setText(QString::number(ab.abHesab(ab.SAGIR, ab.MOJMAL)));
    else if(option == 4) ui->lineEdit->setText(QString::number(ab.abHesab(ab.SAGIR, ab.MOFASSAL)));
    else if(option == 5) ui->lineEdit->setText(QString::number(ab.abHesab(ab.VASIT, ab.MOJMAL)));
    else if(option == 6) ui->lineEdit->setText(QString::number(ab.abHesab(ab.VASIT, ab.MOFASSAL)));
    else if(option == 7) ui->lineEdit->setText(QString::number(ab.abHesab(ab.MAKUS, ab.MOJMAL)));
    else if(option == 8) ui->lineEdit->setText(QString::number(ab.abHesab(ab.MAKUS, ab.MOFASSAL)));
    else if(option == 9) ui->lineEdit->setText(QString::number(ab.abHesab(ab.SAGIR2, ab.MOJMAL)));
    else if(option == 10) ui->lineEdit->setText(QString::number(ab.abHesab(ab.SAGIR2, ab.MOFASSAL)));

    if(ui->checkBox_4->isChecked())
    {
        ui->label_12->setText(QString::number(ab.abHarf()));
        ui->label_14->setText(QString::number(ab.abNoghte()));
    }

    if(ui->checkBox_1->isChecked())
    {
        ui->label_1->setText(QString::number(abjad::abHesab(text)));
        ui->label_2->setText(QString::number(abjad::abHesab(text, abjad::KABIR, abjad::MOFASSAL)));
        ui->label_3->setText(QString::number(abjad::abHesab(text, abjad::SAGIR)));
        ui->label_4->setText(QString::number(abjad::abHesab(text, abjad::SAGIR, abjad::MOFASSAL)));
        ui->label_5->setText(QString::number(abjad::abHesab(text, abjad::VASIT)));
        ui->label_6->setText(QString::number(abjad::abHesab(text, abjad::VASIT, abjad::MOFASSAL)));
        if(ui->checkBox_5->isChecked())
        {
            ui->label_7->setText(QString::number(abjad::abHesab(text, abjad::MAKUS)));
            ui->label_8->setText(QString::number(abjad::abHesab(text, abjad::MAKUS, abjad::MOFASSAL)));
        }
        ui->label_9->setText(QString::number(abjad::abHesab(text, abjad::SAGIR2)));
        ui->label_10->setText(QString::number(abjad::abHesab(text, abjad::SAGIR2, abjad::MOFASSAL)));
    }

    if(ui->checkBox_3->isChecked())
    {
        QString abjadText(ui->textEdit->toPlainText());
        QString diacritics(QString::fromStdWString(abjad::diacritics));
        if(ui->checkBox_9->isChecked())
        {
            QRegularExpression regex("[" + diacritics + "]");
            abjadText.remove(regex);
            diacritics.clear();
        }
        if(ui->checkBox_8->isChecked())
        {
            QString abChars(QString::fromStdWString(abjad::abjadCharacters));
            QRegularExpression regex("[^" + abChars + diacritics + QChar(abjad::zwnj) + QChar(abjad::zwj) + "\\s]");
            abjadText.remove(regex);
        }
        QStringList list(abjadText.split(QRegularExpression("[\\s]"), SKIP_EMPTY_PARTS));
        QString space(" ");
        QString strResult;

        ui->textBrowser->clear();
        space = space.repeated(ui->spinBox->value());

        for(int i = 0; i < list.size(); i++)
        {
            wstring word(list[i].toStdWString());
            QString value;

            if     (option == 1)  value = QString::number(abjad::abHesab(word, abjad::KABIR, abjad::MOJMAL));
            else if(option == 2)  value = QString::number(abjad::abHesab(word, abjad::KABIR, abjad::MOFASSAL));
            else if(option == 3)  value = QString::number(abjad::abHesab(word, abjad::SAGIR, abjad::MOJMAL));
            else if(option == 4)  value = QString::number(abjad::abHesab(word, abjad::SAGIR, abjad::MOFASSAL));
            else if(option == 5)  value = QString::number(abjad::abHesab(word, abjad::VASIT, abjad::MOJMAL));
            else if(option == 6)  value = QString::number(abjad::abHesab(word, abjad::VASIT, abjad::MOFASSAL));
            else if(option == 7)  value = QString::number(abjad::abHesab(word, abjad::MAKUS, abjad::MOJMAL));
            else if(option == 8)  value = QString::number(abjad::abHesab(word, abjad::MAKUS, abjad::MOFASSAL));
            else if(option == 9)  value = QString::number(abjad::abHesab(word, abjad::SAGIR2, abjad::MOJMAL));
            else if(option == 10) value = QString::number(abjad::abHesab(word, abjad::SAGIR2, abjad::MOFASSAL));

            strResult += list[i] + "(" + value + ")" + space;
        }
        ui->textBrowser->setText(strResult.trimmed());
    }

    if(ui->checkBox_7->isChecked())
    {
        QString abjadText(ui->textEdit->toPlainText());
        QString abChars(QString::fromStdWString(abjad::abjadCharacters));
        QRegularExpression regex("[^" + abChars + "]");
        abjadText.remove(regex);

        QStringList list(abjadText.split("", SKIP_EMPTY_PARTS));
        QString space(" ");
        QString strResult;

        ui->textBrowser->clear();
        space = space.repeated(ui->spinBox->value());

        for(int i = 0; i < list.size(); i++)
        {
            wchar_t ch = list[i][0].unicode();
            QString value;

            if     (option == 1)  value = QString::number(abjad::abChar(ch, abjad::KABIR));
            else if(option == 2)  value = QString::number(abjad::abCharMofassal(ch, abjad::KABIR));
            else if(option == 3)  value = QString::number(abjad::abChar(ch, abjad::SAGIR));
            else if(option == 4)  value = QString::number(abjad::abCharMofassal(ch, abjad::SAGIR));
            else if(option == 5)  value = QString::number(abjad::abChar(ch, abjad::VASIT));
            else if(option == 6)  value = QString::number(abjad::abCharMofassal(ch, abjad::VASIT));
            else if(option == 7)  value = QString::number(abjad::abChar(ch, abjad::MAKUS));
            else if(option == 8)  value = QString::number(abjad::abCharMofassal(ch, abjad::MAKUS));
            else if(option == 9)  value = QString::number(abjad::abChar(ch, abjad::SAGIR2));
            else if(option == 10) value = QString::number(abjad::abCharMofassal(ch, abjad::SAGIR2));

            strResult += list[i] + "(" + value + ")" + space;
        }
        ui->textBrowser->setText(strResult.trimmed());
    }
}

void AbjadForm::on_btnExit_clicked()
{
    close();
}

void AbjadForm::on_radioButton_1_clicked() {option = 1; on_btnCalculate_clicked();}
void AbjadForm::on_radioButton_2_clicked() {option = 2; on_btnCalculate_clicked();}
void AbjadForm::on_radioButton_3_clicked() {option = 3; on_btnCalculate_clicked();}
void AbjadForm::on_radioButton_4_clicked() {option = 4; on_btnCalculate_clicked();}
void AbjadForm::on_radioButton_5_clicked() {option = 5; on_btnCalculate_clicked();}
void AbjadForm::on_radioButton_6_clicked() {option = 6; on_btnCalculate_clicked();}
void AbjadForm::on_radioButton_7_clicked() {option = 7; on_btnCalculate_clicked();}
void AbjadForm::on_radioButton_8_clicked() {option = 8; on_btnCalculate_clicked();}
void AbjadForm::on_radioButton_9_clicked() {option = 9; on_btnCalculate_clicked();}
void AbjadForm::on_radioButton_10_clicked() {option = 10; on_btnCalculate_clicked();}
void AbjadForm::on_spinBox_valueChanged(int arg1) {Q_UNUSED(arg1); on_btnCalculate_clicked();}

void AbjadForm::on_textEdit_textChanged()
{
    if(ui->checkBox_2->isChecked()) on_btnCalculate_clicked();

    if(ui->textEdit->toPlainText().trimmed().isEmpty())
    {
        on_checkBox_1_clicked(false);
        ui->label_12->clear();
        ui->label_14->clear();
        ui->lineEdit->clear();
    }
}

void AbjadForm::on_checkBox_1_clicked(bool checked)
{
    if(checked)
    {
        on_btnCalculate_clicked();
    }
    else
    {
        ui->label_1->clear();
        ui->label_2->clear();
        ui->label_3->clear();
        ui->label_4->clear();
        ui->label_5->clear();
        ui->label_6->clear();
        ui->label_7->clear();
        ui->label_8->clear();
        ui->label_9->clear();
        ui->label_10->clear();
    }
}

void AbjadForm::on_checkBox_2_clicked(bool checked)
{
    if(checked) on_btnCalculate_clicked();
}

void AbjadForm::on_checkBox_3_clicked(bool checked)
{
    if(checked)
    {
        ui->textBrowser->setEnabled(true);
        ui->checkBox_7->setChecked(false);
        if(ui->checkBox_2->isChecked() && !ui->textEdit->toPlainText().trimmed().isEmpty())
            on_btnCalculate_clicked();
    }
    else if(!checked && !ui->checkBox_7->isChecked())
    {
        ui->textBrowser->clear();
        ui->textBrowser->setEnabled(false);
    }
}

void AbjadForm::on_checkBox_3_toggled(bool checked)
{
    ui->checkBox_8->setEnabled(checked);
    ui->checkBox_9->setEnabled(checked);
}

void AbjadForm::on_checkBox_4_clicked(bool checked)
{
    if(!checked)
    {
        ui->label_12->clear();
        ui->label_14->clear();
    }
}

void AbjadForm::on_checkBox_5_clicked(bool checked)
{
    if(checked)
    {
        ui->radioButton_7->setEnabled(true);
        ui->radioButton_8->setEnabled(true);
    }
    else
    {
        ui->radioButton_7->setEnabled(false);
        ui->radioButton_8->setEnabled(false);
        ui->label_7->clear();
        ui->label_8->clear();
    }
}

void AbjadForm::on_checkBox_6_clicked(bool checked)
{
    ui->comboBox->setEnabled(checked);
}

void AbjadForm::on_checkBox_7_clicked(bool checked)
{
    if(checked)
    {
        ui->textBrowser->setEnabled(true);
        ui->checkBox_3->setChecked(false);
        if(ui->checkBox_2->isChecked() && !ui->textEdit->toPlainText().trimmed().isEmpty())
            on_btnCalculate_clicked();
    }
    else if(!checked && !ui->checkBox_3->isChecked())
    {
        ui->textBrowser->clear();
        ui->textBrowser->setEnabled(false);
    }
}

void AbjadForm::on_checkBox_8_clicked()
{
    if(ui->checkBox_2->isChecked() && !ui->textEdit->toPlainText().trimmed().isEmpty())
        on_btnCalculate_clicked();
}

void AbjadForm::on_checkBox_9_clicked()
{
    if(ui->checkBox_2->isChecked() && !ui->textEdit->toPlainText().trimmed().isEmpty())
        on_btnCalculate_clicked();
}

void AbjadForm::slotSelectedText(const QString &text)
{
    ui->textEdit->setText(text);
}

void AbjadForm::textEditZWNJPressed(QObject *object, Qt::KeyboardModifier key)
{
    Q_UNUSED(object);  // (void)object;

    if(ui->checkBox_6->isChecked())
    {
        if((ui->comboBox->currentIndex() == ui->comboBox->count() - 1) ||
           (ui->comboBox->currentIndex() == 0 && key == Qt::ShiftModifier) ||
           (ui->comboBox->currentIndex() == 1 && key == Qt::ControlModifier))
            ui->textEdit->insertPlainText(QChar(abjad::zwnj));
    }
}

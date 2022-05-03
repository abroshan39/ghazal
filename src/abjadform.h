/*
    [Ghazal: The library of persian poetry]
    Publisher: Rosybit
    Url: http://www.rosybit.com
    GitHub: https://github.com/abroshan39/ghazal
    Version: 1.4
    Author: Aboutaleb Roshan [ab.roshan39@gmail.com]
    License: MIT License
*/

#ifndef ABJADFORM_H
#define ABJADFORM_H

#include <QMainWindow>
#include "common_functions.h"

using std::wstring;

QT_BEGIN_NAMESPACE
namespace Ui { class AbjadForm; }
QT_END_NAMESPACE

class AbjadForm : public QMainWindow
{
    Q_OBJECT

public:
    explicit AbjadForm(QWidget *parent = nullptr);
    AbjadForm(AppSettings *appSettings, QWidget *parent = nullptr);
    ~AbjadForm();

public slots:
    void slotSelectedText(const QString &text);
    void textEditZWNJPressed(QObject *object, Qt::KeyboardModifier key);

private slots:
    void on_btnCalculate_clicked();
    void on_btnExit_clicked();
    void on_textEdit_textChanged();
    void on_radioButton_1_clicked();
    void on_radioButton_2_clicked();
    void on_radioButton_3_clicked();
    void on_radioButton_4_clicked();
    void on_radioButton_5_clicked();
    void on_radioButton_6_clicked();
    void on_radioButton_7_clicked();
    void on_radioButton_8_clicked();
    void on_radioButton_9_clicked();
    void on_radioButton_10_clicked();
    void on_spinBox_valueChanged(int arg1);
    void on_checkBox_1_clicked(bool checked);
    void on_checkBox_2_clicked(bool checked);
    void on_checkBox_3_clicked(bool checked);
    void on_checkBox_3_toggled(bool checked);
    void on_checkBox_4_clicked(bool checked);
    void on_checkBox_5_clicked(bool checked);
    void on_checkBox_6_clicked(bool checked);
    void on_checkBox_7_clicked(bool checked);
    void on_checkBox_8_clicked();
    void on_checkBox_9_clicked();

protected:
    void keyPressEvent(QKeyEvent *e) override;
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::AbjadForm *ui;
    AppSettings *appSettings;
    int option = 1;
};
#endif // ABJADFORM_H

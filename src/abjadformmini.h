/*
    [Ghazal: The library of persian poetry]
    Publisher: Rosybit
    Url: http://www.rosybit.com
    GitHub: https://github.com/abroshan39/ghazal
    Version: 1.4
    Author: Aboutaleb Roshan [ab.roshan39@gmail.com]
    License: MIT License
*/

#ifndef ABJADFORMMINI_H
#define ABJADFORMMINI_H

#include <QMainWindow>
#include "common_functions.h"

using std::wstring;

namespace Ui {
class AbjadFormMini;
}

class AbjadFormMini : public QMainWindow
{
    Q_OBJECT

public slots:
    void slotSelectedText(const QString &text);

public:
    explicit AbjadFormMini(QWidget *parent = nullptr);
    AbjadFormMini(AppSettings *appSettings, QWidget *parent = nullptr);
    ~AbjadFormMini();

protected:
    void keyPressEvent(QKeyEvent *e) override;
    void closeEvent(QCloseEvent *event) override;

private slots:
    void on_checkBox_toggled(bool checked);

private:
    Ui::AbjadFormMini *ui;
    AppSettings *appSettings;
    QString abjadText;
};

#endif // ABJADFORMMINI_H

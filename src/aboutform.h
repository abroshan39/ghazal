/*
    [Ghazal: The library of persian poetry]
    Publisher: Rosybit
    Url: http://www.rosybit.com
    GitHub: https://github.com/abroshan39/ghazal
    Author: Aboutaleb Roshan [ab.roshan39@gmail.com]
    License: MIT License
*/

#ifndef ABOUTFORM_H
#define ABOUTFORM_H

#include <QMainWindow>
#include "common_functions.h"

namespace Ui {
class AboutForm;
}

class AboutForm : public QMainWindow
{
    Q_OBJECT

public:
    explicit AboutForm(QWidget *parent = nullptr);
    AboutForm(AppSettings *appSettings, QWidget *parent = nullptr);
    ~AboutForm();

public slots:
    void setHtml();

protected:
    void keyPressEvent(QKeyEvent *e) override;

private:
    Ui::AboutForm *ui;
    AppSettings *appSettings;
};

#endif // ABOUTFORM_H

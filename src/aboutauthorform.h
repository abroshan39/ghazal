/*
    [Ghazal: The library of persian poetry]
    Publisher: Rosybit
    Url: http://www.rosybit.com
    GitHub: https://github.com/abroshan39/ghazal
    Version: 1.4
    Author: Aboutaleb Roshan [ab.roshan39@gmail.com]
    License: MIT License
*/

#ifndef ABOUTAUTHORFORM_H
#define ABOUTAUTHORFORM_H

#include <QMainWindow>
#include "common_functions.h"

namespace Ui {
class AboutAuthorForm;
}

class AboutAuthorForm : public QMainWindow
{
    Q_OBJECT

public:
    explicit AboutAuthorForm(QWidget *parent = nullptr);
    AboutAuthorForm(AppSettings *appSettings, QWidget *parent = nullptr);
    ~AboutAuthorForm();

public slots:
    void setHtml();

protected:
    void keyPressEvent(QKeyEvent *e) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    Ui::AboutAuthorForm *ui;
    AppSettings *appSettings;
};

#endif // ABOUTAUTHORFORM_H

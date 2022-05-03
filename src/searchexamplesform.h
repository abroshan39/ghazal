/*
    [Ghazal: The library of persian poetry]
    Publisher: Rosybit
    Url: http://www.rosybit.com
    GitHub: https://github.com/abroshan39/ghazal
    Version: 1.4
    Author: Aboutaleb Roshan [ab.roshan39@gmail.com]
    License: MIT License
*/

#ifndef SEARCHEXAMPLESFORM_H
#define SEARCHEXAMPLESFORM_H

#include <QMainWindow>
#include "common_functions.h"

namespace Ui {
class SearchExamplesForm;
}

class SearchExamplesForm : public QMainWindow
{
    Q_OBJECT

public:
    explicit SearchExamplesForm(QWidget *parent = nullptr);
    SearchExamplesForm(AppSettings *appSettings, QWidget *parent = nullptr);
    ~SearchExamplesForm();

public slots:
    void setHtml();

protected:
    void keyPressEvent(QKeyEvent *e) override;

private:
    Ui::SearchExamplesForm *ui;
    AppSettings *appSettings;
};

#endif // SEARCHEXAMPLESFORM_H

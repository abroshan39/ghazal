/*
    [Ghazal: The library of persian poetry]
    Publisher: Rosybit
    Url: http://www.rosybit.com
    GitHub: https://github.com/abroshan39/ghazal
    Version: 1.4
    Author: Aboutaleb Roshan [ab.roshan39@gmail.com]
    License: MIT License
*/

#ifndef WORDSEARCHFORM_H
#define WORDSEARCHFORM_H

#include <QMainWindow>
#include "common_functions.h"

namespace Ui {
class WordSearchForm;
}

class WordSearchForm : public QMainWindow
{
    Q_OBJECT

public:
    explicit WordSearchForm(QWidget *parent = nullptr);
    WordSearchForm(AppSettings *appSettings, QWidget *parent = nullptr);
    ~WordSearchForm();

signals:
    void sigSetTabContent(const QString &levelID, bool setFocusListWidget, bool rememberScrollBarValue, const QStringList &highlightText, const QString &bookmarkVerseID);

public slots:
    void lineEditZWNJPressed(QObject *object, Qt::KeyboardModifier key);

private slots:
    void on_pushButton_clicked();
    void on_lineEdit_textChanged(const QString &arg1);

protected:
    void keyPressEvent(QKeyEvent *e) override;
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::WordSearchForm *ui;
    AppSettings *appSettings;
    bool checkDirection = true;
};

#endif // WORDSEARCHFORM_H

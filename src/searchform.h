/*
    [Ghazal: The library of persian poetry]
    Publisher: Rosybit
    Url: http://www.rosybit.com
    GitHub: https://github.com/abroshan39/ghazal
    Version: 1.4
    Author: Aboutaleb Roshan [ab.roshan39@gmail.com]
    License: MIT License
*/

#ifndef SEARCHFORM_H
#define SEARCHFORM_H

#include <QMainWindow>
#include "common_functions.h"

namespace Ui {
class SearchForm;
}

class SearchForm : public QMainWindow
{
    Q_OBJECT

public:
    explicit SearchForm(QWidget *parent = nullptr);
    SearchForm(AppSettings *appSettings, QWidget *parent = nullptr);
    ~SearchForm();

signals:
    void sigSearchTableChanged();
    void sigSearch();

public slots:
    void labelUpdate();
    void lineEditsZWNJPressed(QObject *object, Qt::KeyboardModifier key);
    void searchRangeMenuCreator();
    void actionCat();
    void actionPoem();
    void actionVerse();

private slots:
    void on_listWidget_itemChanged(QListWidgetItem *item);
    void on_listWidget_doubleClicked(const QModelIndex &index);
    void on_selectAllCheckBox_clicked(bool checked);
    void on_selectNoneCheckBox_clicked(bool checked);
    void on_skipDiacriticsCheckBox_clicked(bool checked);
    void on_skipCharTypesCheckBox_clicked(bool checked);
    void on_btnOK_clicked();
    void on_btnClose_clicked();
    void on_btnSearch_clicked();
    void on_checkBoxHash_toggled(bool checked);
    void on_btnExamples_clicked();

protected:
    void keyPressEvent(QKeyEvent *e) override;

private:
    Ui::SearchForm *ui;
    AppSettings *appSettings;
    bool fromFormLoad = true;
    bool skipDiacritics;
    bool skipCharTypes;
    QString strSearch;

    QAction *catAction;
    QAction *poemAction;
    QAction *verseAction;
};

#endif // SEARCHFORM_H

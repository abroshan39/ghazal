/*
    [Ghazal: The library of persian poetry]
    Publisher: Rosybit
    Url: http://www.rosybit.com
    GitHub: https://github.com/abroshan39/ghazal
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
    void singleCheckBoxClicked(bool checked);
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
    void on_checkBoxSkipDiacritics_clicked(bool checked);
    void on_checkBoxSkipCharTypes_clicked(bool checked);
    void on_checkBoxShowItems_clicked(bool checked);
    void on_btnOK_clicked();
    void on_btnClose_clicked();
    void on_btnSearch_clicked();
    void on_btnExamples_clicked();
    void on_checkBoxCounter_clicked(bool checked);
    void on_checkBoxRadif_clicked(bool checked);
    void on_checkBoxGhafie_clicked(bool checked);

protected:
    void keyPressEvent(QKeyEvent *e) override;

private:
    Ui::SearchForm *ui;
    AppSettings *appSettings;
    bool allItemsSelected;
    QStringList poetIDList;
    SearchTable searchTable;
    SearchMethod searchMethod;
    bool skipDiacritics;
    bool skipCharTypes;
    bool showItemsDuringSearch;
    QString strSearch;

    QAction *catAction;
    QAction *poemAction;
    QAction *verseAction;
};

#endif // SEARCHFORM_H

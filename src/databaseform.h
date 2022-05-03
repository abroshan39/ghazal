/*
    [Ghazal: The library of persian poetry]
    Publisher: Rosybit
    Url: http://www.rosybit.com
    GitHub: https://github.com/abroshan39/ghazal
    Version: 1.4
    Author: Aboutaleb Roshan [ab.roshan39@gmail.com]
    License: MIT License
*/

#ifndef DATABASEFORM_H
#define DATABASEFORM_H

#include <QMainWindow>
#include "common_functions.h"
#include "worker.h"

namespace Ui {
class DatabaseForm;
}

class DatabaseForm : public QMainWindow
{
    Q_OBJECT

public:
    explicit DatabaseForm(QWidget *parent = nullptr);
    DatabaseForm(AppSettings *appSettings, QWidget *parent = nullptr);
    ~DatabaseForm();

signals:
    void sigMainDBChanged();
    void sigUpdatePoetList();

public slots:
    void slotMainDBChanged();
    void slotUpdatePoetList();
    void labelUpdate();
    void threadStart(Worker *worker);
    void threadFinished(Worker::WorkerType type, QVariant result);

private slots:
    void on_listWidget_itemChanged(QListWidgetItem *item);
    void on_listWidget_doubleClicked(const QModelIndex &index);
    void on_selectAllCheckBox_clicked(bool checked);
    void on_selectNoneCheckBox_clicked(bool checked);
    void on_btnClose_clicked();
    void on_btnAddPoet_clicked();
    void on_btnRemovePoet_clicked();
    void on_btnCompactDB_clicked();
    void on_btnDownloadForm_clicked();
    void on_btnExport_clicked();

protected:
    void keyPressEvent(QKeyEvent *e) override;
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::DatabaseForm *ui;
    AppSettings *appSettings;
    QStringList poetSelectedIDs;
    bool isProcessing = false;
};

#endif // DATABASEFORM_H

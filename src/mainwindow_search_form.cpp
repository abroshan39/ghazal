/*
    [Ghazal: The library of persian poetry]
    Publisher: Rosybit
    Url: http://www.rosybit.com
    GitHub: https://github.com/abroshan39/ghazal
    Author: Aboutaleb Roshan [ab.roshan39@gmail.com]
    License: MIT License
*/

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "searchform.h"

#include <QMenu>

void MainWindow::on_btnAdvancedSearch_clicked()
{
    if(!searchAction->isChecked())
        ui->btnSearchForm->toggled(true);

    if(!appSettings.searchSettings.isSearching)
    {
        SearchForm *searchForm = new SearchForm(&appSettings, this);
        connect(searchForm, &SearchForm::sigSearch, this, &MainWindow::slotSearch);
        connect(searchForm, &SearchForm::sigSearchTableChanged, this, &MainWindow::slotSearchTableChanged);
        searchForm->show();
    }
}

void MainWindow::slotSearch()
{
    ui->lineEditSearch->setText(appSettings.searchSettings.searchPhrase);
    on_btnSearch_clicked();
}

void MainWindow::on_btnSearch_clicked()
{
    if(appSettings.searchSettings.isSearching)
    {
        appSettings.inSearchSettings->isSearching = false;  // inSearchSettings.isSearching = false;
        return;
    }

    appSettings.searchSettings.isSearching = true;
    appSettings.searchSettings.searchPhrase = ui->lineEditSearch->text();

    QString strQuery = searchStrQuery(appSettings.mainDB, appSettings.searchSettings.method, appSettings.searchSettings.searchPhrase, appSettings.searchSettings.allItemsSelected, appSettings.searchSettings.poetIDList, appSettings.searchSettings.table, appSettings.searchSettings.skipDiacritics, appSettings.searchSettings.skipCharTypes);

    modelSearch = new QStandardItemModel;
    if(appSettings.searchSettings.showItemsDuringSearch)
        setTableViewModel(modelSearch);
    else
        setTableViewModel(new QStandardItemModel);  // Clear previous results

    Worker *worker = new Worker(Worker::Searcher, &appSettings, modelSearch, strQuery);
    QThread *thread = new QThread;
    worker->moveToThread(thread);

    connect(thread, &QThread::started, worker, &Worker::process);
    connect(worker, &Worker::finished, this, &MainWindow::threadFinished);
    connect(worker, &Worker::finished, thread, &QThread::quit);
    connect(worker, &Worker::finished, worker, &Worker::deleteLater);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);

    thread->start();

    ui->btnSearch->setText("  توقف  ");
    ui->toolButton->setEnabled(false);
    ui->btnAdvancedSearch->setEnabled(false);
    ui->btnExportXML->setEnabled(false);
    ui->lineEditSearch->setReadOnly(true);
    ui->progressBarSearch->show();
}

void MainWindow::threadFinished(Worker::WorkerType type, QVariant result)
{
    if(type == Worker::Searcher)
    {
        qDebug().noquote() << "Worker: Searcher";
        createHistorySearch();

        ui->btnSearch->setText("  جست‌وجو  ");
        ui->toolButton->setEnabled(true);
        ui->btnAdvancedSearch->setEnabled(true);
        ui->btnExportXML->setEnabled(true);
        ui->lineEditSearch->setReadOnly(false);
        ui->progressBarSearch->hide();

        appSettings.searchSettings.isSearching = false;

        if(!appSettings.searchSettings.showItemsDuringSearch)
            setTableViewModel(modelSearch);

        if(!result.toString().isEmpty())
            messageBox("گزارش", result.toString(), Information, this);
    }
}

void MainWindow::createHistorySearch()
{
    {   // History Block
        if(appSettings.searchSettings.table == VerseTable)
            searchHistory.table = "verse";
        else if(appSettings.searchSettings.table == PoemTable)
            searchHistory.table = "poem";
        else if(appSettings.searchSettings.table == CatTable)
            searchHistory.table = "cat";

        searchHistory.poetIDList.clear();
        if(appSettings.searchSettings.allItemsSelected)
        {
            QSqlQuery query("SELECT id FROM poet ORDER BY id");
            while(query.next())
                searchHistory.poetIDList << query.value(0).toString();
        }
        else
        {
            searchHistory.poetIDList = appSettings.searchSettings.poetIDList;
        }

        searchHistory.allItemsSelected = appSettings.searchSettings.allItemsSelected;
        searchHistory.date = nowDate();
        searchHistory.time = nowTime();
        searchHistory.skipDiacritics = appSettings.searchSettings.skipDiacritics;
        searchHistory.skipCharTypes = appSettings.searchSettings.skipCharTypes;
        searchHistory.searchPhrase = appSettings.searchSettings.searchPhrase;
        searchHistory.count = ui->tableView->model()->rowCount();
    }   // History Block
}

void MainWindow::searchRangeMenuCreator()
{
    toolButtonMenu = new QMenu("Menu");
    catAction = new QAction("جستجو در فهرست‌ها");
    poemAction = new QAction("جستجو در عنوان‌ها");
    verseAction = new QAction("جستجو در متن‌ها");

    catAction->setCheckable(true);
    poemAction->setCheckable(true);
    verseAction->setCheckable(true);

    toolButtonMenu->addAction(catAction);
    toolButtonMenu->addAction(poemAction);
    toolButtonMenu->addAction(verseAction);

    ui->toolButton->setMenu(toolButtonMenu);

    connect(catAction, &QAction::triggered, this, &MainWindow::actionCat);
    connect(poemAction, &QAction::triggered, this, &MainWindow::actionPoem);
    connect(verseAction, &QAction::triggered, this, &MainWindow::actionVerse);

    appSettings.searchSettings.table = VerseTable;
    slotSearchTableChanged();
}

void MainWindow::slotSearchTableChanged()
{
    if(appSettings.searchSettings.table == CatTable)
        actionCat();
    else if(appSettings.searchSettings.table == PoemTable)
        actionPoem();
    else if(appSettings.searchSettings.table == VerseTable)
        actionVerse();
}

void MainWindow::actionCat()
{
    appSettings.searchSettings.table = CatTable;
    catAction->setChecked(true);
    poemAction->setChecked(false);
    verseAction->setChecked(false);
}

void MainWindow::actionPoem()
{
    appSettings.searchSettings.table = PoemTable;
    catAction->setChecked(false);
    poemAction->setChecked(true);
    verseAction->setChecked(false);
}

void MainWindow::actionVerse()
{
    appSettings.searchSettings.table = VerseTable;
    catAction->setChecked(false);
    poemAction->setChecked(false);
    verseAction->setChecked(true);
}

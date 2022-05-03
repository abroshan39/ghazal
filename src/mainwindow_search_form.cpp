/*
    [Ghazal: The library of persian poetry]
    Publisher: Rosybit
    Url: http://www.rosybit.com
    GitHub: https://github.com/abroshan39/ghazal
    Version: 1.4
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

    SearchForm *searchForm = new SearchForm(&appSettings, this);
    connect(searchForm, &SearchForm::sigSearch, this, &MainWindow::slotSearch);
    connect(searchForm, &SearchForm::sigSearchTableChanged, this, &MainWindow::slotSearchTableChanged);
    searchForm->show();
}

void MainWindow::slotSearch()
{
    ui->lineEditSearch->setText(appSettings.ss.searchPhrase);
    on_btnSearch_clicked();
}

void MainWindow::on_btnSearch_clicked()
{
    if(appSettings.ss.isSearching)
    {
        appSettings.ss.searchState = false;
        return;
    }

    appSettings.ss.isSearching = true;
    appSettings.ss.searchPhrase = ui->lineEditSearch->text();
    QString strQuery = searchStrQuery(appSettings.mainDB, appSettings.ss.searchPhrase, appSettings.ss.allItemsSelected, appSettings.ss.poetID, appSettings.ss.table, appSettings.ss.skipDiacritics, appSettings.ss.skipCharTypes);

    qDebug().noquote() << strQuery;

    Worker *worker = new Worker(Worker::Searcher, &appSettings, ui->tableWidget, strQuery);
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

        appSettings.ss.isSearching = false;

        if(!result.toString().isEmpty())
            messageBox("گزارش", result.toString(), Information, this);
    }
}

void MainWindow::createHistorySearch()
{
    {   // History Block
        if(appSettings.ss.table == VerseTable)
            searchHistory.table = "verse";
        else if(appSettings.ss.table == PoemTable)
            searchHistory.table = "poem";
        else if(appSettings.ss.table == CatTable)
            searchHistory.table = "cat";

        searchHistory.poetID.clear();
        if(appSettings.ss.allItemsSelected)
        {
            QSqlQuery query("SELECT id FROM poet ORDER BY id");
            while(query.next())
                searchHistory.poetID << query.value(0).toString();
        }
        else
        {
            searchHistory.poetID = appSettings.ss.poetID;
            std::sort(searchHistory.poetID.begin(), searchHistory.poetID.end(), idComp);
        }

        searchHistory.allItemsSelected = appSettings.ss.allItemsSelected;
        searchHistory.date = nowDate();
        searchHistory.time = nowTime();
        searchHistory.skipDiacritics = appSettings.ss.skipDiacritics;
        searchHistory.skipCharTypes = appSettings.ss.skipCharTypes;
        searchHistory.searchPhrase = appSettings.ss.searchPhrase;
        searchHistory.count = ui->tableWidget->rowCount();
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

    appSettings.ss.table = VerseTable;
    slotSearchTableChanged();
}

void MainWindow::slotSearchTableChanged()
{
    if(appSettings.ss.table == CatTable)
        actionCat();
    else if(appSettings.ss.table == PoemTable)
        actionPoem();
    else if(appSettings.ss.table == VerseTable)
        actionVerse();
}

void MainWindow::actionCat()
{
    appSettings.ss.table = CatTable;
    catAction->setChecked(true);
    poemAction->setChecked(false);
    verseAction->setChecked(false);
}

void MainWindow::actionPoem()
{
    appSettings.ss.table = PoemTable;
    catAction->setChecked(false);
    poemAction->setChecked(true);
    verseAction->setChecked(false);
}

void MainWindow::actionVerse()
{
    appSettings.ss.table = VerseTable;
    catAction->setChecked(false);
    poemAction->setChecked(false);
    verseAction->setChecked(true);
}

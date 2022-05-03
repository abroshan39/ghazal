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
#include "tabform.h"
#include "settingsform.h"
#include "databaseform.h"
#include "downloadform.h"
#include "wordsearchform.h"
#include "aboutauthorform.h"
#include "abjadform.h"
#include "aboutform.h"

#include <QMenuBar>
#include <QMenu>
#include <QAction>

#include <QFileDialog>
#include <QFont>
#include <QIcon>
#include <QKeySequence>

void MainWindow::appMenuCreator()
{
    menuBar()->setNativeMenuBar(false);

    QAction *newTabAction;
    QAction *closeTabAction;
    QAction *openAction;
    QAction *exitAction;
    QAction *previousAction;
    QAction *nextAction;
    QAction *searchAdvancedAction;
    QAction *searchInCurrentTabAction;
    QAction *zoomInAction;
    QAction *zoomOutAction;
    QAction *defaultZoomLevelAction;
    QAction *refreshAction;
    QAction *importBookmarkAction;
    QAction *exportBookmarkAction;
    QAction *databaseAction;
    QAction *downloadDBAction;
    QAction *abjadAction;
    QAction *settingsAction;
    QAction *aboutAction;
    QAction *aboutAuthorAction;

    fileMenu = new QMenu(" پرونده ");
    navigationMenu = new QMenu(" ناوبری ");
    searchMenu = new QMenu(" جست‌وجو ");
    viewMenu = new QMenu(" نمایش ");
    viewDisplaySubMenu = new QMenu(" نحوهٔ نمایش ");
    bookmarkMenu = new QMenu(" نشانه‌ها ");
    toolsMenu = new QMenu(" ابزارها ");
    helpMenu = new QMenu(" راهنما ");

    newTabAction = new QAction("برگ جدید");
    closeTabAction = new QAction("بستن برگه");
    openAction = new QAction("باز کردن");
    exitAction = new QAction("خروج");
    previousAction = new QAction("قبل");
    nextAction = new QAction("بعد");
    searchAction = new QAction("صفحه جست‌وجو");
    searchAdvancedAction = new QAction("جست‌وجوی پیشرفته");
    searchInCurrentTabAction = new QAction("جست‌وجو در متن جاری");
    joftAction = new QAction("کنار هم");
    takAction = new QAction("زیر هم");
    zoomInAction = new QAction("بزرگ‌تر کردن قلم");
    zoomOutAction = new QAction("کوچک‌تر کردن قلم");
    defaultZoomLevelAction = new QAction("بزرگنمایی پیش‌فرض");
    refreshAction = new QAction("تازه‌سازی");
    bookmarkAction = new QAction("نشانه‌گذاری");
    bookmarkListAction = new QAction("صفحه نشانه‌ها");
    showBookmarksAction = new QAction("نمایش نشانه‌ها");
    importBookmarkAction = new QAction("ورودی به نشانه‌ها");
    exportBookmarkAction = new QAction("خروجی از نشانه‌ها");
    databaseAction = new QAction("صفحه حذف و اضافه");
    downloadDBAction = new QAction("دانلود از مخزن");
    abjadAction = new QAction("محاسبه‌گر ابجد");
    settingsAction = new QAction("تنظیمات");
    aboutAuthorAction = new QAction("دربارهٔ نویسنده");
    aboutAction = new QAction("دربارهٔ برنامه");

    newTabAction->setShortcut(QKeySequence("Ctrl+T"));
    closeTabAction->setShortcut(QKeySequence("Ctrl+W"));
    openAction->setShortcut(QKeySequence("Ctrl+O"));
    exitAction->setShortcut(QKeySequence("Ctrl+Q"));
    previousAction->setShortcut(QKeySequence("Ctrl+Right"));
    nextAction->setShortcut(QKeySequence("Ctrl+Left"));
    searchAction->setShortcut(QKeySequence("Ctrl+F"));
    searchAdvancedAction->setShortcut(QKeySequence("Ctrl+Shift+F"));
    searchInCurrentTabAction->setShortcut(QKeySequence("F3"));
    joftAction->setShortcut(QKeySequence("Ctrl+1"));
    takAction->setShortcut(QKeySequence("Ctrl+2"));
    zoomInAction->setShortcuts(QList<QKeySequence>() << QKeySequence("Ctrl++") << QKeySequence("Ctrl+="));
    zoomOutAction->setShortcut(QKeySequence("Ctrl+-"));
    defaultZoomLevelAction->setShortcut(QKeySequence("Ctrl+0"));
    refreshAction->setShortcut(QKeySequence("F5"));
    bookmarkAction->setShortcut(QKeySequence("Ctrl+D"));
    bookmarkListAction->setShortcut(QKeySequence("Ctrl+B"));
    showBookmarksAction->setShortcut(QKeySequence("Ctrl+Shift+B"));
    databaseAction->setShortcut(QKeySequence("Alt+D"));
    downloadDBAction->setShortcut(QKeySequence("Alt+R"));
    abjadAction->setShortcut(QKeySequence("Alt+A"));
    settingsAction->setShortcut(QKeySequence("Alt+T"));

    searchAction->setCheckable(true);
    bookmarkAction->setCheckable(true);
    bookmarkListAction->setCheckable(true);
    showBookmarksAction->setCheckable(true);
    joftAction->setCheckable(true);
    takAction->setCheckable(true);

    fileMenu->addAction(newTabAction);
    fileMenu->addAction(closeTabAction);
    fileMenu->addAction(openAction);
    fileMenu->addAction(exitAction);
    navigationMenu->addAction(previousAction);
    navigationMenu->addAction(nextAction);
    searchMenu->addAction(searchAction);
    searchMenu->addAction(searchAdvancedAction);
    searchMenu->addAction(searchInCurrentTabAction);
    viewMenu->addMenu(viewDisplaySubMenu);
    viewDisplaySubMenu->addAction(joftAction);
    viewDisplaySubMenu->addAction(takAction);
    viewMenu->addSeparator();
    viewMenu->addAction(zoomInAction);
    viewMenu->addAction(zoomOutAction);
    viewMenu->addSeparator();
    viewMenu->addAction(defaultZoomLevelAction);
    viewMenu->addSeparator();
    viewMenu->addAction(refreshAction);
    bookmarkMenu->addAction(bookmarkAction);
    bookmarkMenu->addAction(bookmarkListAction);
    bookmarkMenu->addSeparator();
    bookmarkMenu->addAction(showBookmarksAction);
    bookmarkMenu->addSeparator();
    bookmarkMenu->addAction(importBookmarkAction);
    bookmarkMenu->addAction(exportBookmarkAction);
    toolsMenu->addAction(databaseAction);
    toolsMenu->addAction(downloadDBAction);
    toolsMenu->addSeparator();
    toolsMenu->addAction(abjadAction);
    toolsMenu->addSeparator();
    toolsMenu->addAction(settingsAction);
    helpMenu->addAction(aboutAuthorAction);
    helpMenu->addAction(aboutAction);

    menuBar()->addMenu(fileMenu);
    menuBar()->addMenu(navigationMenu);
    menuBar()->addMenu(searchMenu);
    menuBar()->addMenu(viewMenu);
    menuBar()->addMenu(bookmarkMenu);
    menuBar()->addMenu(toolsMenu);
    menuBar()->addMenu(helpMenu);

    connect(newTabAction, &QAction::triggered, this, &MainWindow::actionNewTab);     // connect(newTabAction, SIGNAL(triggered()), this, SLOT(actionNewTab()));
    connect(ui->btnNewTab, &QPushButton::clicked, this, &MainWindow::actionNewTab);  // connect(ui->btnNewTab, SIGNAL(clicked()), this, SLOT(actionNewTab()));

    connect(closeTabAction, &QAction::triggered, this, &MainWindow::actionCloseTab);
    connect(openAction, &QAction::triggered, this, &MainWindow::actionOpen);
    connect(exitAction, &QAction::triggered, this, &MainWindow::actionExit);

    connect(previousAction, &QAction::triggered, this, &MainWindow::actionPrevious);
    connect(ui->btnPrevious, &QPushButton::clicked, this, &MainWindow::actionPrevious);

    connect(nextAction, &QAction::triggered, this, &MainWindow::actionNext);
    connect(ui->btnNext, &QPushButton::clicked, this, &MainWindow::actionNext);

    connect(searchAction, &QAction::toggled, this, &MainWindow::actionSearchToggled);       // connect(searchAction, SIGNAL(toggled(bool)), this, SLOT(actionSearchToggled(bool)));
    connect(searchAction, &QAction::toggled, ui->btnSearchForm, &QPushButton::setChecked);  // connect(searchAction, SIGNAL(toggled(bool)), ui->btnSearchForm, SLOT(setChecked(bool)));
    connect(ui->btnSearchForm, &QPushButton::toggled, searchAction, &QAction::setChecked);  // connect(ui->btnSearchForm, SIGNAL(toggled(bool)), searchAction, SLOT(setChecked(bool)));

    connect(searchAdvancedAction, &QAction::triggered, ui->btnAdvancedSearch, &QPushButton::clicked);
    connect(searchInCurrentTabAction, &QAction::triggered, this, &MainWindow::actionSearchInCurrentTab);

    connect(joftAction, &QAction::triggered, this, &MainWindow::actionJoft);
    connect(takAction, &QAction::triggered, this, &MainWindow::actionTak);

    connect(zoomInAction, &QAction::triggered, this, &MainWindow::actionZoomIn);
    connect(zoomOutAction, &QAction::triggered, this, &MainWindow::actionZoomOut);
    connect(defaultZoomLevelAction, &QAction::triggered, this, &MainWindow::actionDefaultZoomLevel);
    connect(refreshAction, &QAction::triggered, this, &MainWindow::actionRefresh);

    connect(bookmarkAction, &QAction::toggled, this, &MainWindow::actionBookmarkToggled);
    connect(bookmarkAction, &QAction::toggled, ui->checkBoxBookmark, &QCheckBox::setChecked);
    connect(ui->checkBoxBookmark, &QCheckBox::toggled, bookmarkAction, &QAction::setChecked);

    connect(bookmarkListAction, &QAction::toggled, this, &MainWindow::actionBookmarkToggledList);
    connect(bookmarkListAction, &QAction::toggled, ui->btnBookmarkForm, &QPushButton::setChecked);
    connect(ui->btnBookmarkForm, &QPushButton::toggled, bookmarkListAction, &QAction::setChecked);

    connect(showBookmarksAction, &QAction::triggered, this, &MainWindow::actionShowBookmarks);
    connect(importBookmarkAction, &QAction::triggered, this, &MainWindow::actionImportBookmark);
    connect(exportBookmarkAction, &QAction::triggered, this, &MainWindow::actionExportBookmark);
    connect(databaseAction, &QAction::triggered, this, &MainWindow::actionDatabase);
    connect(downloadDBAction, &QAction::triggered, this, &MainWindow::actionDownloadDB);
    connect(abjadAction, &QAction::triggered, this, &MainWindow::actionAbjad);
    connect(settingsAction, &QAction::triggered, this, &MainWindow::actionSettings);
    connect(aboutAuthorAction, &QAction::triggered, this, &MainWindow::actionAboutAuthor);
    connect(aboutAction, &QAction::triggered, this, &MainWindow::actionAbout);

    if(appSettings.pDisplayType == Joft)
    {
        joftAction->setChecked(true);
        takAction->setChecked(false);
    }
    else if(appSettings.pDisplayType == Tak)
    {
        joftAction->setChecked(false);
        takAction->setChecked(true);
    }

    if(appSettings.showBookmarks)
        showBookmarksAction->setChecked(true);
}

void MainWindow::slotAdjustMenuFont()
{
    int size = (int)(appSettings.appFS.toDouble() + 0.5);

    menuBar()->setFont(QFont(appSettings.appFN, size));
    fileMenu->setFont(QFont(appSettings.appFN, size));
    navigationMenu->setFont(QFont(appSettings.appFN, size));
    searchMenu->setFont(QFont(appSettings.appFN, size));
    viewMenu->setFont(QFont(appSettings.appFN, size));
    bookmarkMenu->setFont(QFont(appSettings.appFN, size));
    toolsMenu->setFont(QFont(appSettings.appFN, size));
    helpMenu->setFont(QFont(appSettings.appFN, size));
    toolButtonMenu->setFont(QFont(appSettings.appFN, size));
}

void MainWindow::actionNewTab()
{
    ui->tabWidget->addTab(new TabForm(&appSettings, this), "برگ " + QString::number(ui->tabWidget->count() + 1));
    ui->tabWidget->setCurrentIndex(ui->tabWidget->count() - 1);
    ui->listWidget->setFocus();
}

void MainWindow::actionCloseTab()
{
    on_tabWidget_tabCloseRequested(ui->tabWidget->currentIndex());
}

void MainWindow::actionOpen()
{
    QString filter = "Database files (*.s3db *.db *.sqlite3 *.sqlite *.gdb);;All files (*.*)";
    QString file_name = QFileDialog::getOpenFileName(this, "Open", QDir::homePath(), filter);
    if(!file_name.isEmpty())
    {
        if(isStdGanjoorDB(file_name))
        {
            appSettings.mainDBPath = file_name;
            slotMainDBChanged();
        }
        else
            messageBox("خطا", "<b>خطا</b>:<br />فایل انتخاب‌شده قالب استانداردی ندارد!", Critical, this);
    }
}

void MainWindow::actionExit()
{
    close();
    QCoreApplication::quit();  // QCoreApplication::exit(0);  // qApp->quit();  // qApp->exit(0);

    /*
    void QCoreApplication::quit()
    Tells the application to exit with return code 0 (success).
    Equivalent to calling QCoreApplication::exit(0).

    QApplication is derived from QCoreApplication and thereby inherits quit()
    which is a public slot of QCoreApplication, so there is no difference between
    QApplication::quit() and QCoreApplication::quit()

    qApp
    A global pointer referring to the unique application object. It is
    equivalent to QCoreApplication::instance(), but cast as a QApplication pointer,
    so only valid when the unique application object is a QApplication.
    */
}

void MainWindow::actionPrevious()
{
    QString previous(previousPoem(appSettings.mainDB, appSettings.tabCurrentPoem.value(appSettings.activeTab)));

    if(!previous.isEmpty())
        setContents(appSettings.activeTab, previous);
}

void MainWindow::actionNext()
{
    QString next(nextPoem(appSettings.mainDB, appSettings.tabCurrentPoem.value(appSettings.activeTab)));

    if(!next.isEmpty())
        setContents(appSettings.activeTab, next);
}

void MainWindow::actionSearchToggled(bool checked)
{
    if(checked)
    {
        bookmarkListAction->setChecked(false);
        ui->splitter_2->setSizes({4000, 3000});
        ui->tableWidget->setEnabled(true);
        ui->lineEditSearch->show();
        ui->lineEditSearch->setEnabled(true);
        ui->btnSearch->show();
        ui->btnSearch->setEnabled(true);
        ui->toolButton->show();
        ui->toolButton->setEnabled(true);
        ui->btnAdvancedSearch->show();
        ui->btnAdvancedSearch->setEnabled(true);
        ui->btnExportXML->show();
        ui->btnExportXML->setEnabled(true);
        ui->lineEditSearch->selectAll();
        ui->lineEditSearch->setFocus();
        if(ui->tableWidget->columnCount() == 3)
        {
            ui->tableWidget->model()->removeRows(0, ui->tableWidget->model()->rowCount());
            ui->tableWidget->model()->removeColumns(0, ui->tableWidget->model()->columnCount());
            ui->tableWidget->horizontalHeader()->setDefaultSectionSize(300);
        }
    }
    else
    {
        ui->splitter_2->setSizes({4000, 0});
        ui->tableWidget->setEnabled(false);
        ui->lineEditSearch->setEnabled(false);
        ui->btnSearch->setEnabled(false);
        ui->toolButton->setEnabled(false);
        ui->btnAdvancedSearch->setEnabled(false);
        ui->btnExportXML->setEnabled(false);
    }
}

void MainWindow::actionSearchInCurrentTab()
{
    if(!appSettings.isOpenWordSearchForm)
    {
        WordSearchForm *wordSearchForm = new WordSearchForm(&appSettings, this);
        wordSearchForm->show();
    }
}

void MainWindow::actionJoft()
{
    if(appSettings.pDisplayType != Joft)
    {
        appSettings.pDisplayType = Joft;
        setContents(appSettings.activeTab, appSettings.tabLastLevelID.value(appSettings.activeTab));
    }

    joftAction->setChecked(true);
    takAction->setChecked(false);
}

void MainWindow::actionTak()
{
    if(appSettings.pDisplayType != Tak)
    {
        appSettings.pDisplayType = Tak;
        setContents(appSettings.activeTab, appSettings.tabLastLevelID.value(appSettings.activeTab));
    }

    joftAction->setChecked(false);
    takAction->setChecked(true);
}

void MainWindow::actionZoomIn()
{
    double size = appSettings.viewFSCurrent.toDouble() + 1;
    appSettings.viewFSCurrent = QString::number(size);
    fontSizeChanged(appSettings.activeTab);
    applyStyleSheetListHeader();
}

void MainWindow::actionZoomOut()
{
    double size = appSettings.viewFSCurrent.toDouble() - 1;
    appSettings.viewFSCurrent = QString::number(size);
    fontSizeChanged(appSettings.activeTab);
    applyStyleSheetListHeader();
}

void MainWindow::actionDefaultZoomLevel()
{
    double size = appSettings.viewFS.toDouble();
    appSettings.viewFSCurrent = QString::number(size);
    fontSizeChanged(appSettings.activeTab);
    applyStyleSheetListHeader();
}

void MainWindow::actionRefresh()
{
    setContents(appSettings.activeTab, appSettings.tabLastLevelID.value(appSettings.activeTab));
}

void MainWindow::actionBookmarkToggled(bool checked)
{
    setBookmarked(appSettings.mainDB, appSettings.tabCurrentPoem.value(appSettings.activeTab), "-1", checked);
    if(bookmarkListAction->isChecked())
        tableWidgetBookmark();
}

void MainWindow::actionBookmarkToggledList(bool checked)
{
    if(checked)
    {
        searchAction->setChecked(false);
        ui->splitter_2->setSizes({4000, 3000});
        ui->btnSearchForm->hide();
        ui->btnBookmarkForm->show();
        ui->tableWidget->setEnabled(true);
        ui->lineEditSearch->hide();
        ui->btnSearch->hide();
        ui->toolButton->hide();
        ui->btnAdvancedSearch->hide();
        ui->btnExportXML->hide();
        appSettings.ss.searchPhrase.clear();
    }
    else
    {
        ui->splitter_2->setSizes({4000, 0});
        ui->btnSearchForm->show();
        ui->btnBookmarkForm->hide();
        ui->tableWidget->setEnabled(false);
        return;
    }

    tableWidgetBookmark();
}

void MainWindow::tableWidgetBookmark()
{
    if(fromClickOnTableWidget)
        return;

    ui->tableWidget->model()->removeRows(0, ui->tableWidget->model()->rowCount());
    ui->tableWidget->model()->removeColumns(0, ui->tableWidget->model()->columnCount());

    QSqlQuery query("SELECT poem_id, verse_id FROM fav ORDER BY pos");
    QSqlQuery queryText;

    ui->tableWidget->setColumnCount(3);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QStringList colList;
    colList << "عنوان" << "محدوده" << "متن";
    ui->tableWidget->setHorizontalHeaderLabels(colList);
    ui->tableWidget->setColumnWidth(1, 120);

    int row_count = 0;
    while(query.next())
    {
        ui->tableWidget->insertRow(row_count);

        QTableWidgetItem *item1 = new QTableWidgetItem;
        QTableWidgetItem *item2 = new QTableWidgetItem;
        QTableWidgetItem *item3 = new QTableWidgetItem;
        QString poemID = query.value(0).toString();
        QString verse_id = query.value(1).toString();
        QString vorder = verse_id == "-1" ? "1" : verse_id;
        GanjoorPath gp = recursiveIDs(appSettings.mainDB, "3", poemID);
        int iLast = gp.text.count() - 1;
        QString str = gp.text[iLast] + ": " + gp.text[0];

        queryText.exec(QString("SELECT text FROM verse WHERE poem_id = %1 AND vorder = %2").arg(poemID, vorder));
        queryText.next();

        item1->setText(spaceReplace(str, "…", 6));
        item1->setData(Qt::UserRole, gp.text[iLast]);
        item2->setText(verse_id == "-1" ? "کل متن" : "یک بخش");
        item2->setData(Qt::UserRole, verse_id);
        item3->setText(queryText.value(0).toString());
        item3->setData(Qt::UserRole, "3-" + poemID);

        ui->tableWidget->setItem(row_count, 0, item1);
        ui->tableWidget->setItem(row_count, 1, item2);
        ui->tableWidget->setItem(row_count, 2, item3);

        row_count++;
    }
}

void MainWindow::actionShowBookmarks(bool checked)
{
    appSettings.showBookmarks = checked;
    setContents(appSettings.activeTab, appSettings.tabLastLevelID.value(appSettings.activeTab), false, true);
}

void MainWindow::actionImportBookmark()
{
    QString filter = "Ganjoor Database files (*.gdb);;Ganjoor Database files (*.s3db);;SQLite Database files (*.sqlite);;SQLite 3 Database files (*.sqlite3);;Database files (*.db)";
    QString file_name = QFileDialog::getOpenFileName(this, "Open", QDir::homePath(), filter);

    if(!file_name.isEmpty())
    {
        if(isStdGanjoorDB(file_name, BookmarkDatabase))
            importBookmarks(appSettings.mainDB, file_name, 50000);
        else
            messageBox("خطا", "<b>خطا</b>:<br />در فایل انتخاب‌شده جدول نشانه‌ها یافت نشد!", Critical, this);
    }
}

void MainWindow::actionExportBookmark()
{
    QString filter = "Ganjoor Database files (*.gdb);;Ganjoor Database files (*.s3db);;SQLite Database files (*.sqlite);;SQLite 3 Database files (*.sqlite3);;Database files (*.db)";
    QString db_file_name = QFileDialog::getSaveFileName(this, "Save As", QDir::homePath() + "/exportedBookmarks_" + nowDate("-") + "_" + nowTime("") + ".gdb", filter);

    if(!db_file_name.isEmpty())
    {
        SqliteDB secondDB(db_file_name, "secondDatabase", false);
        exportBookmarks(appSettings.mainDB, secondDB.DB(), 50000);
        secondDB.remove();
    }
}

void MainWindow::actionDatabase()
{
    DatabaseForm *databaseForm = new DatabaseForm(&appSettings, this);
    connect(databaseForm, &DatabaseForm::sigMainDBChanged, this, &MainWindow::slotMainDBChanged);
    connect(databaseForm, &DatabaseForm::sigUpdatePoetList, this, &MainWindow::slotUpdatePoetList);
    databaseForm->show();
}

void MainWindow::actionDownloadDB()
{
    DownloadForm *downloadForm = new DownloadForm(&appSettings, this);
    connect(downloadForm, &DownloadForm::sigMainDBChanged, this, &MainWindow::slotMainDBChanged);
    connect(downloadForm, &DownloadForm::sigUpdatePoetList, this, &MainWindow::slotUpdatePoetList);
    downloadForm->show();
}

void MainWindow::actionAbjad()
{
    if(!appSettings.isOpenAbjadForm)
    {
        AbjadForm *abjadForm = new AbjadForm(&appSettings, this);
        connect(this, &MainWindow::sigSelectedText, abjadForm, &AbjadForm::slotSelectedText);
        abjadForm->show();
    }
}

void MainWindow::actionSettings()
{
    SettingsForm *settingsForm = new SettingsForm(&appSettings, this);
    connect(settingsForm, &SettingsForm::sigAdjustMenuFont, this, &MainWindow::slotAdjustMenuFont);
    connect(settingsForm, &SettingsForm::sigMainDBChanged, this, &MainWindow::slotMainDBChanged);
    connect(settingsForm, SIGNAL(sigTabFormSize()), ui->tabWidget->currentWidget(), SLOT(slotTabFormSize()));
    connect(settingsForm, SIGNAL(sigTabTheme()), ui->tabWidget->currentWidget(), SLOT(slotTabTheme()));
    settingsForm->show();
}

void MainWindow::actionAboutAuthor()
{
    AboutAuthorForm *aboutAuthorForm = new AboutAuthorForm(&appSettings, this);
    aboutAuthorForm->show();
}

void MainWindow::actionAbout()
{
    AboutForm *aboutForm = new AboutForm(&appSettings, this);
    aboutForm->show();
}

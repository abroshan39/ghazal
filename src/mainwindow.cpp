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
#include "appthemes.h"

#include <QXmlStreamWriter>
#include <QClipboard>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle(Constants::GhazalFa + ": کتابخانه شعر فارسی");
    setWindowIcon(QIcon(":/files/images/ghazal-256x256.png"));
    QApplication::setLayoutDirection(Qt::RightToLeft);

    loadDefaultFonts();
    readSettings();
    appMenuCreator();
    searchRangeMenuCreator();
    slotMainDBChanged();
    readHistory();
    startup();
    applyStyleSheet();
    checkDBExist();
    tabHeaderLabel();
    lineEditDirectionCorrector(ui->lineEditPoet);
    lineEditDirectionCorrector(ui->lineEditSearch);

    clipboard = QApplication::clipboard();
    connect(clipboard, &QClipboard::dataChanged, this, &MainWindow::changeTextCopiedToCB);
    isClipboardConnect = true;

    connect(qApp, &QGuiApplication::applicationStateChanged, this, &MainWindow::appStateChanged);
    connect(new KeyPress(ui->listWidget), &KeyPress::keyPressed, this, &MainWindow::listWidgetKeyPressed);
    connect(new KeyPress(ui->tableView), &KeyPress::keyPressed, this, &MainWindow::tableViewKeyPressed);
    connect(new KeyPress(ui->lineEditSearch), &KeyPress::keyPressed, this, &MainWindow::lineEditSearchKeyPressed);
    connect(new FocusWatcher(ui->lineEditPoet), &FocusWatcher::focusChanged, this, &MainWindow::lineEditsFocusChanged);
    connect(new FocusWatcher(ui->lineEditSearch), &FocusWatcher::focusChanged, this, &MainWindow::lineEditsFocusChanged);
    connect((zwnjPoet = new ZWNJPress(ui->lineEditPoet)), &ZWNJPress::zwnjPressed, this, &MainWindow::lineEditsZWNJPressed);
    connect((zwnjSearch = new ZWNJPress(ui->lineEditSearch)), &ZWNJPress::zwnjPressed, this, &MainWindow::lineEditsZWNJPressed);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent *e)
{
    if(e->key() == Qt::Key_Escape)
    {
        if(searchAction->isChecked())
            searchAction->setChecked(false);
        if(bookmarkListAction->isChecked())
            bookmarkListAction->setChecked(false);
    }
}

void MainWindow::closeEvent(QCloseEvent *e)
{
    Q_UNUSED(e);  // (void)e;

    writeSettings();
    if(appSettings.saveHistoryOnExit)
        writeHistory();
    dbCloseRemove(appSettings.mainDB);
}

void MainWindow::dragEnterEvent(QDragEnterEvent *e)
{
    if(e->mimeData()->hasUrls())
        e->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent *e)
{
    QString dropFilePath;
    for(int i = 0; i < e->mimeData()->urls().count(); i++)
    {
        dropFilePath = e->mimeData()->urls().at(i).toLocalFile();
        break;  // Only first item
    }

    if(QFileInfo(dropFilePath).isFile())
    {
        if(isStdGanjoorDB(dropFilePath))
        {
            appSettings.mainDBPath = dropFilePath;
            slotMainDBChanged();
        }
        else
        {
            messageBox("خطا", "<b>خطا</b>:<br />فایل انتخاب‌شده قالب استانداردی ندارد!", Critical, this);
        }
    }
}

void MainWindow::appStateChanged(Qt::ApplicationState state)
{
    if(state == Qt::ApplicationActive)
    {
        if(!isClipboardConnect)
        {
            qDebug().noquote() << QString("%1 connected").arg(Constants::Ghazal);
            connect(clipboard, &QClipboard::dataChanged, this, &MainWindow::changeTextCopiedToCB);
            isClipboardConnect = true;
        }
    }
    else if(state == Qt::ApplicationInactive)
    {
        if(isClipboardConnect)
        {
            qDebug().noquote() << QString("%1 disconnected").arg(Constants::Ghazal);
            disconnect(clipboard, &QClipboard::dataChanged, this, &MainWindow::changeTextCopiedToCB);
            isClipboardConnect = false;
        }
    }
}

void MainWindow::changeTextCopiedToCB()
{
    QString text = correctHtmlTableText(clipboard->text());
    if(clipboard->text() != text)
        clipboard->setText(text);
}

void MainWindow::setContents(QWidget *ptrTab, const QString &levelID, bool setFocusListWidget, bool rememberScrollBarValue, const QStringList &highlightText, const QString &vorder, bool highlightVorder, bool highlightWithUnderline)
{
    connect(this, SIGNAL(sigSetTabContent(QString, bool, bool, QStringList, QString, bool, bool)), ptrTab, SLOT(slotSetTabContent(QString, bool, bool, QStringList, QString, bool, bool)));
    emit sigSetTabContent(levelID, setFocusListWidget, rememberScrollBarValue, highlightText, vorder, highlightVorder, highlightWithUnderline);
    disconnect(this, SIGNAL(sigSetTabContent(QString, bool, bool, QStringList, QString, bool, bool)), ptrTab, SLOT(slotSetTabContent(QString, bool, bool, QStringList, QString, bool, bool)));
}

void MainWindow::on_listWidget_doubleClicked(const QModelIndex &index)
{
    poetSelected(index);
}

void MainWindow::on_tableView_doubleClicked(const QModelIndex &index)
{
    if(searchAction->isChecked())
    {
        QString poetName = index.sibling(index.row(), 0).data(Qt::UserRole).toString();
        QString levelIDVorder = index.sibling(index.row(), 1).data(Qt::UserRole).toString();
        QStringList searchList = textListHighlight(appSettings.searchSettings.searchPhrase);
        QStringList list(levelIDVorder.split("-"));
        QString levelID;
        QString vorder;

        if(list.count() < 3)
        {
            levelID = list.join("-");
        }
        else
        {
            levelID = list[0] + "-" + list[1];
            vorder = list[2];
        }

        ui->tabWidget->setTabText(ui->tabWidget->currentIndex(), "  " + poetName + "  ");
        setContents(appSettings.activeTab, levelID, false, false, searchList, vorder, true, false);
    }
    else if(bookmarkListAction->isChecked())
    {
        QString poetName = index.sibling(index.row(), 0).data(Qt::UserRole).toString();
        QString verse_id = index.sibling(index.row(), 1).data(Qt::UserRole).toString();
        QString levelID = index.sibling(index.row(), 2).data(Qt::UserRole).toString();

        fromClickOnTable = true;
        ui->tabWidget->setTabText(ui->tabWidget->currentIndex(), "  " + poetName + "  ");
        if(verse_id == "-1")
            setContents(appSettings.activeTab, levelID);
        else
            setContents(appSettings.activeTab, levelID, false, false, QStringList(), verse_id, true);
        fromClickOnTable = false;
    }

    ui->tableView->setFocus();
}

void MainWindow::poetSelected(const QModelIndex &index, bool setFocusListWidget)
{
    QString poetName = index.data().toString();
    QString poetID = index.data(Qt::UserRole).toString();

    QSqlQuery query("SELECT cat_id FROM poet WHERE id = " + poetID);
    query.next();

    ui->tabWidget->setTabText(ui->tabWidget->currentIndex(), "  " + poetName + "  ");
    setContents(ui->tabWidget->currentWidget(), "1-" + query.value(0).toString(), setFocusListWidget);
}

void MainWindow::on_tabWidget_tabCloseRequested(int index)
{
    QWidget *tabWidget = ui->tabWidget->widget(index);

    appSettings.tabLastLevelID.remove(tabWidget);
    appSettings.tabCurrentPoem.remove(tabWidget);
    tabFontSize.remove(tabWidget);
    tabIsDarkMode.remove(tabWidget);

    if(ui->tabWidget->count() != 1)
    {
        ui->tabWidget->removeTab(index);
    }
    else
    {
        actionNewTab();
        tabWidget = ui->tabWidget->widget(0);
        ui->tabWidget->removeTab(0);
        ui->tabWidget->setTabText(0, "برگ 1");
    }

    if(tabWidget != nullptr)
        tabWidget->deleteLater();
}

void MainWindow::on_splitter_2_splitterMoved(int pos, int index)
{
    Q_UNUSED(pos);    // (void)pos;
    Q_UNUSED(index);  // (void)index;

    QList<int> sSizes = ui->splitter_2->sizes();

    if(sSizes[1] && !searchAction->isChecked())
    {
        if(!bookmarkListAction->isChecked())
            searchAction->setChecked(true);
    }
    else if(!sSizes[1] && (searchAction->isChecked() || bookmarkListAction->isChecked()))
    {
        bookmarkListAction->setChecked(false);
        searchAction->setChecked(false);

        ui->btnSearchForm->show();
        ui->btnBookmarkForm->hide();
    }
}

void MainWindow::on_lineEditPoet_textChanged(const QString &arg1)
{
    listWidgetPoetList(ui->listWidget, appSettings.mainDB, false, "SELECT name, id, cat_id FROM poet WHERE name LIKE '%" + arg1 + "%'");
}

void MainWindow::slotUpdatePoetList()
{
    listWidgetPoetList(ui->listWidget, appSettings.mainDB, false, "SELECT name, id, cat_id FROM poet WHERE name LIKE '%" + ui->lineEditPoet->text() + "%'");
}

void MainWindow::on_tabWidget_currentChanged(int index)
{
    appSettings.activeTab = ui->tabWidget->widget(index);

    checkBookmark();

    if(historyOnLoad.contains(appSettings.activeTab))
    {
        setContents(appSettings.activeTab, historyOnLoad.value(appSettings.activeTab));
        historyOnLoad.remove(appSettings.activeTab);
    }

    if(tabFontSize.value(appSettings.activeTab) != appSettings.viewFSCurrent)
    {
        fontSizeChanged(appSettings.activeTab);
        tabFontSize.insert(appSettings.activeTab, appSettings.viewFSCurrent);
    }

    if(tabIsDarkMode.value(appSettings.activeTab) != appSettings.isDarkMode)
    {
        themeChanged();
        tabIsDarkMode.insert(appSettings.activeTab, appSettings.isDarkMode);
    }
}

void MainWindow::slotMainDBChanged()
{
    dbCloseRemove(appSettings.mainDB);
    SqliteDB sqliteDB(appSettings.mainDBPath, "", true);
    appSettings.mainDB = sqliteDB.DB();

    listWidgetPoetList(ui->listWidget, appSettings.mainDB);
    ui->lineEditPoet->clear();
}

void MainWindow::slotTabLastLevelIDChanged()
{
    checkBookmark();
}

void MainWindow::slotSelectedText(const QString &text)
{
    if(appSettings.isOpenAbjadForm || appSettings.isOpenAbjadFormMini)
        emit sigSelectedText(text);
}

void MainWindow::themeChanged()
{
    connect(this, SIGNAL(sigTabTheme()), appSettings.activeTab, SLOT(slotTabTheme()));
    emit sigTabTheme();
    disconnect(this, SIGNAL(sigTabTheme()), appSettings.activeTab, SLOT(slotTabTheme()));
}

void MainWindow::fontSizeChanged(QWidget *ptrTab)
{
    connect(this, SIGNAL(sigTabFormSize()), ptrTab, SLOT(slotTabFormSize()));
    emit sigTabFormSize();
    disconnect(this, SIGNAL(sigTabFormSize()), ptrTab, SLOT(slotTabFormSize()));
}

void MainWindow::tabHeaderLabel()
{
    connect(this, SIGNAL(sigTabHeaderLabel()), appSettings.activeTab, SLOT(slotTabHeaderLabel()));
    emit sigTabHeaderLabel();
    disconnect(this, SIGNAL(sigTabHeaderLabel()), appSettings.activeTab, SLOT(slotTabHeaderLabel()));
}

void MainWindow::checkBookmark()
{
    if(appSettings.tabCurrentPoem.value(appSettings.activeTab).left(1) != "3")
    {
        bookmarkAction->setChecked(false);
        bookmarkAction->setEnabled(false);
        ui->checkBoxBookmark->setEnabled(false);
        return;
    }
    else
    {
        bookmarkAction->setEnabled(true);
        ui->checkBoxBookmark->setEnabled(true);
    }

    bool value = isBookmarked(appSettings.mainDB, appSettings.tabCurrentPoem.value(appSettings.activeTab), "-1");
    bookmarkAction->setChecked(value);
}

void MainWindow::slotBookmarkChanged()
{
    checkBookmark();
    if(bookmarkListAction->isChecked())
        tableBookmark();
}

void MainWindow::on_checkBoxDarkMode_clicked(bool checked)
{
    if(checked)
        appSettings.isDarkMode = true;
    else
        appSettings.isDarkMode = false;
    applyStyleSheet();

    themeChanged();
}

void MainWindow::setTableViewModel(QStandardItemModel *model)
{
    QItemSelectionModel *m = ui->tableView->selectionModel();
    ui->tableView->setModel(model);
    if(m)
        delete m;  // IMPORTANT
}

void MainWindow::listWidgetKeyPressed(QObject *object, QKeyEvent *event)
{
    Q_UNUSED(object);  // (void)object;

    int key = event->key();
    if(key == Qt::Key_Return || key == Qt::Key_Enter)
        poetSelected(ui->listWidget->currentIndex());
    else if(key == Qt::Key_Space && ui->listWidget->count() && ui->listWidget->currentRow() < 0)
        ui->listWidget->setCurrentRow(0);
    else if(key == Qt::Key_Left)
        poetSelected(ui->listWidget->currentIndex(), true);
}

void MainWindow::tableViewKeyPressed(QObject *object, QKeyEvent *event)
{
    Q_UNUSED(object);  // (void)object;
    if(event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
        on_tableView_doubleClicked(ui->tableView->currentIndex());
}

void MainWindow::lineEditSearchKeyPressed(QObject *object, QKeyEvent *event)
{
    Q_UNUSED(object);  // (void)object;
    if(event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
    {
        if(ui->lineEditSearch->isEnabled() && !ui->lineEditSearch->isReadOnly())
            on_btnSearch_clicked();
    }
}

void MainWindow::lineEditsZWNJPressed(QObject *object, Qt::KeyboardModifier key)
{
    Q_UNUSED(key);  // (void)key;
    static_cast<QLineEdit *>(object)->insert(Constants::ZWNJ);
}

void MainWindow::lineEditsFocusChanged(QObject *object, QEvent *event)
{
    QLineEdit *lineEdit = static_cast<QLineEdit *>(object);
    if(lineEdit == ui->lineEditPoet && event->type() == QEvent::FocusIn)
        zwnjPoet->clear();
    else if(lineEdit == ui->lineEditSearch && event->type() == QEvent::FocusIn)
        zwnjSearch->clear();
}

void MainWindow::on_btnExportXML_clicked()
{
    if(!ui->tableView->model()->rowCount())
        return;

    QString file_path = QFileDialog::getSaveFileName(this, "Save As", QDir::homePath() + "/" + Constants::Ghazal + "_SearchResult_" + QString(searchHistory.date).replace("/", "-") + "_" + QString(searchHistory.time).replace(":", "") + ".xml", "XML files (*.xml)");
    if(file_path.isEmpty())
    {
        messageBox("خطا", "لطفا مسیر فایل خروجی را درست انتخاب کنید!.", Warning, this);
        return;
    }

    QFile file(file_path);
    file.open(QIODevice::WriteOnly);

    QXmlStreamWriter stream(&file);
#if QT_VERSION < 0x060000
    stream.setCodec("UTF-8");
#endif
    stream.setAutoFormatting(true);
    stream.setAutoFormattingIndent(4);
    stream.writeStartDocument("1.0");

    stream.writeStartElement(Constants::Ghazal);
    stream.writeTextElement("Version", Constants::GhazalVersion);
    stream.writeStartElement("SearchResult");
    stream.writeTextElement("Date", searchHistory.date);
    stream.writeTextElement("Time", searchHistory.time);
    stream.writeTextElement("Table", searchHistory.table);
    stream.writeTextElement("SkipDiacritics", QVariant(searchHistory.skipDiacritics).toString());
    stream.writeTextElement("SkipCharTypes", QVariant(searchHistory.skipCharTypes).toString());

    stream.writeStartElement("SearchRange");
    if(searchHistory.allItemsSelected)
        stream.writeAttribute("Summary", "AllItems");
    else
        stream.writeAttribute("Summary", "SelectedItems");
    stream.writeAttribute("Count", QString::number(searchHistory.poetIDList.count()));
    for(int i = 0; i < searchHistory.poetIDList.count(); i++)
    {
        stream.writeStartElement("Item");
        stream.writeAttribute("ID", searchHistory.poetIDList.at(i));
        stream.writeCharacters(getPoetNameByPoetID(appSettings.mainDB, searchHistory.poetIDList.at(i)));
        stream.writeEndElement();  // Item
    }
    stream.writeEndElement();  // SearchRange

    stream.writeTextElement("SearchPhrase", searchHistory.searchPhrase);
    stream.writeTextElement("Count", QString::number(searchHistory.count));
    stream.writeStartElement("Results");

    for(int i = 0; i < ui->tableView->model()->rowCount(); i++)
    {
        stream.writeStartElement("Item");
        stream.writeAttribute("ID", ui->tableView->model()->data(ui->tableView->model()->index(i, 1), Qt::UserRole).toString());
        stream.writeAttribute("Name", ui->tableView->model()->data(ui->tableView->model()->index(i, 0), Qt::UserRole).toString());
        stream.writeCharacters(ui->tableView->model()->data(ui->tableView->model()->index(i, 1), Qt::DisplayRole).toString());
        stream.writeEndElement();  // Item
    }

    stream.writeEndElement();  // Results
    stream.writeEndElement();  // SearchResult
    stream.writeEndElement();  // Ghazal
    stream.writeEndDocument();
    file.close();
}

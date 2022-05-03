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
#include "appthemes.h"
#include "event_functions.h"

#include <QXmlStreamWriter>
#include <QClipboard>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle(Constants::AppNameFa + ": کتابخانه شعر فارسی");
    setWindowIcon(QIcon(":/files/images/ghazal-256x256.png"));
    QApplication::setLayoutDirection(Qt::RightToLeft);

    loadDefaultFonts();
    readSettings();
    appMenuCreator();
    searchRangeMenuCreator();
    slotMainDBChanged();
    readHistory();
    widgetsStartup();
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
    connect(new KeyPress(ui->tableWidget), &KeyPress::keyPressed, this, &MainWindow::tableWidgetKeyPressed);
    connect(new KeyPress(ui->lineEditSearch), &KeyPress::keyPressed, this, &MainWindow::lineEditSearchKeyPressed);
    connect(new ZWNJPress(ui->lineEditSearch), &ZWNJPress::zwnjPressed, this, &MainWindow::lineEditsZWNJPressed);
    connect(new ZWNJPress(ui->lineEditPoet), &ZWNJPress::zwnjPressed, this, &MainWindow::lineEditsZWNJPressed);
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

void MainWindow::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);  // (void)event;

    writeSettings();
    writeHistory();
    dbCloseRemove(appSettings.mainDB);
}

void MainWindow::appStateChanged(Qt::ApplicationState state)
{
    if(state == Qt::ApplicationActive)
    {
        if(!isClipboardConnect)
        {
            qDebug().noquote() << QString("%1 connected").arg(Constants::AppName);
            connect(clipboard, &QClipboard::dataChanged, this, &MainWindow::changeTextCopiedToCB);
            isClipboardConnect = true;
        }
    }
    else if(state == Qt::ApplicationInactive)
    {
        if(isClipboardConnect)
        {
            qDebug().noquote() << QString("%1 disconnected").arg(Constants::AppName);
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

void MainWindow::setContents(QWidget *ptrTab, const QString &levelID, bool setFocusListWidget, bool rememberScrollBarValue, const QStringList &highlightText, const QString &bookmarkVerseID)
{
    connect(this, SIGNAL(sigSetTabContent(QString, bool, bool, QStringList, QString)), ptrTab, SLOT(slotSetTabContent(QString, bool, bool, QStringList, QString)));
    emit sigSetTabContent(levelID, setFocusListWidget, rememberScrollBarValue, highlightText, bookmarkVerseID);
    disconnect(this, SIGNAL(sigSetTabContent(QString, bool, bool, QStringList, QString)), ptrTab, SLOT(slotSetTabContent(QString, bool, bool, QStringList, QString)));
}

void MainWindow::on_listWidget_doubleClicked(const QModelIndex &index)
{
    poetSelected(index);
}

void MainWindow::on_tableWidget_doubleClicked(const QModelIndex &index)
{
    if(searchAction->isChecked())
    {
        QString poetName = index.sibling(index.row(), 0).data(Qt::UserRole).toString();
        QString id = index.sibling(index.row(), 1).data(Qt::UserRole).toString();
        QStringList searchList = textListHighlight(appSettings.ss.searchPhrase);

        ui->tabWidget->setTabText(ui->tabWidget->currentIndex(), "  " + poetName + "  ");
        setContents(appSettings.activeTab, id, false, false, searchList);
    }
    else if(bookmarkListAction->isChecked())
    {
        QString poetName = index.sibling(index.row(), 0).data(Qt::UserRole).toString();
        QString verse_id = index.sibling(index.row(), 1).data(Qt::UserRole).toString();
        QString id = index.sibling(index.row(), 2).data(Qt::UserRole).toString();

        fromClickOnTableWidget = true;
        ui->tabWidget->setTabText(ui->tabWidget->currentIndex(), "  " + poetName + "  ");
        if(verse_id == "-1")
            setContents(appSettings.activeTab, id);
        else
            setContents(appSettings.activeTab, id, false, false, QStringList(), verse_id);
        fromClickOnTableWidget = false;
    }

    ui->tableWidget->setFocus();
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
        tableWidgetBookmark();
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

void MainWindow::tableWidgetKeyPressed(QObject *object, QKeyEvent *event)
{
    Q_UNUSED(object);  // (void)object;
    if(event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
        on_tableWidget_doubleClicked(ui->tableWidget->currentIndex());
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

void MainWindow::on_btnExportXML_clicked()
{
    if(!ui->tableWidget->rowCount())
        return;

    QString file_path = QFileDialog::getSaveFileName(this, "Save As", QDir::homePath() + "/" + Constants::AppName + "_SearchResult_" + QString(searchHistory.date).replace("/", "-") + "_" + QString(searchHistory.time).replace(":", "") + ".xml", "XML files (*.xml)");
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

    stream.writeStartElement(Constants::AppName + "App");
    stream.writeTextElement("Version", Constants::AppVersion);
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
    stream.writeAttribute("Count", QString::number(searchHistory.poetID.count()));
    for(int i = 0; i < searchHistory.poetID.count(); i++)
    {
        stream.writeStartElement("Item");
        stream.writeAttribute("ID", searchHistory.poetID.at(i));
        stream.writeCharacters(getPoetNameByPoetID(appSettings.mainDB, searchHistory.poetID.at(i)));
        stream.writeEndElement();  // Item
    }
    stream.writeEndElement();  // SearchRange

    stream.writeTextElement("SearchPhrase", searchHistory.searchPhrase);
    stream.writeTextElement("Count", QString::number(searchHistory.count));
    stream.writeStartElement("Results");

    for(int i = 0; i < ui->tableWidget->rowCount(); i++)
    {
        stream.writeStartElement("Item");
        stream.writeAttribute("ID", ui->tableWidget->item(i, 1)->data(Qt::UserRole).toString());
        stream.writeAttribute("Name", ui->tableWidget->item(i, 0)->data(Qt::UserRole).toString());
        stream.writeCharacters(ui->tableWidget->item(i, 1)->data(Qt::DisplayRole).toString());
        stream.writeEndElement();  // Item
    }

    stream.writeEndElement();  // Results
    stream.writeEndElement();  // SearchResult
    stream.writeEndElement();  // GhazalApp
    stream.writeEndDocument();
    file.close();
}

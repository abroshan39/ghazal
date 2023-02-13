/*
    [Ghazal: The library of persian poetry]
    Publisher: Rosybit
    Url: http://www.rosybit.com
    GitHub: https://github.com/abroshan39/ghazal
    Author: Aboutaleb Roshan [ab.roshan39@gmail.com]
    License: MIT License
*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenu>
#include <QAction>
#include <QClipboard>
#include "common_functions.h"
#include "event_functions.h"
#include "worker.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void sigSetTabContent(const QString &levelID, bool setFocusListWidget, bool rememberScrollBarValue, const QStringList &highlightText, const QString &vorder, bool highlightVorder, bool highlightWithUnderline);
    void sigTabFormSize();
    void sigTabTheme();
    void sigTabHeaderLabel();
    void sigSelectedText(const QString &text);
    void stopSearch();

public slots:
    void appStateChanged(Qt::ApplicationState state);
    void changeTextCopiedToCB();
    void slotUpdatePoetList();
    void slotMainDBChanged();
    void slotTabLastLevelIDChanged();
    void slotThemeAndMenuFont();
    void slotSelectedText(const QString &text);
    void slotSearchTableChanged();
    void slotSearch();
    void setTableViewModel(QStandardItemModel *model);
    void poetSelected(const QModelIndex &index, bool setFocusListWidget = false);
    void themeChanged();
    void fontSizeChanged(QWidget *ptrTab);
    void tabHeaderLabel();
    void checkBookmark();
    void slotBookmarkChanged();
    void setContents(QWidget *ptrTab, const QString &levelID, bool setFocusListWidget = false, bool rememberScrollBarValue = false, const QStringList &highlightText = QStringList(), const QString &vorder = QString(), bool highlightVorder = false, bool highlightWithUnderline = true);
    void threadFinished(Worker::WorkerType type, QVariant result);
    void createHistorySearch();
    void loadDefaultFonts();
    void writeSettings();
    void readSettings();
    void writeHistory();
    void readHistory();
    void startup();
    void applyStyleSheet();
    void checkDBExist();
    void applyStyleSheetListHeader();
    void lineEditsZWNJPressed(QObject *object, Qt::KeyboardModifier key);
    void lineEditsFocusChanged(QObject *object, QEvent *event);
    void listWidgetKeyPressed(QObject *object, QKeyEvent *event);
    void tableViewKeyPressed(QObject *object, QKeyEvent *event);
    void lineEditSearchKeyPressed(QObject *object, QKeyEvent *event);
    void appMenuCreator();
    void searchRangeMenuCreator();
    void tableBookmark();
    void actionNewTab();
    void actionCloseTab();
    void actionOpen();
    void actionSave();
    void actionExit();
    void actionPrevious();
    void actionNext();
    void actionSearchToggled(bool checked);
    void actionSearchInCurrentTab();
    void actionJoft();
    void actionTak();
    void actionZoomIn();
    void actionZoomOut();
    void actionDefaultZoomLevel();
    void actionRefresh();
    void actionRefreshPos();
    void actionBookmarkToggled(bool checked);
    void actionBookmarkToggledList(bool checked);
    void actionShowBookmarks(bool checked);
    void actionImportBookmark();
    void actionExportBookmark();
    void actionDatabase();
    void actionDownloadDB();
    void actionAbjad();
    void actionSettings();
    void actionUpdateCheck();
    void actionAboutAuthor();
    void actionAbout();
    void actionCat();
    void actionPoem();
    void actionVerse();

private slots:
    void on_listWidget_doubleClicked(const QModelIndex &index);
    void on_lineEditPoet_textChanged(const QString &arg1);
    void on_tableView_doubleClicked(const QModelIndex &index);
    void on_tabWidget_currentChanged(int index);
    void on_tabWidget_tabCloseRequested(int index);
    void on_splitter_2_splitterMoved(int pos, int index);
    void on_checkBoxDarkMode_clicked(bool checked);
    void on_btnSearch_clicked();
    void on_btnAdvancedSearch_clicked();
    void on_btnExportXML_clicked();

protected:
    void keyPressEvent(QKeyEvent *e) override;
    void closeEvent(QCloseEvent *e) override;
    void dragEnterEvent(QDragEnterEvent *e) override;
    void dropEvent(QDropEvent *e) override;

private:
    Ui::MainWindow *ui;
    AppSettings appSettings;
    InSearchSettings inSearchSettings;
    SearchHistory searchHistory;
    QClipboard *clipboard;
    QStandardItemModel *modelSearch;
    bool isClipboardConnect;
    bool fromClickOnTable = false;

    QMap<QWidget *, QString> historyOnLoad;
    QMap<QWidget *, QString> tabFontSize;
    QMap<QWidget *, bool> tabIsDarkMode;

    ZWNJPress *zwnjPoet;
    ZWNJPress *zwnjSearch;

    QMenu *fileMenu;
    QMenu *navigationMenu;
    QMenu *searchMenu;
    QMenu *viewMenu;
    QMenu *viewDisplaySubMenu;
    QMenu *bookmarkMenu;
    QMenu *toolsMenu;
    QMenu *helpMenu;
    QMenu *toolButtonMenu;
    QAction *searchAction;
    QAction *joftAction;
    QAction *takAction;
    QAction *bookmarkAction;
    QAction *bookmarkListAction;
    QAction *showBookmarksAction;
    QAction *catAction;
    QAction *poemAction;
    QAction *verseAction;
};
#endif // MAINWINDOW_H

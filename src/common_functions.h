/*
    [Ghazal: The library of persian poetry]
    Publisher: Rosybit
    Url: http://www.rosybit.com
    GitHub: https://github.com/abroshan39/ghazal
    Version: 1.4
    Author: Aboutaleb Roshan [ab.roshan39@gmail.com]
    License: MIT License
*/

#ifndef COMMON_FUNCTIONS_H
#define COMMON_FUNCTIONS_H

#include "date_converter.h"
#include <cmath>

#include <QApplication>
#include <QtGlobal>
#include <QEvent>
#include <QCloseEvent>
#include <QKeyEvent>

#include <QFuture>
#include <QtConcurrent>

#include <QDate>
#include <QTime>

#include <QDesktopServices>
#include <QStyle>
#include <QScreen>

#include <QFileDialog>
#include <QListWidget>
#include <QTableWidget>
#include <QHeaderView>
#include <QPushButton>
#include <QLineEdit>

#if QT_VERSION < 0x060000
#include <QRegExp>
#endif
#include <QRegularExpression>
#include <QUuid>
#include <QString>
#include <QtXml>

#include <QSqlDatabase>
#include <QSqlQuery>

#include <QMessageBox>
#include <QDebug>

// A preprocessor variable to support both Qt5 and Qt6
#if QT_VERSION >= 0x060000
#define SKIP_EMPTY_PARTS Qt::SkipEmptyParts
#else
#define SKIP_EMPTY_PARTS QString::SkipEmptyParts
#endif

/*
QT_VERSION:
This macro expands a numeric value of the
form 0xMMNNPP (MM = major, NN = minor, PP = patch) that
specifies Qt's version number. For example,
if you compile your application against Qt 4.1.2,
the QT_VERSION macro will expand to 0x040102.
Qt 4.1.2  = 0x040102
Qt 5.10.0 = 0x050A00
Qt 5.15.2 = 0x050F02
*/

struct Constants
{
    static const QString Rosybit;
    static const QString RosybitFa;
    static const QString RosybitUrl;
    static const QString Programmer;
    static const QString ProgrammerFa;
    static const QString Email;
    static const QString AppName;
    static const QString AppNameFa;
    static const QString AppVersion;
    static const QString GitHub;
    static const QString AppUrl;
    static const QString DefaultDBName;
    static const QString SettingsFileName;
    static const QString HistoryFileName;

    static const QChar ZWNJ;
    static const QString DIACRITICS;
    static const QString A_PERSIAN;
    static const QString E_PERSIAN;
    static const QString K_PERSIAN;
    static const QString V_PERSIAN;
    static const QString H_PERSIAN;
    static const QString A_TYPES;
    static const QString E_TYPES;
    static const QString K_TYPES;
    static const QString V_TYPES;
    static const QString H_TYPES;

    static const QRegularExpression ZWNJ_REGEX;
    static const QRegularExpression DIACRITICS_REGEX;
    static const QRegularExpression A_TYPES_REGEX;
    static const QRegularExpression E_TYPES_REGEX;
    static const QRegularExpression K_TYPES_REGEX;
    static const QRegularExpression V_TYPES_REGEX;
    static const QRegularExpression H_TYPES_REGEX;
    static const QRegularExpression A_REGEX;
    static const QRegularExpression E_REGEX;
    static const QRegularExpression K_REGEX;
    static const QRegularExpression V_REGEX;
    static const QRegularExpression H_REGEX;
};

enum DatabaseType
{
    MainDatabase,
    BookmarkDatabase
};

enum MessageBoxType
{
    Information,
    Warning,
    Critical,
    Question,
    WarningQuestion,
    WarningQuestionToAll
};

enum PoemDisplayType
{
    Joft,
    Tak
};

enum TableDisplayType
{
    AllItems,
    PreInstalledItems,
    NotInstalledItems
};

enum SearchTable
{
    VerseTable,
    PoemTable,
    CatTable
};

struct SearchHistory
{
    QString date;
    QString time;
    QString table;
    bool allItemsSelected;
    bool skipDiacritics;
    bool skipCharTypes;
    QStringList poetID;
    QString searchPhrase;
    int count;
};

struct SearchWord
{
    QStringList orderExact;
    QStringList order;
    QStringList negExact;
    QStringList neg;
    QStringList plusExact;
    QStringList plus;
};

struct SearchWordLike
{
    QString orderAllLike;
    QString plusAllLike;
};

struct SearchSetting
{
    QStringList poetID;
    bool allItemsSelected;
    bool skipDiacritics;
    bool skipCharTypes;
    QString searchPhrase;
    SearchTable table;
    bool searchState;
    bool isSearching;
};

struct StartupSettings
{
    QList<int> poetSplitterSize;
    bool isMaximized;
    QSize mainWindowSize;
    QPoint mainWindowPos;
};

struct AppSettings
{
    QString mainDBPath;
    QSqlDatabase mainDB;
    QStringList otherDBsPath;
    QListWidget *listWidget;
    QTabWidget *tabWidget;
    QWidget *activeTab;
    QMap<QWidget *, QString> tabLastLevelID;
    QMap<QWidget *, QString> tabCurrentPoem;
    QString appFN;
    QString appFS;
    QString listFN;
    QString listFS;
    QString viewFN;
    QString viewFS;
    QString viewFSCurrent;
    PoemDisplayType pDisplayType;
    SearchSetting ss;
    int hemistichDistance;
    int hemistichDistanceMin;
    int hemistichDistanceMax;
    bool isDarkMode;
    bool showBookmarks;
    bool isOpenWordSearchForm;
    bool isOpenAbjadForm;
    bool isOpenAbjadFormMini;
};

struct GanjoorPath
{
    QStringList level;
    QStringList text;
    QStringList id;
};

struct XmlPoet
{
    QString CatName;
    QString PoetID;
    QString CatID;
    QString DownloadUrl;
    QString FileExt;
    QString ImageUrl;
    QString FileSizeInByte;
    QString LowestPoemID;
    QString PubDate;
};

class SqliteDB
{
public:
    SqliteDB() {;}
    SqliteDB(const QString &databasePath, const QString &connectionName, bool isMainDB = false)
    {
        database_path = databasePath;
        if(isMainDB)
            database = QSqlDatabase::addDatabase("QSQLITE");
        else
            database = QSqlDatabase::addDatabase("QSQLITE", connectionName);
        database.setDatabaseName(databasePath);
        database.open();
    }

    void setDatabase(const QString &databasePath, const QString &connectionName, bool isMainDB = false)
    {
        remove();
        database_path = databasePath;
        if(isMainDB)
            database = QSqlDatabase::addDatabase("QSQLITE");
        else
            database = QSqlDatabase::addDatabase("QSQLITE", connectionName);
        database.setDatabaseName(databasePath);
        database.open();
    }

    void remove()
    {
        if(database.isOpen())
        {
            database_path.clear();
            QString connection;
            connection = database.connectionName();
            database.close();
            database = QSqlDatabase();
            database.removeDatabase(connection);
        }
    }

    QString DBPath() {return database_path;}
    QSqlDatabase &DB() {return database;}

private:
    QString database_path;
    QSqlDatabase database;
};

class TableValues
{
private:
    QString values;

public:
    TableValues() {;}
    TableValues(QString value) {values = value + ",";}

    inline void addValue(QString value) {values += value + ",";}
    inline QString getValues() {return values.left(values.size() - 1);}
    inline void clear() {values.clear();}
};

bool isStdGanjoorDB(const QSqlDatabase &database, DatabaseType databaseType = MainDatabase);
bool isStdGanjoorDB(const QString &database, DatabaseType databaseType = MainDatabase);
void lineEditDirectionCorrector(QLineEdit *lineEdit = nullptr, Qt::LayoutDirection direction = Qt::RightToLeft);
void listWidgetPoetList(QListWidget *listWidget, const QSqlDatabase &database, bool checkable = false, const QString &strQuery = "SELECT name, id, cat_id FROM poet");
bool listWidgetItemList(QListWidget *listWidget, const QSqlDatabase &database, const QString &id);
bool importDatabase(QSqlDatabase &mainDatabase, const QSqlDatabase &secondDatabase, bool removePreVersion = false, int speed = 1000);
bool importDatabase(QSqlDatabase &mainDatabase, const QString &secondDatabase, bool removePreVersion = false, int speed = 1000);
bool importDatabaseByIDs(QSqlDatabase &mainDatabase, const QSqlDatabase &secondDatabase, const QStringList &poetIDs, int speed = 1000);
bool exportDatabase(const QSqlDatabase &mainDatabase, QSqlDatabase &newDatabase, const QStringList &poetIDs, int speed = 1000, bool createNewTable = true);
bool removePoet(QSqlDatabase &database, const QStringList &poetIDs);
bool createGanjoorTable(QSqlDatabase &database);
bool createGanjoorTable(const QString &database);
QString getPoetNameByPoetID(const QSqlDatabase &database, const QString &id);
QString getPoetIDByCatID(const QSqlDatabase &database, const QString &id);
QString getPositionByPoemIDVorder(const QSqlDatabase &database, const QString &id, const QString &vorder);
QStringList getPoetsIDs(const QSqlDatabase &database);
bool existsCommonPoet(const QSqlDatabase &mainDatabase, const QSqlDatabase &secondDatabase);
bool existsCommonPoet(const QSqlDatabase &mainDatabase, const QStringList &poetIDs);
QStringList commonPoetsID(const QSqlDatabase &mainDatabase, const QSqlDatabase &secondDatabase);
QStringList uncommonPoetsID(const QSqlDatabase &mainDatabase, const QSqlDatabase &secondDatabase);
QString listWidgetAddTitle(const QSqlDatabase &database, const QString &text, const QString &poemID, int spacePermittedNumber);
GanjoorPath recursiveIDs(const QSqlDatabase &database, const QString &level, const QString &id);
GanjoorPath recursiveIDs(const QSqlDatabase &database, const QString &levelID);
QString idTitle(const QSqlDatabase &database, const QString &level, const QString &id);
QString idTitle(const QSqlDatabase &database, const QString &levelID);
QString spaceReplace(const QString &text, const QString &replaceWith, int permittedNumber);
QList<XmlPoet> xmlPoetListElements(const QDomElement &root, const QString &tagName);
void tableWidgetDownloadList(QTableWidget *tableWidget, const QSqlDatabase &mainDatabase, const QList<XmlPoet> &list, QList<int> &preInstalled, QList<int> &notInstalled, bool isDarkMode = false, TableDisplayType tableDisplayType = AllItems);
void dbCloseRemove(QSqlDatabase *database);
void dbCloseRemove(QSqlDatabase &database);
QString oldStyleHtml(const QSqlDatabase &database, const QString &poemID, const QString &fontSize, bool isDarkMode = false, const QStringList &highlightText = QStringList(), bool showAllBookmarks = true, const QString &bookmarkVerseID = QString());
QString newStyleHtml(const QSqlDatabase &database, const QString &poemID, const QString &fontSize, bool isDarkMode = false, const QStringList &highlightText = QStringList(), bool showAllBookmarks = true, const QString &bookmarkVerseID = QString(), int hemistichDistance = 60);
inline void htmlEntitiesAndNewLine(QString &str);
inline QString openTable(bool &isTableOpen);
inline QString closeTable(bool &isTableOpen);
inline QString lBreakAdder(bool preBreak);
inline void bookmarkHighlighter(QString &str);
void searchHighlighter(QString &text, const QStringList &list, const QString &cssClass);
QString previousPoem(const QSqlDatabase &database, const QString &level, const QString &id);
QString previousPoem(const QSqlDatabase &database, const QString &levelID);
QString nextPoem(const QSqlDatabase &database, const QString &level, const QString &id);
QString nextPoem(const QSqlDatabase &database, const QString &levelID);
bool isBookmarked(const QSqlDatabase &database, const QString &level, const QString &id, const QString &verse_id);
bool isBookmarked(const QSqlDatabase &database, const QString &levelID, const QString &verse_id);
void setBookmarked(const QSqlDatabase &database, const QString &level, const QString &id, const QString &verse_id, bool value);
void setBookmarked(const QSqlDatabase &database, const QString &levelID, const QString &verse_id, bool value);
bool importBookmarks(QSqlDatabase &mainDatabase, const QSqlDatabase &secondDatabase, int speed = 50000);
bool importBookmarks(QSqlDatabase &mainDatabase, const QString &secondDatabase, int speed = 50000);
bool exportBookmarks(const QSqlDatabase &mainDatabase, QSqlDatabase &newDatabase, int speed = 50000);
QString randString(int len = 16);
QString createDBDialog(QWidget *parent = nullptr, const QString &defaultFilePath = QDir::homePath() + "/" + Constants::DefaultDBName);
QString writableDirDialog(QWidget *parent = nullptr, const QString &defaultDirPath = QDir::homePath());
QMessageBox::StandardButton messageBox(const QString &title, const QString &text, MessageBoxType messageBoxType, QWidget *parent = nullptr, QMessageBox::StandardButton defaultButton = QMessageBox::No);
QString byteToHuman(qint64 size);
QString ratioFontSize(double size, double ratio = 1);
QString appStyleSheet(const QString &appFN = "Sahel", const QString &appFS = "10.5", const QString &listFN = "Sahel", const QString &listFS = "11", const QString &viewFN = "Sahel", const QString &viewFS = "11");
bool dbExtCheck(const QString &filePath);
bool removeTempDir(const QString &dirName);
QString rosybitDir();
QString appNameOS();
QString defaultDBPath();
void showFileInDir(const QString &filePath);
QString gregorianToPersian(int day, int month, int year, const QString &delimiter = "/", bool dd = true, bool mm = true);
QString nowDate(const QString &delimiter = "/", bool dd = true, bool mm = true);
QString nowTime(const QString &delimiter = ":");
QString correctHtmlTableText(const QString &text);
QString persianNumber(int n);
bool idComp(const QString &id1, const QString &id2);
bool faLessThan(const QString &str1, const QString &str2);
QString faReplaceChars(const QString &text);

// COMMON_SEARCH
QString searchTableWidget(AppSettings *appSettings, QTableWidget *tableWidget, const QString &strQuery);
bool patternMatched(const QList<SearchWord> &swList, const QString &text);
int wordCount(const QString &word, const QString &text);
SearchWordLike searchWordLike(const SearchWord &sw, const QString &fieldStr);
QString hashSignFinder(const QString &text);
SearchWord searchWordAnalyser(const QString &orPart);
QString searchStrQuery(const QSqlDatabase &database, const QString &userStr, bool allItemsSelected = true, const QStringList &poetID = QStringList(), SearchTable searchTable = VerseTable, bool sDiacritics = false, bool sCharTypes = false);
QString searchRange(const QSqlDatabase &database, const QStringList &poetID = QStringList(), SearchTable searchTable = VerseTable);
bool findActiveWord(const SearchWord &sw);
QString wordLikeRevision(const QString &text);
QString skipZWNJ(const QString &text);
QString skipDiacritics(const QString &text);
QString skipCharTypes(const QString &text);
QString removeZWNJ(const QString &text);
QString removeDiacritics(const QString &text);
QString removeCharTypes(const QString &text);
QString replace_AEKVH_withUnderscore(const QString &text);
QStringList textListHighlight(const QString &searchPhrase);
QString quotationRemover(const QString &text);
// COMMON_SEARCH

#endif // COMMON_FUNCTIONS_H

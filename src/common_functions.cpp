/*
    [Ghazal: The library of persian poetry]
    Publisher: Rosybit
    Url: http://www.rosybit.com
    GitHub: https://github.com/abroshan39/ghazal
    Version: 1.4
    Author: Aboutaleb Roshan [ab.roshan39@gmail.com]
    License: MIT License
*/

#include "common_functions.h"

const QString Constants::Rosybit = "Rosybit";
const QString Constants::RosybitFa = "رزی‌بیت";
const QString Constants::RosybitUrl = "http://www.rosybit.com";
const QString Constants::Programmer = "Aboutaleb Roshan";
const QString Constants::ProgrammerFa = "ابوطالب روشن";
const QString Constants::Email = "ab.roshan39@gmail.com";
const QString Constants::AppName = "Ghazal";
const QString Constants::AppNameFa = "غزل";
const QString Constants::AppVersion = "1.4";
const QString Constants::GitHub = "https://github.com/abroshan39/ghazal";
const QString Constants::AppUrl = "http://www.rosybit.com/apps/ghazal";
const QString Constants::DefaultDBName = "ganjoor.s3db";
const QString Constants::SettingsFileName = "settings.conf";
const QString Constants::HistoryFileName = "history.ghl";

bool isStdGanjoorDB(const QSqlDatabase &database, DatabaseType databaseType)
{
    QSqlQuery query(database);

    if(databaseType == MainDatabase)
    {
        if(query.exec("SELECT id, name, cat_id, description FROM poet") &&
           query.exec("SELECT id, poet_id, text, parent_id, url FROM cat") &&
           query.exec("SELECT id, cat_id, title, url FROM poem") &&
           query.exec("SELECT poem_id, vorder, position, text FROM verse"))
            return true;
    }
    else if(databaseType == BookmarkDatabase)
    {
        if(query.exec("SELECT poem_id, verse_id, pos FROM fav"))
            return true;
    }

    return false;
}

bool isStdGanjoorDB(const QString &database, DatabaseType databaseType)
{
    SqliteDB checkDB(database, "checkDatabase");
    bool result = isStdGanjoorDB(checkDB.DB(), databaseType);
    checkDB.remove();

    return result;
}

void lineEditDirectionCorrector(QLineEdit *lineEdit, Qt::LayoutDirection direction)
{
    // https://stackoverflow.com/questions/10998105/qt-how-to-change-the-direction-of-placeholder-in-a-qlineedit
    if(lineEdit)
    {
        if(direction == Qt::RightToLeft)
        {
            QKeyEvent e(QEvent::KeyPress, Qt::Key_Direction_R, Qt::NoModifier);
            QApplication::sendEvent(lineEdit, &e);
            lineEdit->setAlignment(Qt::AlignLeft);
        }
        else if(direction == Qt::LeftToRight)
        {
            QKeyEvent e(QEvent::KeyPress, Qt::Key_Direction_L, Qt::NoModifier);
            QApplication::sendEvent(lineEdit, &e);
            lineEdit->setAlignment(Qt::AlignLeft);
        }
    }
}

void listWidgetPoetList(QListWidget *listWidget, const QSqlDatabase &database, bool checkable, const QString &strQuery)
{
    QSqlQuery query(database);
    QList<QString> poetList;
    QString delimiter = "||**|*|**||";

    listWidget->clear();

    query.exec(strQuery);
    while(query.next())
    {
        QString name = query.value(0).toString();
        QString id = query.value(1).toString();
        poetList.append(faReplaceChars(name) + delimiter + name + delimiter + id);
    }

    std::sort(poetList.begin(), poetList.end(), faLessThan);

    QListWidgetItem *item;
    for(int i = 0; i < poetList.count(); i++)
    {
        QStringList nameID = poetList[i].split(delimiter);
        item = new QListWidgetItem(nameID[1]);
        item->setData(Qt::UserRole, nameID[2]);
        if(checkable)
            item->setCheckState(Qt::Unchecked);
        listWidget->addItem(item);
    }
}

bool listWidgetItemList(QListWidget *listWidget, const QSqlDatabase &database, const QString &id)
{
    int space_count = 25;
    QString item_space = "";
    QSqlQuery query(database);

    listWidget->clear();

    query.exec("SELECT text, id FROM cat WHERE parent_id = " + id);
    if(query.first())
    {
        query.previous();
        QListWidgetItem *item;
        while(query.next())
        {
            item = new QListWidgetItem(query.value(0).toString());
            item->setData(Qt::UserRole, "2-" + query.value(1).toString());
            listWidget->addItem(item);
        }
        space_count = 12;
        item_space = "      ";
    }

    query.exec("SELECT title, id FROM poem WHERE cat_id = " + id);
    if(query.first())
    {
        query.previous();
        QString text, poemID;
        QListWidgetItem *item;
        while(query.next())
        {
            text = query.value(0).toString();
            poemID = query.value(1).toString();
            item = new QListWidgetItem(item_space + listWidgetAddTitle(database, text, poemID, space_count));
            item->setData(Qt::UserRole, "3-" + poemID);
            listWidget->addItem(item);
        }
    }

    return true;
}

bool importDatabase(QSqlDatabase &mainDatabase, const QSqlDatabase &secondDatabase, bool removePreVersion, int speed)
{
    QSqlQuery queryMainDB(mainDatabase), query(secondDatabase);
    TableValues *tValues = new TableValues;

    if(removePreVersion)
    {
        removePoet(mainDatabase, commonPoetsID(mainDatabase, secondDatabase));
    }
    else
    {
        if(existsCommonPoet(mainDatabase, secondDatabase))
        {
            QStringList uncommonIDs(uncommonPoetsID(mainDatabase, secondDatabase));
            if(!uncommonIDs.isEmpty())
                importDatabaseByIDs(mainDatabase, secondDatabase, uncommonIDs, speed);
            delete tValues;
            return true;
        }
    }

    query.exec("SELECT id, name, cat_id, description FROM poet");
    while(query.next())
    {
        tValues->clear();
        query.previous();
        for(int i = 0; i < speed; i++)
        {
            if(query.next())
                tValues->addValue(QString("(%1,'%2',%3,'%4')").arg(query.value(0).toInt()).arg(query.value(1).toString()).arg(query.value(2).toInt()).arg(query.value(3).toString()));
            else
                break;
        }
        queryMainDB.exec("INSERT INTO poet (id, name, cat_id, description) VALUES " + tValues->getValues());
    }

    query.exec("SELECT id, poet_id, text, parent_id, url FROM cat");
    while(query.next())
    {
        tValues->clear();
        query.previous();
        for(int i = 0; i < speed; i++)
        {
            if(query.next())
                tValues->addValue(QString("(%1,%2,'%3',%4,'%5')").arg(query.value(0).toInt()).arg(query.value(1).toInt()).arg(query.value(2).toString()).arg(query.value(3).toInt()).arg(query.value(4).toString()));
            else
                break;
        }
        queryMainDB.exec("INSERT INTO cat (id, poet_id, text, parent_id, url) VALUES " + tValues->getValues());
    }

    query.exec("SELECT id, cat_id, title, url FROM poem");
    while(query.next())
    {
        tValues->clear();
        query.previous();
        for(int i = 0; i < speed; i++)
        {
            if(query.next())
                tValues->addValue(QString("(%1,%2,'%3','%4')").arg(query.value(0).toInt()).arg(query.value(1).toInt()).arg(query.value(2).toString()).arg(query.value(3).toString()));
            else
                break;
        }
        queryMainDB.exec("INSERT INTO poem (id, cat_id, title, url) VALUES " + tValues->getValues());
    }

    query.exec("SELECT poem_id, vorder, position, text FROM verse");
    while(query.next())
    {
        tValues->clear();
        query.previous();
        for(int i = 0; i < speed; i++)
        {
            if(query.next())
                tValues->addValue(QString("(%1,%2,%3,'%4')").arg(query.value(0).toInt()).arg(query.value(1).toInt()).arg(query.value(2).toInt()).arg(query.value(3).toString()));
            else
                break;
        }
        queryMainDB.exec("INSERT INTO verse (poem_id, vorder, position, text) VALUES " + tValues->getValues());
    }

    delete tValues;
    return true;
}

bool importDatabase(QSqlDatabase &mainDatabase, const QString &secondDatabase, bool removePreVersion, int speed)
{
    SqliteDB secondDB(secondDatabase, "secondDatabase");
    bool result = importDatabase(mainDatabase, secondDB.DB(), removePreVersion, speed);
    secondDB.remove();

    return result;
}

bool importDatabaseByIDs(QSqlDatabase &mainDatabase, const QSqlDatabase &secondDatabase, const QStringList &poetIDs, int speed)
{
    return exportDatabase(secondDatabase, mainDatabase, poetIDs, speed, false);
}

bool exportDatabase(const QSqlDatabase &mainDatabase, QSqlDatabase &newDatabase, const QStringList &poetIDs, int speed, bool createNewTable)
{
    QSqlQuery queryMainDB(mainDatabase), query(mainDatabase), queryNewDB(newDatabase);
    TableValues *tValues = new TableValues;

    if(createNewTable)
        createGanjoorTable(newDatabase);

    for(int i = 0; i < poetIDs.count(); i++)
    {
        QString catID, pIN, vIN;

        queryMainDB.exec("SELECT id FROM cat WHERE poet_id = " + poetIDs[i]);
        while(queryMainDB.next())
        {
            pIN += (catID = queryMainDB.value(0).toString()) + ",";
            query.exec("SELECT id FROM poem WHERE cat_id = " + catID);
            while(query.next())
                vIN += query.value(0).toString() + ",";
        }
        pIN = pIN.left(pIN.size() - 1);
        vIN = vIN.left(vIN.size() - 1);

        queryMainDB.exec("SELECT id, name, cat_id, description FROM poet WHERE id = " + poetIDs[i]);
        while(queryMainDB.next())
        {
            tValues->clear();
            queryMainDB.previous();
            for(int j = 0; j < speed; j++)
            {
                if(queryMainDB.next())
                    tValues->addValue(QString("(%1,'%2',%3,'%4')").arg(queryMainDB.value(0).toInt()).arg(queryMainDB.value(1).toString()).arg(queryMainDB.value(2).toInt()).arg(queryMainDB.value(3).toString()));
                else
                    break;
            }
            queryNewDB.exec("INSERT INTO poet (id, name, cat_id, description) VALUES " + tValues->getValues());
        }

        queryMainDB.exec("SELECT id, poet_id, text, parent_id, url FROM cat WHERE poet_id = " + poetIDs[i]);
        while(queryMainDB.next())
        {
            tValues->clear();
            queryMainDB.previous();
            for(int j = 0; j < speed; j++)
            {
                if(queryMainDB.next())
                    tValues->addValue(QString("(%1,%2,'%3',%4,'%5')").arg(queryMainDB.value(0).toInt()).arg(queryMainDB.value(1).toInt()).arg(queryMainDB.value(2).toString()).arg(queryMainDB.value(3).toInt()).arg(queryMainDB.value(4).toString()));
                else
                    break;
            }
            queryNewDB.exec("INSERT INTO cat (id, poet_id, text, parent_id, url) VALUES " + tValues->getValues());
        }

        queryMainDB.exec("SELECT id, cat_id, title, url FROM poem WHERE cat_id IN (" + pIN + ")");
        while(queryMainDB.next())
        {
            tValues->clear();
            queryMainDB.previous();
            for(int j = 0; j < speed; j++)
            {
                if(queryMainDB.next())
                    tValues->addValue(QString("(%1,%2,'%3','%4')").arg(queryMainDB.value(0).toInt()).arg(queryMainDB.value(1).toInt()).arg(queryMainDB.value(2).toString()).arg(queryMainDB.value(3).toString()));
                else
                    break;
            }
            queryNewDB.exec("INSERT INTO poem (id, cat_id, title, url) VALUES " + tValues->getValues());
        }

        queryMainDB.exec("SELECT poem_id, vorder, position, text FROM verse WHERE poem_id IN (" + vIN + ")");
        while(queryMainDB.next())
        {
            tValues->clear();
            queryMainDB.previous();
            for(int j = 0; j < speed; j++)
            {
                if(queryMainDB.next())
                    tValues->addValue(QString("(%1,%2,%3,'%4')").arg(queryMainDB.value(0).toInt()).arg(queryMainDB.value(1).toInt()).arg(queryMainDB.value(2).toInt()).arg(queryMainDB.value(3).toString()));
                else
                    break;
            }
            queryNewDB.exec("INSERT INTO verse (poem_id, vorder, position, text) VALUES " + tValues->getValues());
        }
    }

    delete tValues;
    return true;
}

bool removePoet(QSqlDatabase &database, const QStringList &poetIDs)
{
    QSqlQuery query(database), query2(database);

    for(int i = 0; i < poetIDs.count(); i++)
    {
        QString catID, pIN, vIN;

        query.exec("SELECT id FROM cat WHERE poet_id = " + poetIDs[i]);
        while(query.next())
        {
            pIN += (catID = query.value(0).toString()) + ",";
            query2.exec("SELECT id FROM poem WHERE cat_id = " + catID);
            while(query2.next())
                vIN += query2.value(0).toString() + ",";
        }
        pIN = pIN.left(pIN.size() - 1);
        vIN = vIN.left(vIN.size() - 1);

        query.exec("DELETE FROM verse WHERE poem_id IN (" + vIN + ")");
        query.exec("DELETE FROM poem WHERE cat_id IN (" + pIN + ")");
        query.exec("DELETE FROM cat WHERE poet_id = " + poetIDs[i]);
        query.exec("DELETE FROM poet WHERE id = " + poetIDs[i]);
    }

    return true;
}

bool createGanjoorTable(QSqlDatabase &database)
{
    QSqlQuery query(database);

    // TABLES:
    query.exec("CREATE TABLE poet (id INTEGER NOT NULL, name NVARCHAR(20), cat_id INTEGER, description TEXT, PRIMARY KEY(id))");
    query.exec("CREATE TABLE cat (id INTEGER NOT NULL, poet_id INTEGER, text NVARCHAR(100), parent_id INTEGER, url NVARCHAR(255), PRIMARY KEY(id))");
    query.exec("CREATE TABLE poem (id INTEGER, cat_id INTEGER, title NVARCHAR(255), url NVARCHAR(255), PRIMARY KEY(id))");
    query.exec("CREATE TABLE verse (poem_id INTEGER, vorder INTEGER, position INTEGER, text TEXT)");

    query.exec("CREATE TABLE fav (poem_id INTEGER, verse_id INTEGER, pos INTEGER)");
    query.exec("CREATE TABLE poemsnd (poem_id INTEGER NOT NULL, id INTEGER NOT NULL, filepath TEXT, description TEXT, dnldurl TEXT, isdirect INTEGER, syncguid TEXT, fchksum TEXT, isuploaded INTEGER)");
    query.exec("CREATE TABLE sndsync (poem_id INTEGER NOT NULL, snd_id INTEGER NOT NULL, verse_order INTEGER NOT NULL, milisec INTEGER NOT NULL)");
    query.exec("CREATE TABLE gil (cat_id INTEGER)");
    query.exec("CREATE TABLE gver (curver INTEGER)");

    // INDICES:
    query.exec("CREATE INDEX cat_pid ON cat (parent_id ASC)");
    query.exec("CREATE INDEX poem_cid ON poem (cat_id ASC)");
    query.exec("CREATE INDEX verse_pid ON verse (poem_id ASC)");

    query.exec("CREATE INDEX idx_poem_title ON poem (id ASC, title ASC)");
    query.exec("CREATE UNIQUE INDEX idx_poem_catid ON poem (id ASC, cat_id ASC)");
    query.exec("CREATE UNIQUE INDEX idx_verse ON verse (poem_id ASC, vorder ASC)");

    return true;
}

bool createGanjoorTable(const QString &database)
{
    SqliteDB sqliteDB(database, "createGanjoorTable");
    bool result = createGanjoorTable(sqliteDB.DB());
    sqliteDB.remove();

    return result;
}

QString getPoetNameByPoetID(const QSqlDatabase &database, const QString &id)
{
    QSqlQuery query("SELECT name FROM poet WHERE id = " + id, database);
    if(query.next())
        return query.value(0).toString();
    return QString();
}

QString getPoetIDByCatID(const QSqlDatabase &database, const QString &id)
{
    QSqlQuery query("SELECT id FROM poet WHERE cat_id = " + id, database);
    if(query.next())
        return query.value(0).toString();
    return QString();
}

QString getPositionByPoemIDVorder(const QSqlDatabase &database, const QString &id, const QString &vorder)
{
    QSqlQuery query(QString("SELECT position FROM verse WHERE poem_id = %1 AND vorder = %2").arg(id, vorder), database);
    if(query.next())
        return query.value(0).toString();
    return QString();
}

QStringList getPoetsIDs(const QSqlDatabase &database)
{
    QSqlQuery query(database);
    QStringList poetIDs;

    query.exec("SELECT id FROM poet");
    while(query.next())
        poetIDs << query.value(0).toString();

    return poetIDs;
}

bool existsCommonPoet(const QSqlDatabase &mainDatabase, const QSqlDatabase &secondDatabase)
{
    QSqlQuery query(mainDatabase);
    QStringList poetIDs(getPoetsIDs(secondDatabase));

    for(int i = 0; i < poetIDs.count(); i++)
    {
        query.exec("SELECT * FROM poet WHERE id = " + poetIDs[i]);
        if(query.first())
            return true;
    }

    return false;
}

bool existsCommonPoet(const QSqlDatabase &mainDatabase, const QStringList &poetIDs)
{
    QSqlQuery query(mainDatabase);

    for(int i = 0; i < poetIDs.count(); i++)
    {
        query.exec("SELECT * FROM poet WHERE id = " + poetIDs[i]);
        if(query.first())
            return true;
    }

    return false;
}

QStringList commonPoetsID(const QSqlDatabase &mainDatabase, const QSqlDatabase &secondDatabase)
{
    QSqlQuery query(mainDatabase);
    QStringList poetIDs(getPoetsIDs(secondDatabase)), newPoetIDs;

    for(int i = 0; i < poetIDs.count(); i++)
    {
        query.exec("SELECT * FROM poet WHERE id = " + poetIDs[i]);
        if(query.first())
            newPoetIDs << poetIDs[i];
    }

    return newPoetIDs;
}

QStringList uncommonPoetsID(const QSqlDatabase &mainDatabase, const QSqlDatabase &secondDatabase)
{
    QSqlQuery query(mainDatabase);
    QStringList poetIDs(getPoetsIDs(secondDatabase)), newPoetIDs;

    for(int i = 0; i < poetIDs.count(); i++)
    {
        query.exec("SELECT * FROM poet WHERE id = " + poetIDs[i]);
        if(!query.first())
            newPoetIDs << poetIDs[i];
    }

    return newPoetIDs;
}

QString listWidgetAddTitle(const QSqlDatabase &database, const QString &text, const QString &poemID, int spacePermittedNumber)
{
    QString str(text);
    QSqlQuery query(database);
    query.exec("SELECT text FROM verse WHERE poem_id = " + poemID + " AND vorder = 1");
    query.next();
    str = str + ": " + query.value(0).toString();
    return spaceReplace(str, "…", spacePermittedNumber);
}

GanjoorPath recursiveIDs(const QSqlDatabase &database, const QString &level, const QString &id)
{
    GanjoorPath gp;
    QString parent_id, current_id(id);
    QSqlQuery query(database);

    if(level == "3")
    {
        query.exec("SELECT cat_id, title FROM poem WHERE id = " + current_id);
        query.next();

        gp.text << query.value(1).toString();
        gp.id << current_id;
        gp.level << "3";

        current_id = query.value(0).toString();
    }

    query.exec("SELECT parent_id, text FROM cat WHERE id = " + current_id);
    while(query.next())
    {
        gp.text << query.value(1).toString();
        gp.id << current_id;

        parent_id = query.value(0).toString();
        if(parent_id == "0")
        {
            gp.level << "1";
            break;
        }
        else
        {
            gp.level << "2";
        }

        current_id = parent_id;
        query.exec("SELECT parent_id, text FROM cat WHERE id = " + current_id);
    }

    return gp;
}

GanjoorPath recursiveIDs(const QSqlDatabase &database, const QString &levelID)
{
    QString level(levelID.left(1)), id(levelID.right(levelID.size() - 2));
    return recursiveIDs(database, level, id);
}

QString idTitle(const QSqlDatabase &database, const QString &level, const QString &id)
{
    QString parent_id, current_id(id);
    QSqlQuery query(database);

    if(level == "3")
    {
        query.exec("SELECT cat_id, title FROM poem WHERE id = " + current_id);
        query.next();
        current_id = query.value(0).toString();
    }

    query.exec("SELECT parent_id, text FROM cat WHERE id = " + current_id);
    while(query.next())
    {
        parent_id = query.value(0).toString();
        if(parent_id == "0")
            return query.value(1).toString();

        current_id = parent_id;
        query.exec("SELECT parent_id, text FROM cat WHERE id = " + current_id);
    }

    return QString();
}

QString idTitle(const QSqlDatabase &database, const QString &levelID)
{
    QString level(levelID.left(1)), id(levelID.right(levelID.size() - 2));
    return idTitle(database, level, id);
}

QString spaceReplace(const QString &text, const QString &replaceWith, int permittedNumber)
{
    QString str(text);

//    QRegExp regex("(\\s+)");
//    int count = 0, pos = 0;
//    while((pos = regex.indexIn(str, pos)) != -1 && (count++) < permittedNumber)
//        pos += regex.matchedLength();

    QRegularExpression regex("(\\s+)");
    QRegularExpressionMatchIterator i = regex.globalMatch(str);
    int count = 0, pos = 0;
    while(i.hasNext() && count <= permittedNumber)
    {
        pos = i.next().capturedStart();
        count++;
    }

    if(count > permittedNumber)
        str = str.left(pos) + replaceWith;

    str = str.replace(QRegularExpression("\\s+"), " ");

    return str;
}

QList<XmlPoet> xmlPoetListElements(const QDomElement &root, const QString &tagName)
{
    QList<XmlPoet> list;
    QDomNodeList poetNodeList = root.elementsByTagName(tagName);

    if(!root.tagName().contains("ganjoor", Qt::CaseInsensitive))
        return list;

    for(int i = 0; i < poetNodeList.count(); i++)
    {
        XmlPoet xmlPoet;
        QDomNode poetNode = poetNodeList.at(i);

        xmlPoet.CatName = poetNode.namedItem("CatName").toElement().text();
        xmlPoet.PoetID = poetNode.namedItem("PoetID").toElement().text();
        xmlPoet.CatID = poetNode.namedItem("CatID").toElement().text();
        xmlPoet.DownloadUrl = poetNode.namedItem("DownloadUrl").toElement().text();
        xmlPoet.FileExt = poetNode.namedItem("FileExt").toElement().text();
        xmlPoet.ImageUrl = poetNode.namedItem("ImageUrl").toElement().text();
        xmlPoet.FileSizeInByte = poetNode.namedItem("FileSizeInByte").toElement().text();
        xmlPoet.LowestPoemID = poetNode.namedItem("LowestPoemID").toElement().text();
        xmlPoet.PubDate = poetNode.namedItem("PubDate").toElement().text();

        list.append(xmlPoet);
    }

    return list;
}

void tableWidgetDownloadList(QTableWidget *tableWidget, const QSqlDatabase &mainDatabase, const QList<XmlPoet> &list, QList<int> &preInstalled, QList<int> &notInstalled, bool isDarkMode, TableDisplayType tableDisplayType)
{
    tableWidget->clear();
    tableWidget->model()->removeRows(0, tableWidget->model()->rowCount());
    tableWidget->model()->removeColumns(0, tableWidget->model()->columnCount());
    tableWidget->setColumnCount(5);
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QStringList colList;
    colList << "نام شاعر یا نویسنده" << "حجم فایل" << "تاریخ انتشار";
    tableWidget->setHorizontalHeaderLabels(colList);

    preInstalled.clear();
    notInstalled.clear();
    int row_count = 0;
    for(int i = 0; i < list.count(); i++)
    {
        QString catName = list[i].CatName;
        QString poetID = list[i].PoetID;
        long long int fileSize = list[i].FileSizeInByte.toInt();
        QString pubDate = list[i].PubDate;
        QString downloadURL = list[i].DownloadUrl;

        bool isPoetExist = false;
        if(mainDatabase.isOpen())
        {
            QSqlQuery query(mainDatabase);
            query.exec("SELECT name FROM poet WHERE id = " + poetID);
            isPoetExist = query.first();
        }

        if(tableDisplayType == NotInstalledItems)
            if(isPoetExist)
                continue;

        if(tableDisplayType == PreInstalledItems)
            if(!isPoetExist)
                continue;

        tableWidget->insertRow(row_count);

        QStringList dmyList = pubDate.split("-");
        int d = dmyList[2].toInt(), m = dmyList[1].toInt(), y = dmyList[0].toInt();
        pubDate = gregorianToPersian(d, m, y, "/", false, false);

        QTableWidgetItem *item1 = new QTableWidgetItem;
        QTableWidgetItem *item2 = new QTableWidgetItem;
        QTableWidgetItem *item3 = new QTableWidgetItem;
        QTableWidgetItem *item4 = new QTableWidgetItem;
        QTableWidgetItem *item5 = new QTableWidgetItem;

        item1->setCheckState(Qt::Unchecked);
        item1->setData(Qt::UserRole, poetID);

        if(isPoetExist)
        {
            preInstalled << row_count;
            if(!isDarkMode)
                item1->setBackground(QColor(144, 238, 144));
            else
                item1->setBackground(QColor(20, 20, 20));
        }
        else
        {
            notInstalled << row_count;
            if(!isDarkMode)
                item1->setBackground(QColor(255, 153, 153));
            else
                item1->setBackground(QColor(60, 60, 60));
        }

        item1->setText(catName);
        item2->setText(byteToHuman(fileSize));
        item3->setText(pubDate);
        item4->setText(downloadURL);
        item5->setText(QString::number(fileSize));

        tableWidget->setItem(row_count, 0, item1);
        tableWidget->setItem(row_count, 1, item2);
        tableWidget->setItem(row_count, 2, item3);
        tableWidget->setItem(row_count, 3, item4);
        tableWidget->setItem(row_count, 4, item5);

        row_count++;
    }
    tableWidget->hideColumn(3);
    tableWidget->hideColumn(4);
    tableWidget->horizontalHeader()->setDefaultSectionSize(180);
}

void dbCloseRemove(QSqlDatabase *database)
{
    QString connection;
    connection = database->connectionName();
    database->close();
    *database = QSqlDatabase();
    database->removeDatabase(connection);
}

void dbCloseRemove(QSqlDatabase &database)
{
    QString connection;
    connection = database.connectionName();
    database.close();
    database = QSqlDatabase();
    database.removeDatabase(connection);
}

QString oldStyleHtml(const QSqlDatabase &database, const QString &poemID, const QString &fontSize, bool isDarkMode, const QStringList &highlightText, bool showAllBookmarks, const QString &bookmarkVerseID)
{
    QSqlQuery query(database), queryIn(database);
    QString textColor = isDarkMode ? "white" : "black";
    QString searchHighlightCssClass = "shClass";
    double size = fontSize.toDouble();
    bool preBreak = true;
    bool pos2Break = false;
    bool highlight = !highlightText.isEmpty();
    bool showBookmark = !bookmarkVerseID.isEmpty();

    query.exec("SELECT title FROM poem WHERE id = " + poemID);
    query.next();
    QString title = query.value(0).toString();

    QString strHtml;
    strHtml = "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n";
    strHtml += "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n";
    strHtml += "p, span, table {\n";
    strHtml += "font-size: " + QString::number(size) + "pt;\n";
    strHtml += "color: " + textColor + ";\n";
    strHtml += "white-space: pre-wrap;\n";
    strHtml += "margin-top: 0px;\n";
    strHtml += "margin-bottom: 0px;\n";
    strHtml += "margin-left: 0px;\n";
    strHtml += "margin-right: 0px;\n";
    strHtml += "text-indent: 0px;\n";
    strHtml += "}\n";
    strHtml += "a {\n";
    strHtml += "color: " + textColor + ";\n";
    strHtml += "text-decoration: none;\n";
    strHtml += "}\n";
    strHtml += "." + searchHighlightCssClass + " {\n";
    strHtml += "color: black;\n";
    strHtml += "background-color: yellow;\n";
    strHtml += "}\n";
    strHtml += "</style></head><body>\n";
    strHtml += "<p align=\"center\" dir=\"rtl\"><a href=\"3-" + poemID + "\"><span style=\"font-size:" + ratioFontSize(size, 1.4) + "pt;font-weight:bold;\">" + title + "</span></a><br /></p>\n";

    query.exec("SELECT poem_id FROM verse WHERE poem_id = " + poemID);

    int current_row = 0;
    while(query.seek(current_row))
    {
        QString vorderMain, posMain, textMain;
        QString vorderNext, posNext, textNext;
        QString hrefMain, hrefNext;

        queryIn.exec("SELECT vorder, position, text FROM verse WHERE poem_id = " + poemID + " AND vorder = " + QString::number(++current_row));
        queryIn.next();
        vorderMain = queryIn.value(0).toString();
        posMain = queryIn.value(1).toString();
        textMain = queryIn.value(2).toString().trimmed();
        hrefMain = "3-" + poemID + "-" + vorderMain;
        htmlEntitiesAndNewLine(textMain);
        if(showAllBookmarks && !showBookmark && isBookmarked(database, "3", poemID, vorderMain))
            bookmarkHighlighter(textMain);
        else if(showBookmark && vorderMain == bookmarkVerseID)
            bookmarkHighlighter(textMain);
        if(highlight)
            searchHighlighter(textMain, highlightText, searchHighlightCssClass);

        if(posMain == "0")
        {
            queryIn.exec("SELECT vorder, position, text FROM verse WHERE poem_id = " + poemID + " AND vorder = " + QString::number(++current_row));
            if(queryIn.next())
            {
                vorderNext = queryIn.value(0).toString();
                posNext = queryIn.value(1).toString();
                textNext = queryIn.value(2).toString().trimmed();
                hrefNext = "3-" + poemID + "-" + vorderNext;
                htmlEntitiesAndNewLine(textNext);
                if(showAllBookmarks && !showBookmark && isBookmarked(database, "3", poemID, vorderNext))
                    bookmarkHighlighter(textNext);
                else if(showBookmark && vorderNext == bookmarkVerseID)
                    bookmarkHighlighter(textNext);
                if(highlight)
                    searchHighlighter(textNext, highlightText, searchHighlightCssClass);
            }
        }
        else if(posMain == "2")
        {
            queryIn.exec("SELECT vorder, position, text FROM verse WHERE poem_id = " + poemID + " AND vorder = " + QString::number(current_row + 1));
            if(queryIn.next())
                posNext = queryIn.value(1).toString();
        }

        if(posMain == "0" && posNext != "1")
        {
            textNext = "";
            current_row--;
        }
        else if(posMain == "1")
        {
            textNext = textMain;
            textMain = "";
            hrefNext = hrefMain;
        }
        else if(posMain == "2" && posNext != "3")
        {
            pos2Break = true;
        }

        if(posMain == "0" || posMain == "1")
        {
            strHtml += "<p align=\"center\" dir=\"rtl\"><a href=\"" + hrefMain + "\">" + lBreakAdder(preBreak) + textMain + "</a></p>\n";
            strHtml += "<p align=\"center\" dir=\"rtl\"><a href=\"" + hrefNext + "\">" + textNext + "</a></p>\n";
            preBreak = false;
        }
        else if(posMain == "-1")
        {
            strHtml += "<p align=\"justify\" dir=\"rtl\" style=\"margin-left:0.8em;\"><a href=\"" + hrefMain + "\">" + lBreakAdder(preBreak) + textMain + "<br /></a></p>\n";
            preBreak = true;
        }
        else if(posMain == "2")
        {
            strHtml += "<p align=\"center\" dir=\"rtl\" style=\"font-weight:bold;\"><a href=\"" + hrefMain + "\">" + lBreakAdder(preBreak) + textMain + (pos2Break ? "<br />" : "") + "</a></p>\n";
            preBreak = pos2Break;
            pos2Break = false;
        }
        else if(posMain == "3")
        {
            strHtml += "<p align=\"center\" dir=\"rtl\" style=\"font-weight:bold;\"><a href=\"" + hrefMain + "\">" + textMain + "<br /></a></p>\n";
            preBreak = true;
        }
        else if(posMain == "4")
        {
            strHtml += "<p align=\"right\" dir=\"rtl\"><a href=\"" + hrefMain + "\">" + textMain + "</a></p>\n";
            preBreak = false;
        }
    }
    strHtml += "</body></html>";

    return strHtml;
}

QString newStyleHtml(const QSqlDatabase &database, const QString &poemID, const QString &fontSize, bool isDarkMode, const QStringList &highlightText, bool showAllBookmarks, const QString &bookmarkVerseID, int hemistichDistance)
{
    QSqlQuery query(database), queryIn(database);
    QString textColor = isDarkMode ? "white" : "black";
    QString searchHighlightCssClass = "shClass";
    double size = fontSize.toDouble();
    bool preBreak = true;
    bool pos2Break = false;
    bool highlight = !highlightText.isEmpty();
    bool showBookmark = !bookmarkVerseID.isEmpty();
    bool isTableOpen = false;
    QString hemistichDis = QString::number(hemistichDistance / 2);

    query.exec("SELECT title FROM poem WHERE id = " + poemID);
    query.next();
    QString title = query.value(0).toString();

    QString strHtml;
    strHtml = "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n";
    strHtml += "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n";
    strHtml += "p, span, table {\n";
    strHtml += "font-size: " + QString::number(size) + "pt;\n";
    strHtml += "color: " + textColor + ";\n";
    strHtml += "white-space: pre-wrap;\n";
    strHtml += "margin-top: 0px;\n";
    strHtml += "margin-bottom: 0px;\n";
    strHtml += "margin-left: 0px;\n";
    strHtml += "margin-right: 0px;\n";
    strHtml += "text-indent: 0px;\n";
    strHtml += "}\n";
    strHtml += "a {\n";
    strHtml += "color: " + textColor + ";\n";
    strHtml += "text-decoration: none;\n";
    strHtml += "}\n";
    strHtml += "." + searchHighlightCssClass + " {\n";
    strHtml += "color: black;\n";
    strHtml += "background-color: yellow;\n";
    strHtml += "}\n";
    strHtml += "</style></head><body>\n";
    strHtml += "<p align=\"center\" dir=\"rtl\"><a href=\"3-" + poemID + "\"><span style=\"font-size:" + ratioFontSize(size, 1.4) + "pt;font-weight:bold;\">" + title + "</span></a><br /></p>\n";

    query.exec("SELECT poem_id FROM verse WHERE poem_id = " + poemID);

    int current_row = 0;
    while(query.seek(current_row))
    {
        QString vorderMain, posMain, textMain;
        QString vorderNext, posNext, textNext;
        QString hrefMain, hrefNext;

        queryIn.exec("SELECT vorder, position, text FROM verse WHERE poem_id = " + poemID + " AND vorder = " + QString::number(++current_row));
        queryIn.next();
        vorderMain = queryIn.value(0).toString();
        posMain = queryIn.value(1).toString();
        textMain = queryIn.value(2).toString().trimmed();
        hrefMain = "3-" + poemID + "-" + vorderMain;
        htmlEntitiesAndNewLine(textMain);
        if(showAllBookmarks && !showBookmark && isBookmarked(database, "3", poemID, vorderMain))
            bookmarkHighlighter(textMain);
        else if(showBookmark && vorderMain == bookmarkVerseID)
            bookmarkHighlighter(textMain);
        if(highlight)
            searchHighlighter(textMain, highlightText, searchHighlightCssClass);

        if(posMain == "0")
        {
            queryIn.exec("SELECT vorder, position, text FROM verse WHERE poem_id = " + poemID + " AND vorder = " + QString::number(++current_row));
            if(queryIn.next())
            {
                vorderNext = queryIn.value(0).toString();
                posNext = queryIn.value(1).toString();
                textNext = queryIn.value(2).toString().trimmed();
                hrefNext = "3-" + poemID + "-" + vorderNext;
                htmlEntitiesAndNewLine(textNext);
                if(showAllBookmarks && !showBookmark && isBookmarked(database, "3", poemID, vorderNext))
                    bookmarkHighlighter(textNext);
                else if(showBookmark && vorderNext == bookmarkVerseID)
                    bookmarkHighlighter(textNext);
                if(highlight)
                    searchHighlighter(textNext, highlightText, searchHighlightCssClass);
            }
        }
        else if(posMain == "2")
        {
            queryIn.exec("SELECT vorder, position, text FROM verse WHERE poem_id = " + poemID + " AND vorder = " + QString::number(current_row + 1));
            if(queryIn.next())
                posNext = queryIn.value(1).toString();
        }

        if(posMain == "0" && posNext != "1")
        {
            textNext = "";
            current_row--;
        }
        else if(posMain == "1")
        {
            textNext = textMain;
            textMain = "";
            hrefNext = hrefMain;
        }
        else if(posMain == "2" && posNext != "3")
        {
            pos2Break = true;
        }

        if(posMain == "0" || posMain == "1")
        {
            strHtml += openTable(isTableOpen) + "<tr>\n";
            strHtml += "<td width=\"50%\" align=\"right\" dir=\"rtl\" style=\"padding-right:" + hemistichDis + "px;\"><a href=\"" + hrefNext + "\">" + textNext + "</a></td>\n";
            strHtml += "<td width=\"0%\">\u200c\u200c\u200c\u200c\u200c</td>\n";
            strHtml += "<td width=\"50%\" align=\"left\" dir=\"rtl\" style=\"padding-left:" + hemistichDis + "px;\"><a href=\"" + hrefMain + "\">" + textMain + "</a></td>\n";
            strHtml += "</tr>\n";
            preBreak = false;
        }
        else if(posMain == "-1")
        {
            strHtml += closeTable(isTableOpen) + "<p align=\"justify\" dir=\"rtl\" style=\"margin-left:0.8em;\"><a href=\"" + hrefMain + "\">" + lBreakAdder(preBreak) + textMain + "<br /></a></p>\n";
            preBreak = true;
        }
        else if(posMain == "2")
        {
            strHtml += closeTable(isTableOpen) + "<p align=\"center\" dir=\"rtl\" style=\"font-weight:bold;\"><a href=\"" + hrefMain + "\">" + lBreakAdder(preBreak) + textMain + (pos2Break ? "<br />" : "") + "</a></p>\n";
            preBreak = pos2Break;
            pos2Break = false;
        }
        else if(posMain == "3")
        {
            strHtml += closeTable(isTableOpen) + "<p align=\"center\" dir=\"rtl\" style=\"font-weight:bold;\"><a href=\"" + hrefMain + "\">" + textMain + "<br /></a></p>\n";
            preBreak = true;
        }
        else if(posMain == "4")
        {
            strHtml += closeTable(isTableOpen) + "<p align=\"right\" dir=\"rtl\"><a href=\"" + hrefMain + "\">" + textMain + "</a></p>\n";
            preBreak = false;
        }
    }
    strHtml += closeTable(isTableOpen) + "</body></html>";

    return strHtml;
}

inline void htmlEntitiesAndNewLine(QString &str)
{
    str.replace("&", "&amp;");  // This line must be first.
    str.replace("<", "&lt;");
    str.replace(">", "&gt;");
    str.replace("\"", "&quot;");
    str.replace("'", "&apos;");

    str.replace("\r\n", "\n");
    str.replace("\r", "\n");
    str = str.trimmed();
    str.replace("\n", "<br />");
}

inline QString openTable(bool &isTableOpen)
{
    if(!isTableOpen)
    {
        isTableOpen = true;
        return "<table align=\"center\" style=\"width:100%;\">\n";
    }
    return "";
}

inline QString closeTable(bool &isTableOpen)
{
    if(isTableOpen)
    {
        isTableOpen = false;
        return "</table>\n";
    }
    return "";
}

inline QString lBreakAdder(bool preBreak)
{
    if(preBreak)
        return "";
    return "<br />";
}

inline void bookmarkHighlighter(QString &str)
{
    str = "<u><b>" + str + "</b></u>";
}

void searchHighlighter(QString &text, const QStringList &list, const QString &cssClass)
{
    QString textCopy(text);
    QStringList listCopy(list);
    QString openTag = QString("<span class=\"%1\">").arg(cssClass);
    QString closeTag = QString("</span>");
    QString diacritics_zwnj = "[" + Constants::DIACRITICS + Constants::ZWNJ + "]*";

    textCopy.replace(Constants::A_TYPES_REGEX, Constants::A_PERSIAN);
    textCopy.replace(Constants::E_TYPES_REGEX, Constants::E_PERSIAN);
    textCopy.replace(Constants::K_TYPES_REGEX, Constants::K_PERSIAN);
    textCopy.replace(Constants::V_TYPES_REGEX, Constants::V_PERSIAN);
    textCopy.replace(Constants::H_TYPES_REGEX, Constants::H_PERSIAN);

    for(int i = 0; i < list.count(); i++)
    {
        listCopy[i].replace(Constants::A_TYPES_REGEX, Constants::A_PERSIAN);
        listCopy[i].replace(Constants::E_TYPES_REGEX, Constants::E_PERSIAN);
        listCopy[i].replace(Constants::K_TYPES_REGEX, Constants::K_PERSIAN);
        listCopy[i].replace(Constants::V_TYPES_REGEX, Constants::V_PERSIAN);
        listCopy[i].replace(Constants::H_TYPES_REGEX, Constants::H_PERSIAN);

        listCopy[i] = listCopy[i].split("").join(diacritics_zwnj);

        QRegularExpression regex(listCopy[i]);
        QRegularExpressionMatch match;
        match = regex.match(textCopy);
        while(match.hasMatch())
        {
            text.insert(match.capturedStart(), openTag);
            text.insert(match.capturedEnd() + openTag.size(), closeTag);
            textCopy.insert(match.capturedStart(), openTag);
            textCopy.insert(match.capturedEnd() + openTag.size(), closeTag);
            match = regex.match(textCopy, match.capturedEnd() + openTag.size() + closeTag.size());
        }
    }
}

QString previousPoem(const QSqlDatabase &database, const QString &level, const QString &id)
{
    QString cat_id;
    QSqlQuery query(database);

    if(level == "3")
    {
        query.exec("SELECT cat_id FROM poem WHERE id = " + id);
        query.next();

        cat_id = query.value(0).toString();

        query.exec(QString("SELECT id FROM poem WHERE cat_id = %1 AND id < %2 ORDER BY id DESC LIMIT 1").arg(cat_id).arg(id));
        if(query.first())
            return "3-" + query.value(0).toString();
    }

    return QString();
}

QString previousPoem(const QSqlDatabase &database, const QString &levelID)
{
    QString level(levelID.left(1)), id(levelID.right(levelID.size() - 2));
    return previousPoem(database, level, id);
}

QString nextPoem(const QSqlDatabase &database, const QString &level, const QString &id)
{
    QString cat_id;
    QSqlQuery query(database);

    if(level == "3")
    {
        query.exec("SELECT cat_id FROM poem WHERE id = " + id);
        query.next();

        cat_id = query.value(0).toString();

        query.exec(QString("SELECT id FROM poem WHERE cat_id = %1 AND id > %2 LIMIT 1").arg(cat_id).arg(id));
        if(query.first())
            return "3-" + query.value(0).toString();
    }

    return QString();
}

QString nextPoem(const QSqlDatabase &database, const QString &levelID)
{
    QString level(levelID.left(1)), id(levelID.right(levelID.size() - 2));
    return nextPoem(database, level, id);
}

bool isBookmarked(const QSqlDatabase &database, const QString &level, const QString &id, const QString &verse_id)
{
    if(level == "3")
    {
        QSqlQuery query(database);
        query.exec(QString("SELECT * FROM fav WHERE poem_id = %1 AND verse_id = %2").arg(id).arg(verse_id));

        if(query.first())
            return true;
    }

    return false;
}

bool isBookmarked(const QSqlDatabase &database, const QString &levelID, const QString &verse_id)
{
    QString level(levelID.left(1)), id(levelID.right(levelID.size() - 2));
    return isBookmarked(database, level, id, verse_id);
}

void setBookmarked(const QSqlDatabase &database, const QString &level, const QString &id, const QString &verse_id, bool value)
{
    if(level == "3")
    {
        bool preValue;
        QSqlQuery query(database);
        query.exec(QString("SELECT * FROM fav WHERE poem_id = %1 AND verse_id = %2").arg(id).arg(verse_id));
        preValue = query.first();

        if(value && !preValue)
        {
            int pos = 0;
            query.exec("SELECT pos FROM fav ORDER BY pos DESC LIMIT 1");
            if(query.first())
                pos = query.value(0).toInt() + 1;

            query.exec(QString("INSERT INTO fav (poem_id, verse_id, pos) VALUES (%1,%2,%3)").arg(id).arg(verse_id).arg(pos));
        }
        else if(!value && preValue)
        {
            query.exec(QString("DELETE FROM fav WHERE poem_id = %1 AND verse_id = %2").arg(id).arg(verse_id));
        }
    }
}

void setBookmarked(const QSqlDatabase &database, const QString &levelID, const QString &verse_id, bool value)
{
    QString level(levelID.left(1)), id(levelID.right(levelID.size() - 2));
    setBookmarked(database, level, id, verse_id, value);
}

bool importBookmarks(QSqlDatabase &mainDatabase, const QSqlDatabase &secondDatabase, int speed)
{
    QSqlQuery queryMainDB(mainDatabase), query(secondDatabase);
    TableValues *tValues = new TableValues;

    query.exec("SELECT poem_id, verse_id, pos FROM fav");
    while(query.next())
    {
        tValues->clear();
        query.previous();
        for(int i = 0; i < speed; i++)
        {
            if(query.next())
                tValues->addValue(QString("(%1,%2,%3)").arg(query.value(0).toInt()).arg(query.value(1).toInt()).arg(query.value(2).toInt()));
            else
                break;
        }
        queryMainDB.exec("INSERT INTO fav (poem_id, verse_id, pos) VALUES " + tValues->getValues());
    }

    delete tValues;
    return true;
}

bool importBookmarks(QSqlDatabase &mainDatabase, const QString &secondDatabase, int speed)
{
    SqliteDB secondDB(secondDatabase, "secondDatabase");
    bool result = importBookmarks(mainDatabase, secondDB.DB(), speed);
    secondDB.remove();

    return result;
}

bool exportBookmarks(const QSqlDatabase &mainDatabase, QSqlDatabase &newDatabase, int speed)
{
    QSqlQuery queryMainDB(mainDatabase), queryNewDB(newDatabase);
    TableValues *tValues = new TableValues;

    queryNewDB.exec("CREATE TABLE fav (poem_id INTEGER, verse_id INTEGER, pos INTEGER)");

    queryMainDB.exec("SELECT poem_id, verse_id, pos FROM fav");
    while(queryMainDB.next())
    {
        tValues->clear();
        queryMainDB.previous();
        for(int i = 0; i < speed; i++)
        {
            if(queryMainDB.next())
                tValues->addValue(QString("(%1,%2,%3)").arg(queryMainDB.value(0).toInt()).arg(queryMainDB.value(1).toInt()).arg(queryMainDB.value(2).toInt()));
            else
                break;
        }
        queryNewDB.exec("INSERT INTO fav (poem_id, verse_id, pos) VALUES " + tValues->getValues());
    }

    delete tValues;
    return true;
}

QString randString(int len)
{
    return QUuid::createUuid().toString().remove(QRegularExpression("\\{|\\}|\\-")).left(len).toUpper();
}

QString createDBDialog(QWidget *parent, const QString &defaultFilePath)
{
    QString filter = "SQLite 3 Database files (*.s3db *.db *.sqlite3 *.sqlite);;Ganjoor Database files (*.gdb);;Database files (*.db)";
    QString file_path = QFileDialog::getSaveFileName(parent, "Save As", defaultFilePath, filter);
    if(!file_path.isEmpty())
    {
        QFileInfo file(file_path);
        QFileInfo dir(file.path());
        if(dir.isWritable())
        {
            if(QFile::exists(file_path) && !isStdGanjoorDB(file_path))
                QFile::remove(file_path);

            if(!QFile::exists(file_path) || !isStdGanjoorDB(file_path))
                createGanjoorTable(file_path);

            return file_path;
        }
        else
        {
            messageBox("خطا", "این مسیر قابل نوشتن نیست! لطفا مسیر دیگری انتخاب کنید.", Warning, parent);
        }
    }
    else
    {
        messageBox("پیام", "لطفا مسیر را به درستی انتخاب کنید!", Warning, parent);
    }

    return QString();
}

QString writableDirDialog(QWidget *parent, const QString &defaultDirPath)
{
    QString dir_path = QFileDialog::getExistingDirectory(parent, "Select Directory", defaultDirPath, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if(!dir_path.isEmpty())
    {
        if(QFileInfo(dir_path).isWritable())
            return dir_path;
        else
            messageBox("خطا", "این مسیر قابل نوشتن نیست! لطفا مسیر دیگری انتخاب کنید.", Warning, parent);
    }
    else
    {
        messageBox("پیام", "لطفا مسیر را به درستی انتخاب کنید!", Warning, parent);
    }

    return QString();
}

QMessageBox::StandardButton messageBox(const QString &title, const QString &text, MessageBoxType messageBoxType, QWidget *parent, QMessageBox::StandardButton defaultButton)
{
    QMessageBox msgBox(parent);

    msgBox.setLayoutDirection(Qt::LeftToRight);
    msgBox.setWindowTitle(title);
    msgBox.setText(text);  // msgBox.setText("<p>" + text + "</p>");

    if(messageBoxType == Information || messageBoxType == Warning || messageBoxType == Critical)
    {
        QPushButton *ok = new QPushButton("تایید");
        msgBox.addButton(ok, QMessageBox::AcceptRole);
        msgBox.setDefaultButton(ok);
        if(messageBoxType == Information)
            msgBox.setIcon(QMessageBox::Information);
        else if(messageBoxType == Warning)
            msgBox.setIcon(QMessageBox::Warning);
        else if(messageBoxType == Critical)
            msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();
        delete ok;
        return QMessageBox::Ok;
    }
    else if(messageBoxType == Question || messageBoxType == WarningQuestion)
    {
        QPushButton *yes = new QPushButton("بله");
        QPushButton *no = new QPushButton("خیر");
        msgBox.addButton(yes, QMessageBox::YesRole);
        msgBox.addButton(no, QMessageBox::NoRole);
        if(defaultButton == QMessageBox::Yes)
            msgBox.setDefaultButton(yes);
        else if(defaultButton == QMessageBox::No)
            msgBox.setDefaultButton(no);
        if(messageBoxType == Question)
            msgBox.setIcon(QMessageBox::Question);
        else if(messageBoxType == WarningQuestion)
            msgBox.setIcon(QMessageBox::Warning);
        int b = msgBox.exec();
        delete yes;
        delete no;
        if(b == 0)
            return QMessageBox::Yes;
        else
            return QMessageBox::No;
    }
    else if(messageBoxType == WarningQuestionToAll)
    {
        QPushButton *yesToAll = new QPushButton("بله همه");
        QPushButton *noToAll = new QPushButton("خیر");
        QPushButton *abort = new QPushButton("لغو");
        msgBox.addButton(yesToAll, QMessageBox::YesRole);
        msgBox.addButton(noToAll, QMessageBox::NoRole);
        msgBox.addButton(abort, QMessageBox::RejectRole);
        if(defaultButton == QMessageBox::YesToAll)
            msgBox.setDefaultButton(yesToAll);
        else if(defaultButton == QMessageBox::NoToAll)
            msgBox.setDefaultButton(noToAll);
        else if(defaultButton == QMessageBox::Abort)
            msgBox.setDefaultButton(abort);
        msgBox.setIcon(QMessageBox::Warning);
        int b = msgBox.exec();
        delete yesToAll;
        delete noToAll;
        delete abort;
        if(b == 0)
            return QMessageBox::YesToAll;
        else if(b == 1)
            return QMessageBox::NoToAll;
        else
            return QMessageBox::Abort;
    }

    return QMessageBox::NoButton;
}

QString byteToHuman(qint64 size)
{
    int base = 1024;

    if(size < base)
        return QString::number(size) + " بایت";
    else if(size < pow(base, 2))
        return QString::number(size / base) + " کیلوبایت";
    else if(size < pow(base, 3))
        return QString::number((float)size / pow(base, 2), 'f', 2) + " مگابایت";
    else if(size < pow(base, 4))
        return QString::number((float)size / pow(base, 3), 'f', 2) + " گیگابایت";
    else if(size < pow(base, 5))
        return QString::number((float)size / pow(base, 4), 'f', 2) + " ترابایت";
    else if(size < pow(base, 6))
        return QString::number((float)size / pow(base, 5), 'f', 2) + " پتابایت";

    return QString::number((float)size / pow(base, 6), 'f', 2) + " اگزابایت";
}

QString ratioFontSize(double size, double ratio)
{
    return QString::number(size * ratio, 'f', 2);
}

QString appStyleSheet(const QString &appFN, const QString &appFS, const QString &listFN, const QString &listFS, const QString &viewFN, const QString &viewFS)
{
    QString str, appFNFD(appFN);
    if(appFN == "Sahel")
        appFNFD += " FD";

    str = "*[accessibleName=classView],*[accessibleName=classViewListHeader]{font-family:" + viewFN + ";font-size:" + viewFS + "pt;}";
    str += "*[accessibleName=class2],QMessageBox,QPushButton,QToolButton,QMenuBar,QMenu{font-family:" + appFN + ";font-size:" + appFS + "pt;}";
    str += "*[accessibleName=class1]{font-family:" + listFN + ";font-size:" + listFS + "pt;}";
    str += "*[accessibleName=classFD],QTabBar,QHeaderView{font-family:'" + appFNFD + "';font-size:" + appFS + "pt;}";
    str += "*[accessibleName=classFDs]{font-family:'" + appFNFD + "';font-size:" + ratioFontSize(appFS.toDouble(), 0.95) + "pt;}";
    str += "*[accessibleName=classFinish]{font-family:" + appFN + ";font-size:" + ratioFontSize(appFS.toDouble(), 1.45) + "pt;font-weight:bold;}";

    return str;
}

bool dbExtCheck(const QString &filePath)
{
    QString file_path = filePath.toLower();
    if(file_path.endsWith(".gdb") ||
       file_path.endsWith(".s3db") ||
       file_path.endsWith(".db") ||
       file_path.endsWith(".sqlite") ||
       file_path.endsWith(".sqlite3"))
        return true;

    return false;
}

bool removeTempDir(const QString &dirName)
{
    QDir qDir(QDir::tempPath() + "/" + dirName);
    return qDir.removeRecursively();
}

QString rosybitDir()
{
    QString str;
#ifdef Q_OS_WIN
    str = QDir::fromNativeSeparators(qgetenv("APPDATA")) + "/" + Constants::Rosybit;
#else
    str = QDir::fromNativeSeparators(qgetenv("HOME")) + "/.config/" + Constants::Rosybit.toLower();
#endif
    return str;
}

QString appNameOS()
{
    QString appName;
#ifdef Q_OS_WIN
    appName = Constants::AppName;
#else
    appName = Constants::AppName.toLower();
#endif
    return appName;
}

QString defaultDBPath()
{
    QStringList dbPath;

    // qDebug().noquote() << QProcessEnvironment::systemEnvironment().value(QStringLiteral("APPIMAGE"));  // AppImage Path
    // qDebug().noquote() << qgetenv("APPIMAGE");  // AppImage Path

    // dbPath << QCoreApplication::applicationDirPath() + "/" + Constants::DefaultDBName;  // This is not work if you are inside an AppImage package.
    dbPath << QDir::currentPath() + "/" + Constants::DefaultDBName;
#ifdef Q_OS_WIN
    dbPath << rosybitDir() + "/" + Constants::AppName + "/" + Constants::DefaultDBName;
#endif
    dbPath << QDir::homePath() + "/" + Constants::AppName + "/" + Constants::DefaultDBName;
    dbPath << QDir::homePath() + "/" + Constants::DefaultDBName;
#ifdef Q_OS_WIN
    dbPath << QDir::fromNativeSeparators(qgetenv("LOCALAPPDATA")) + "/ganjoor/" + Constants::DefaultDBName;
#endif

    for(int i = 0; i < dbPath.count(); i++)
        if(QFile::exists(dbPath[i]) && isStdGanjoorDB(dbPath[i]))
            return dbPath[i];
    return QString();
}

void showFileInDir(const QString &filePath)
{
#if defined Q_OS_WIN
    QProcess::startDetached("explorer.exe", {"/select,", QDir::toNativeSeparators(filePath)});
#elif defined Q_OS_MACOS
    QProcess::execute("/usr/bin/osascript", {"-e", "tell application \"Finder\" to reveal POSIX file \"" + filePath + "\""});
    QProcess::execute("/usr/bin/osascript", {"-e", "tell application \"Finder\" to activate"});
#else
    QDesktopServices::openUrl(QUrl(QFileInfo(filePath).path()));
#endif
}

QString gregorianToPersian(int day, int month, int year, const QString &delimiter, bool dd, bool mm)
{
    int _d = day, _m = month, _y = year;
    gregorian_persian(&_d, &_m, &_y);

    QString d = QString::number(_d);
    QString m = QString::number(_m);

    if(dd && _d < 10)
        d = "0" + d;
    if(mm && _m < 10)
        m = "0" + m;

    return QString("%1%2%3%4%5").arg(_y).arg(delimiter).arg(m).arg(delimiter).arg(d);
}

QString nowDate(const QString &delimiter, bool dd, bool mm)
{
    QDate date(QDate::currentDate());
    return gregorianToPersian(date.day(), date.month(), date.year(), delimiter, dd, mm);
}

QString nowTime(const QString &delimiter)
{
    return QTime::currentTime().toString(QString("HH%1mm%1ss").arg(delimiter));
}

QString correctHtmlTableText(const QString &text)
{
    QString str(text);

//    QRegExp regex("([^\\n]*)\\n\u200c\u200c\u200c\u200c\u200c\\n([^\\n]*)");
//    int pos = 0;
//    while((pos = regex.indexIn(str, pos)) != -1)
//        str.replace(pos, regex.cap(0).size(), regex.cap(2) + "\n" + regex.cap(1) + "\n");
//    str.replace(QRegExp("\\n{3,}"), "\n\n");
//    str = str.trimmed();

    QRegularExpression regex("([^\\n]*)\\n\u200c\u200c\u200c\u200c\u200c\\n([^\\n]*)");
    QRegularExpressionMatch match = regex.match(str);
    while(match.hasMatch())
    {
        str.replace(match.capturedStart(0), match.capturedLength(0), match.captured(2) + "\n" + match.captured(1) + "\n");
        match = regex.match(str);
    }
    str.replace(QRegularExpression("\\n{3,}"), "\n\n");
    str = str.trimmed();

    return str;
}

QString persianNumber(int n)
{
    QString number(QString::number(n));
    number.replace("0", "۰");
    number.replace("1", "۱");
    number.replace("2", "۲");
    number.replace("3", "۳");
    number.replace("4", "۴");
    number.replace("5", "۵");
    number.replace("6", "۶");
    number.replace("7", "۷");
    number.replace("8", "۸");
    number.replace("9", "۹");
    return number;
}

bool idComp(const QString &id1, const QString &id2)
{
    return id1.toInt() < id2.toInt();
}

bool faLessThan(const QString &str1, const QString &str2)
{
    return str1 < str2;
}

QString faReplaceChars(const QString &text)
{
    QString str(text);

    str.replace("ک", "ك");
    str.replace("ی", "ي");

    str.replace("پ", "بييييي");
    str.replace("چ", "جييييي");
    str.replace("ژ", "زييييي");
    str.replace("گ", "كييييي");

    str.replace("و", "_*v*_");
    str.replace("ه", "و");
    str.replace("_*v*_", "ه");

    return str;
}

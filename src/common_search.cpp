/*
    [Ghazal: The library of persian poetry]
    Publisher: Rosybit
    Url: http://www.rosybit.com
    GitHub: https://github.com/abroshan39/ghazal
    Author: Aboutaleb Roshan [ab.roshan39@gmail.com]
    License: MIT License
*/

#include "common_functions.h"

// ATTENTION: MAX NESTED REPLACE OF SQL QUERY = 29

const QString Constants::SQL_ID_FILTER = "${SQL_ID_FILTER}";
const QString Constants::MARKER_COUNTER = "#";  // If you want to change this constant, do it carefully, because some characters must be escaped in regex. Also change it in the search examples form.
const QString Constants::MARKER_RADIF = "==";   // If you want to change this constant, do it carefully, because some characters must be escaped in regex. Also change it in the search examples form.
const QString Constants::MARKER_GHAFIE = "=";   // If you want to change this constant, do it carefully, because some characters must be escaped in regex. Also change it in the search examples form.

const QChar Constants::ZWNJ = QChar(0x200C);
const QRegularExpression Constants::ZWNJ_REGEX = QRegularExpression(QString("[%1]").arg(Constants::ZWNJ));

// A << E << O << SOKUN << AN << EN << ON << TASHDID << HAMZE << ALEF_KHANJARI << KESHIDE
const QString Constants::DIACRITICS = "\u064E\u0650\u064F\u0652\u064B\u064D\u064C\u0651\u0654\u0670\u0640";
const QRegularExpression Constants::DIACRITICS_REGEX = QRegularExpression(QString("[%1]").arg(Constants::DIACRITICS));

const QString Constants::A_PERSIAN = "\u0627";
const QString Constants::E_PERSIAN = "\u06CC";
const QString Constants::K_PERSIAN = "\u06A9";
const QString Constants::V_PERSIAN = "\u0648";
const QString Constants::H_PERSIAN = "\u0647";

const QString Constants::A_TYPES = "\u0622\u0623\u0625";
const QString Constants::E_TYPES = "\u064A\u0626\u0649";
const QString Constants::K_TYPES = "\u0643";
const QString Constants::V_TYPES = "\u0624";
const QString Constants::H_TYPES = "\u0629\u06C0";

const QRegularExpression Constants::A_TYPES_REGEX = QRegularExpression(QString("[%1]").arg(Constants::A_TYPES));
const QRegularExpression Constants::E_TYPES_REGEX = QRegularExpression(QString("[%1]").arg(Constants::E_TYPES));
const QRegularExpression Constants::K_TYPES_REGEX = QRegularExpression(QString("[%1]").arg(Constants::K_TYPES));
const QRegularExpression Constants::V_TYPES_REGEX = QRegularExpression(QString("[%1]").arg(Constants::V_TYPES));
const QRegularExpression Constants::H_TYPES_REGEX = QRegularExpression(QString("[%1]").arg(Constants::H_TYPES));

const QRegularExpression Constants::A_REGEX = QRegularExpression(QString("[%1]").arg(Constants::A_TYPES + Constants::A_PERSIAN));
const QRegularExpression Constants::E_REGEX = QRegularExpression(QString("[%1]").arg(Constants::E_TYPES + Constants::E_PERSIAN));
const QRegularExpression Constants::K_REGEX = QRegularExpression(QString("[%1]").arg(Constants::K_TYPES + Constants::K_PERSIAN));
const QRegularExpression Constants::V_REGEX = QRegularExpression(QString("[%1]").arg(Constants::V_TYPES + Constants::V_PERSIAN));
const QRegularExpression Constants::H_REGEX = QRegularExpression(QString("[%1]").arg(Constants::H_TYPES + Constants::H_PERSIAN));

const QString Constants::OTHER_CHARS = "()?؟!«»،؛.";
const QRegularExpression Constants::OTHER_CHARS_REGEX = QRegularExpression(QString("[%1]").arg(Constants::OTHER_CHARS));

QString searchTableView(QStandardItemModel *model, const QSqlDatabase &database, InSearchSettings *inSearchSettings, SearchMethod searchMethod, bool allItemsSelected, const QStringList &poetIDList, const QString &strQuery, const QString &userStr, SearchTable searchTable, bool sDiacritics, bool sCharTypes)
{
    if(searchMethod == SearchMethod::Method1)
        return sTVMethod1(model, database, inSearchSettings, strQuery, userStr, searchTable, sDiacritics, sCharTypes);
    if(searchMethod == SearchMethod::Method2)
        return sTVMethod2(model, database, inSearchSettings, allItemsSelected, poetIDList, strQuery, userStr, searchTable, sDiacritics, sCharTypes);
    return QString();
}

QString sTVMethod1(QStandardItemModel *model, const QSqlDatabase &database, InSearchSettings *inSearchSettings, const QString &strQuery, const QString &userStr, SearchTable searchTable, bool sDiacritics, bool sCharTypes)
{
    inSearchSettings->isSearching = true;

    QString searchPhrase(userStr);
    QString level;
    QStringList colList;
    QSqlQuery query(database);
    query.exec(strQuery);

    searchPhrase = removeZWNJ(searchPhrase);
    if(sDiacritics)
        searchPhrase = removeDiacritics(searchPhrase);
    if(sCharTypes)
        searchPhrase = removeCharTypes(searchPhrase);

    QList<SearchWord> swList;
    QString counterWord(counterMarkerFinder(searchPhrase));
    if(counterWord.isEmpty())
    {
        QStringList listOr = searchPhrase.split("|");
        for(int i = 0; i < listOr.count(); i++)
        {
            SearchWord sw = searchPhraseParser(listOr[i]);
            swList.append(sw);
        }
    }

    if(searchTable == VerseTable)
    {
        colList << "عنوان" << "متن";
        level = "3";
    }
    else if(searchTable == PoemTable)
    {
        colList << "نام شاعر یا نویسنده" << "عنوان";
        level = "3";
    }
    else if(searchTable == CatTable)
    {
        colList << "نام شاعر یا نویسنده" << "فهرست";
        level = "2";
    }

    model->setHorizontalHeaderLabels(colList);

    int count = 0;
    int row_count = 0;
    while(inSearchSettings->isSearching && query.next())
    {
        { // Filtering Block
            QString text = query.value(1).toString();
            if(sDiacritics)
                text = removeDiacritics(text);
            if(sCharTypes)
                text = removeCharTypes(text);

            if(counterWord.isEmpty())
            {
                if(!patternMatched(swList, text))
                    continue;
            }
            else
            {
                int n = wordCount(counterWord, text);
                count += n;
                if(n < 2)
                    continue;
            }
        } // Filtering Block

        QString id = query.value(0).toString();
        GanjoorPath gp = recursiveIDs(database, level, id);
        int iLast = gp.text.count() - 1;
        QString str = gp.text[iLast] + ": " + gp.text[0];
        QString item1Text, item2Text;

        if(searchTable == VerseTable)
        {
            item1Text = spaceReplace(str, "…", 6);
            item2Text = query.value(1).toString();
        }
        else if(searchTable == PoemTable)
        {
            item1Text = gp.text[iLast];
            item2Text = query.value(1).toString();
        }
        else if(searchTable == CatTable)
        {
            item1Text = gp.text[iLast];
            if(iLast > 1)
            {
                QString preCat;
                for(int i = iLast - 1; i > 0; i--)
                {
                    if(preCat.isEmpty())
                        preCat = gp.text[i];
                    else
                        preCat += " | " + gp.text[i];
                }
                item2Text = QString("%1 (%2)").arg(query.value(1).toString(), preCat);
            }
            else
            {
                item2Text = query.value(1).toString();
            }
        }

        QStandardItem *item1 = new QStandardItem(item1Text);
        QStandardItem *item2 = new QStandardItem(item2Text);
        item1->setData(gp.text[iLast], Qt::UserRole);
        item2->setData(level + "-" + id + (searchTable == VerseTable ? "-" + query.value(2).toString() : ""), Qt::UserRole);
        model->setItem(row_count, 0, item1);
        model->setItem(row_count, 1, item2);

        row_count++;
    }

    inSearchSettings->isSearching = false;
    if(!counterWord.isEmpty())
        return QString("تعداد کل کلمه (عبارت): <b>%1</b><br />برابر است با: <b>%2</b><br />کلمه‌هایی (عبارت‌هایی) که بیش از یک بار (دو یا بیشتر) در یک رکورد وجود دارد، در جدول قرار گرفت.").arg(counterWord, persianNumber(count));
    return QString();
}

QString sTVMethod2(QStandardItemModel *model, const QSqlDatabase &database, InSearchSettings *inSearchSettings, bool allItemsSelected, const QStringList &poetIDList, const QString &strQuery, const QString &userStr, SearchTable searchTable, bool sDiacritics, bool sCharTypes)
{
    inSearchSettings->isSearching = true;

    QString searchPhrase(userStr);
    QString level;
    QStringList colList;
    QSqlQuery query(database), queryCat(database), queryPoem(database);
    QString strQueryHalf1 = strQuery.mid(0, strQuery.indexOf(Constants::SQL_ID_FILTER));
    QString strQueryHalf2 = strQuery.mid(strQuery.indexOf(Constants::SQL_ID_FILTER) + Constants::SQL_ID_FILTER.size());

    searchPhrase = removeZWNJ(searchPhrase);
    if(sDiacritics)
        searchPhrase = removeDiacritics(searchPhrase);
    if(sCharTypes)
        searchPhrase = removeCharTypes(searchPhrase);

    QList<SearchWord> swList;
    QString counterWord(counterMarkerFinder(searchPhrase));
    if(counterWord.isEmpty())
    {
        QStringList listOr = searchPhrase.split("|");
        for(int i = 0; i < listOr.count(); i++)
        {
            SearchWord sw = searchPhraseParser(listOr[i]);
            swList.append(sw);
        }
    }

    if(searchTable == VerseTable)
    {
        colList << "عنوان" << "متن";
        level = "3";
    }
    else if(searchTable == PoemTable)
    {
        colList << "نام شاعر یا نویسنده" << "عنوان";
        level = "3";
    }
    else if(searchTable == CatTable)
    {
        colList << "نام شاعر یا نویسنده" << "فهرست";
        level = "2";
    }

    model->setHorizontalHeaderLabels(colList);

    QStringList poetIDListSearch(poetIDList);
    if(allItemsSelected)
    {
        poetIDListSearch.clear();
        QSqlQuery querySearchRange("SELECT id FROM poet ORDER BY id", database);
        while(querySearchRange.next())
            poetIDListSearch << querySearchRange.value(0).toString();
    }

    int count = 0;
    int row_count = 0;
    for(int i = 0; inSearchSettings->isSearching && i < poetIDListSearch.count(); i++)
    {
        QString poetID(poetIDListSearch[i]);
        if(searchTable == CatTable)
        {
            query.exec(strQueryHalf1 + poetID + strQueryHalf2);
            while(inSearchSettings->isSearching && query.next())
                addSearchTableItem(model, database, row_count, count, searchTable, level, query.value(0).toString(), query.value(1).toString(), QString(), counterWord, swList, sDiacritics, sCharTypes);
        }
        else
        {
            queryCat.exec(QString("SELECT id FROM cat WHERE poet_id = %1 ORDER BY id").arg(poetID));
            while(inSearchSettings->isSearching && queryCat.next())
            {
                QString catID(queryCat.value(0).toString());
                if(searchTable == PoemTable)
                {
                    query.exec(strQueryHalf1 + catID + strQueryHalf2);
                    while(inSearchSettings->isSearching && query.next())
                        addSearchTableItem(model, database, row_count, count, searchTable, level, query.value(0).toString(), query.value(1).toString(), QString(), counterWord, swList, sDiacritics, sCharTypes);
                }
                else
                {
                    queryPoem.exec(QString("SELECT id FROM poem WHERE cat_id = %1 ORDER BY id").arg(catID));
                    while(inSearchSettings->isSearching && queryPoem.next())
                    {
                        QString poemID(queryPoem.value(0).toString());
                        query.exec(strQueryHalf1 + poemID + strQueryHalf2);
                        while(inSearchSettings->isSearching && query.next())
                            addSearchTableItem(model, database, row_count, count, searchTable, level, query.value(0).toString(), query.value(1).toString(), query.value(2).toString(), counterWord, swList, sDiacritics, sCharTypes);
                    }
                }
            }
        }
    }

    inSearchSettings->isSearching = false;
    if(!counterWord.isEmpty())
        return QString("تعداد کل کلمه (عبارت): <b>%1</b><br />برابر است با: <b>%2</b><br />کلمه‌هایی (عبارت‌هایی) که بیش از یک بار (دو یا بیشتر) در یک رکورد وجود دارد، در جدول قرار گرفت.").arg(counterWord, persianNumber(count));
    return QString();
}

void searchRadifTableView(QStandardItemModel *model, const QSqlDatabase &database, InSearchSettings *inSearchSettings, bool allItemsSelected, const QStringList &poetIDList, const QString &userStr, bool sDiacritics, bool sCharTypes)
{
    inSearchSettings->isSearching = true;

    QString userStrPure(QString(userStr).remove("\""));
    QRegularExpression regex_marker(QString("^\\s*%1\\s*([^\\s]+.*)").arg(Constants::MARKER_RADIF));
    QRegularExpressionMatch match = regex_marker.match(userStrPure);
    if(match.hasMatch())
        userStrPure = match.captured(1).trimmed();

    bool exact = userStr.contains(QRegularExpression(QString("^\\s*%1\\s*\\\"([^\\\"]+)\\\"").arg(Constants::MARKER_RADIF)));

    QString searchPhrase(userStrPure), purePhrase(userStrPure);
    QStringList colList(QStringList() << "عنوان" << "متن");
    QSqlQuery queryCat(database), queryPoem(database), queryVerse(database);
    QSqlQuery query0c(database), query1c(database), query1p(database);
    QString fieldStr("text");

    if(sDiacritics)
    {
        searchPhrase = removeDiacritics(searchPhrase);
        searchPhrase = removeZWNJ(searchPhrase);
        fieldStr = skipDiacritics(fieldStr);
        fieldStr = skipZWNJ(fieldStr);
    }
    if(sCharTypes)
        searchPhrase = replace_AEKVH_withUnderscore(searchPhrase);

    purePhrase = removeOtherChars(removeCharTypes(removeDiacritics(removeZWNJ(purePhrase)))).trimmed();

    QString strRegex;
    if(exact)
        strRegex = QString("\\b%1\\b$").arg(purePhrase);
    else
        strRegex = QString("%1$").arg(purePhrase);

    QRegularExpression regex(strRegex, QRegularExpression::UseUnicodePropertiesOption | QRegularExpression::DotMatchesEverythingOption | QRegularExpression::MultilineOption);

    model->setHorizontalHeaderLabels(colList);

    QStringList poetIDListSearch(poetIDList);
    if(allItemsSelected)
    {
        poetIDListSearch.clear();
        QSqlQuery querySearchRange("SELECT id FROM poet ORDER BY id", database);
        while(querySearchRange.next())
            poetIDListSearch << querySearchRange.value(0).toString();
    }

    int row_count = 0;
    for(int i = 0; inSearchSettings->isSearching && i < poetIDListSearch.count(); i++)
    {
        QString poetID(poetIDListSearch[i]);
        queryCat.exec(QString("SELECT id FROM cat WHERE poet_id = %1 ORDER BY id").arg(poetID));
        while(inSearchSettings->isSearching && queryCat.next())
        {
            QString catID(queryCat.value(0).toString());
            queryPoem.exec(QString("SELECT id FROM poem WHERE cat_id = %1 ORDER BY id").arg(catID));
            while(inSearchSettings->isSearching && queryPoem.next())
            {
                QSet<int> vorders1cRegistered;
                QString poemID(queryPoem.value(0).toString());
                queryVerse.exec(QString("SELECT vorder, position FROM verse WHERE poem_id = %1 AND %2 LIKE '%%3%' AND (position = 1 OR position = 3) ORDER BY vorder").arg(poemID, fieldStr, searchPhrase));
                while(inSearchSettings->isSearching && queryVerse.next())
                {
                    int vorder1c = queryVerse.value(0).toInt();
                    int vorder0c = vorder1c - 1;
                    int vorder1p = vorder1c - 2;
                    int position = queryVerse.value(1).toInt();

                    query1c.exec(QString("SELECT text FROM verse WHERE poem_id = %1 AND vorder = %2 AND position = %3").arg(poemID).arg(vorder1c).arg(position));
                    query0c.exec(QString("SELECT text FROM verse WHERE poem_id = %1 AND vorder = %2 AND position = %3").arg(poemID).arg(vorder0c).arg(position - 1));
                    query1p.exec(QString("SELECT text FROM verse WHERE poem_id = %1 AND vorder = %2 AND position = %3").arg(poemID).arg(vorder1p).arg(position));

                    QString text1c, text0c, text1p;
                    if(query1c.first())
                        text1c = query1c.value(0).toString();
                    if(query0c.first())
                        text0c = query0c.value(0).toString();
                    if(query1p.first())
                        text1p = query1p.value(0).toString();

                    text1c = removeOtherChars(removeCharTypes(removeDiacritics(removeZWNJ(text1c)))).trimmed();
                    text0c = removeOtherChars(removeCharTypes(removeDiacritics(removeZWNJ(text0c)))).trimmed();
                    text1p = removeOtherChars(removeCharTypes(removeDiacritics(removeZWNJ(text1p)))).trimmed();

                    if(!text1c.contains(regex))
                        text1c.clear();
                    if(!text0c.contains(regex))
                        text0c.clear();
                    if(!text1p.contains(regex))
                        text1p.clear();

                    if(!text1c.isEmpty() && !text0c.isEmpty() && !text1p.isEmpty())
                    {
                        if(!vorders1cRegistered.contains(vorder1p))
                            addSearchTableItemVerse(model, database, row_count, poemID, vorder1p, query1p.value(0).toString());
                        addSearchTableItemVerse(model, database, row_count, poemID, vorder0c, query0c.value(0).toString());
                        addSearchTableItemVerse(model, database, row_count, poemID, vorder1c, query1c.value(0).toString());
                        vorders1cRegistered << vorder1c;
                    }
                    else if(!text1c.isEmpty() && !text0c.isEmpty() && text1p.isEmpty())
                    {
                        addSearchTableItemVerse(model, database, row_count, poemID, vorder0c, query0c.value(0).toString());
                        addSearchTableItemVerse(model, database, row_count, poemID, vorder1c, query1c.value(0).toString());
                        vorders1cRegistered << vorder1c;
                    }
                    else if(!text1c.isEmpty() && text0c.isEmpty() && !text1p.isEmpty())
                    {
                        if(!vorders1cRegistered.contains(vorder1p))
                            addSearchTableItemVerse(model, database, row_count, poemID, vorder1p, query1p.value(0).toString());
                        addSearchTableItemVerse(model, database, row_count, poemID, vorder1c, query1c.value(0).toString());
                        vorders1cRegistered << vorder1c;
                    }
                }
            }
        }
    }

    inSearchSettings->isSearching = false;
}

void searchGhafieTableView(QStandardItemModel *model, const QSqlDatabase &database, InSearchSettings *inSearchSettings, bool allItemsSelected, const QStringList &poetIDList, const QString &userStr, bool sDiacritics, bool sCharTypes)
{
    inSearchSettings->isSearching = true;

    QString userStrPure(QString(userStr).remove("\""));
    QRegularExpression regex_marker(QString("^\\s*%1\\s*([^\\s]+.*)").arg(Constants::MARKER_GHAFIE));
    QRegularExpressionMatch match = regex_marker.match(userStrPure);
    if(match.hasMatch())
        userStrPure = match.captured(1).trimmed();

    bool exact = userStr.contains(QRegularExpression(QString("^\\s*%1\\s*\\\"([^\\\"]+)\\\"").arg(Constants::MARKER_GHAFIE)));

    QString searchPhrase(userStrPure), purePhrase(userStrPure);
    QStringList colList(QStringList() << "عنوان" << "متن");
    QSqlQuery queryCat(database), queryPoem(database), queryVerse(database);
    QString fieldStr("text");

    if(sDiacritics)
    {
        searchPhrase = removeDiacritics(searchPhrase);
        searchPhrase = removeZWNJ(searchPhrase);
        fieldStr = skipDiacritics(fieldStr);
        fieldStr = skipZWNJ(fieldStr);
    }
    if(sCharTypes)
        searchPhrase = replace_AEKVH_withUnderscore(searchPhrase);

    purePhrase = removeOtherChars(removeCharTypes(removeDiacritics(removeZWNJ(purePhrase)))).trimmed();

    QRegularExpression regex_exact(QString("\\b%1\\b").arg(purePhrase), QRegularExpression::UseUnicodePropertiesOption | QRegularExpression::DotMatchesEverythingOption | QRegularExpression::MultilineOption);

    model->setHorizontalHeaderLabels(colList);

    QStringList poetIDListSearch(poetIDList);
    if(allItemsSelected)
    {
        poetIDListSearch.clear();
        QSqlQuery querySearchRange("SELECT id FROM poet ORDER BY id", database);
        while(querySearchRange.next())
            poetIDListSearch << querySearchRange.value(0).toString();
    }

    int row_count = 0;
    for(int i = 0; inSearchSettings->isSearching && i < poetIDListSearch.count(); i++)
    {
        QString poetID(poetIDListSearch[i]);
        queryCat.exec(QString("SELECT id FROM cat WHERE poet_id = %1 ORDER BY id").arg(poetID));
        while(inSearchSettings->isSearching && queryCat.next())
        {
            QString catID(queryCat.value(0).toString());
            queryPoem.exec(QString("SELECT id FROM poem WHERE cat_id = %1 ORDER BY id").arg(catID));
            while(inSearchSettings->isSearching && queryPoem.next())
            {
                QString poemID(queryPoem.value(0).toString());
                queryVerse.exec(QString("SELECT vorder, position, text FROM verse WHERE poem_id = %1 AND %2 LIKE '%%3%' ORDER BY vorder").arg(poemID, fieldStr, searchPhrase));
                while(inSearchSettings->isSearching && queryVerse.next())
                {
                    int vorder = queryVerse.value(0).toInt();
                    int position = queryVerse.value(1).toInt();
                    QString text = removeOtherChars(removeCharTypes(removeDiacritics(removeZWNJ(queryVerse.value(2).toString())))).trimmed();

                    if(exact && !text.contains(regex_exact))
                        continue;

                    if(position == 0 || position == 2)
                    {
                        QRegularExpression regex_verse(purePhrase, QRegularExpression::UseUnicodePropertiesOption | QRegularExpression::DotMatchesEverythingOption | QRegularExpression::MultilineOption);
                        QRegularExpressionMatch match_verse = regex_verse.match(text, text.lastIndexOf(purePhrase));
                        if(match_verse.hasMatch())
                        {
                            QString phrase1, phrase2;
                            phrase1 = text.mid(match_verse.capturedStart()).trimmed();
                            phrase2 = text.mid(match_verse.capturedEnd()).trimmed();

                            if(isRadif(database, poemID, vorder, position, phrase2) && !isRadif(database, poemID, vorder, position, phrase1))
                            {
                                addSearchTableItemVerse(model, database, row_count, poemID, vorder, queryVerse.value(2).toString());
                            }
                            else if(phrase2.isEmpty() && !isRadif(database, poemID, vorder, position, phrase1))
                            {
                                if(isGhafieLeft(database, poemID, vorder + 1, position + 1, purePhrase))
                                    addSearchTableItemVerse(model, database, row_count, poemID, vorder, queryVerse.value(2).toString());
                            }
                        }
                    }
                    else if(position == 1 || position == 3)
                    {
                        QRegularExpression regex_verse(purePhrase, QRegularExpression::UseUnicodePropertiesOption | QRegularExpression::DotMatchesEverythingOption | QRegularExpression::MultilineOption);
                        QRegularExpressionMatch match_verse = regex_verse.match(text);
                        while(match_verse.hasMatch())
                        {
                            QString phrase1, phrase2;
                            phrase1 = text.mid(match_verse.capturedStart()).trimmed();
                            phrase2 = text.mid(match_verse.capturedEnd()).trimmed();

                            if((phrase2.isEmpty() && !isRadif(database, poemID, vorder, position, phrase1)) ||
                               (isRadif(database, poemID, vorder, position, phrase2) && !isRadif(database, poemID, vorder, position, phrase1)))
                            {
                                addSearchTableItemVerse(model, database, row_count, poemID, vorder, queryVerse.value(2).toString());
                                break;
                            }
                            match_verse = regex_verse.match(text, match_verse.capturedEnd());
                        }
                    }
                }
            }
        }
    }

    inSearchSettings->isSearching = false;
}

bool isRadif(const QSqlDatabase &database, const QString &poemID, int vorder, int position, const QString &purePhrase)
{
    if(purePhrase.isEmpty())
        return false;

    if(position == 0 || position == 2)
    {
        QSqlQuery query0c(database), query1c(database);
        QRegularExpression regex(QString("%1$").arg(purePhrase), QRegularExpression::UseUnicodePropertiesOption | QRegularExpression::DotMatchesEverythingOption | QRegularExpression::MultilineOption);

        int vorder1c = vorder + 1;
        int vorder0c = vorder;

        query1c.exec(QString("SELECT text FROM verse WHERE poem_id = %1 AND vorder = %2 AND position = %3").arg(poemID).arg(vorder1c).arg(position + 1));
        query0c.exec(QString("SELECT text FROM verse WHERE poem_id = %1 AND vorder = %2 AND position = %3").arg(poemID).arg(vorder0c).arg(position));

        QString text1c, text0c;
        if(query1c.first())
            text1c = query1c.value(0).toString();
        if(query0c.first())
            text0c = query0c.value(0).toString();

        text1c = removeOtherChars(removeCharTypes(removeDiacritics(removeZWNJ(text1c)))).trimmed();
        text0c = removeOtherChars(removeCharTypes(removeDiacritics(removeZWNJ(text0c)))).trimmed();

        if(!text1c.contains(regex))
            text1c.clear();
        if(!text0c.contains(regex))
            text0c.clear();

        if(!text1c.isEmpty() && !text0c.isEmpty())
            return true;
    }
    else if(position == 1 || position == 3)
    {
        QSqlQuery query0c(database), query1c(database), query1p(database), query1n(database);
        QRegularExpression regex(QString("%1$").arg(purePhrase), QRegularExpression::UseUnicodePropertiesOption | QRegularExpression::DotMatchesEverythingOption | QRegularExpression::MultilineOption);

        int vorder1c = vorder;
        int vorder0c = vorder - 1;
        int vorder1p = vorder - 2;
        int vorder1n = vorder + 2;

        query1c.exec(QString("SELECT text FROM verse WHERE poem_id = %1 AND vorder = %2 AND position = %3").arg(poemID).arg(vorder1c).arg(position));
        query0c.exec(QString("SELECT text FROM verse WHERE poem_id = %1 AND vorder = %2 AND position = %3").arg(poemID).arg(vorder0c).arg(position - 1));
        query1p.exec(QString("SELECT text FROM verse WHERE poem_id = %1 AND vorder = %2 AND position = %3").arg(poemID).arg(vorder1p).arg(position));
        query1n.exec(QString("SELECT text FROM verse WHERE poem_id = %1 AND vorder = %2 AND position = %3").arg(poemID).arg(vorder1n).arg(position));

        QString text1c, text0c, text1p, text1n;
        if(query1c.first())
            text1c = query1c.value(0).toString();
        if(query0c.first())
            text0c = query0c.value(0).toString();
        if(query1p.first())
            text1p = query1p.value(0).toString();
        if(query1n.first())
            text1n = query1n.value(0).toString();

        text1c = removeOtherChars(removeCharTypes(removeDiacritics(removeZWNJ(text1c)))).trimmed();
        text0c = removeOtherChars(removeCharTypes(removeDiacritics(removeZWNJ(text0c)))).trimmed();
        text1p = removeOtherChars(removeCharTypes(removeDiacritics(removeZWNJ(text1p)))).trimmed();
        text1n = removeOtherChars(removeCharTypes(removeDiacritics(removeZWNJ(text1n)))).trimmed();

        if(!text1c.contains(regex))
            text1c.clear();
        if(!text0c.contains(regex))
            text0c.clear();
        if(!text1p.contains(regex))
            text1p.clear();
        if(!text1n.contains(regex))
            text1n.clear();

        if(!text1c.isEmpty() && (!text0c.isEmpty() || !text1p.isEmpty() || !text1n.isEmpty()))
            return true;
    }

    return false;
}

bool isGhafieLeft(const QSqlDatabase &database, const QString &poemID, int vorder, int position, const QString &purePhrase)
{
    if(purePhrase.isEmpty())
        return false;

    if(position == 1 || position == 3)
    {
        QSqlQuery query(database);
        query.exec(QString("SELECT text FROM verse WHERE poem_id = %1 AND vorder = %2 AND position = %3").arg(poemID).arg(vorder).arg(position));

        QString text;
        if(query.first())
            text = query.value(0).toString();

        text = removeOtherChars(removeCharTypes(removeDiacritics(removeZWNJ(text)))).trimmed();

        QString lastWord = text.mid(text.lastIndexOf(" ") + 1);

        if(purePhrase.at(purePhrase.size() - 1) == lastWord.at(lastWord.size() - 1) &&
           purePhrase.at(purePhrase.size() - 2) == lastWord.at(lastWord.size() - 2))
            return true;

        if(purePhrase.at(purePhrase.size() - 1) == lastWord.at(lastWord.size() - 1) &&
           purePhrase.size() == lastWord.size())
            return true;
    }

    return false;
}

void addSearchTableItem(QStandardItemModel *model, const QSqlDatabase &database, int &row_count, int &count, SearchTable searchTable, const QString &level, const QString &id, const QString &text, const QString &vorder, const QString &counterWord, const QList<SearchWord> &swList, bool sDiacritics, bool sCharTypes)
{
    { // Filtering Block
        QString textTemp = text;
        if(sDiacritics)
            textTemp = removeDiacritics(textTemp);
        if(sCharTypes)
            textTemp = removeCharTypes(textTemp);

        if(counterWord.isEmpty())
        {
            if(!patternMatched(swList, textTemp))
                return;
        }
        else
        {
            int n = wordCount(counterWord, textTemp);
            count += n;
            if(n < 2)
                return;
        }
    } // Filtering Block

    GanjoorPath gp = recursiveIDs(database, level, id);
    int iLast = gp.text.count() - 1;
    QString str = gp.text[iLast] + ": " + gp.text[0];
    QString item1Text, item2Text;

    if(searchTable == VerseTable)
    {
        item1Text = spaceReplace(str, "…", 6);
        item2Text = text;
    }
    else if(searchTable == PoemTable)
    {
        item1Text = gp.text[iLast];
        item2Text = text;
    }
    else if(searchTable == CatTable)
    {
        item1Text = gp.text[iLast];
        if(iLast > 1)
        {
            QString preCat;
            for(int i = iLast - 1; i > 0; i--)
            {
                if(preCat.isEmpty())
                    preCat = gp.text[i];
                else
                    preCat += " | " + gp.text[i];
            }
            item2Text = QString("%1 (%2)").arg(text, preCat);
        }
        else
        {
            item2Text = text;
        }
    }

    QStandardItem *item1 = new QStandardItem(item1Text);
    QStandardItem *item2 = new QStandardItem(item2Text);
    item1->setData(gp.text[iLast], Qt::UserRole);
    item2->setData(level + "-" + id + (searchTable == VerseTable ? "-" + vorder : ""), Qt::UserRole);
    model->setItem(row_count, 0, item1);
    model->setItem(row_count, 1, item2);

    row_count++;
}

void addSearchTableItemVerse(QStandardItemModel *model, const QSqlDatabase &database, int &row_count, const QString &poemID, int vorder, const QString &text)
{
    QString level("3");
    GanjoorPath gp = recursiveIDs(database, level, poemID);
    int iLast = gp.text.count() - 1;
    QString str = gp.text[iLast] + ": " + gp.text[0];

    QStandardItem *item1 = new QStandardItem(spaceReplace(str, "…", 6));
    QStandardItem *item2 = new QStandardItem(text);
    item1->setData(gp.text[iLast], Qt::UserRole);
    item2->setData(QString("%1-%2-%3").arg(level, poemID).arg(vorder), Qt::UserRole);
    model->setItem(row_count, 0, item1);
    model->setItem(row_count, 1, item2);

    row_count++;
}

bool patternMatched(const QList<SearchWord> &swList, const QString &text)
{
    QString str(removeZWNJ(text));
    QString strRegex;
    bool continueFlag;

    for(int i = 0; i < swList.count(); i++)
    {
        continueFlag = false;

        if(!swList[i].orderExact.isEmpty())
        {
            for(int j = 0; j < swList[i].orderExact.count(); j++)
            {
                if(j)
                    strRegex += QString(".*\\b%1\\b").arg(swList[i].orderExact[j]);
                else
                    strRegex = QString("\\b%1\\b").arg(swList[i].orderExact[j]);
            }
            QRegularExpression regex(strRegex, QRegularExpression::UseUnicodePropertiesOption | QRegularExpression::DotMatchesEverythingOption | QRegularExpression::MultilineOption);
            if(!str.contains(regex))
                continue;
        }

        if(!swList[i].order.isEmpty())
        {
            for(int j = 0; j < swList[i].order.count(); j++)
            {
                if(j)
                    strRegex += QString(".*%1").arg(swList[i].order[j]);
                else
                    strRegex = QString("%1").arg(swList[i].order[j]);
            }
            QRegularExpression regex(strRegex, QRegularExpression::UseUnicodePropertiesOption | QRegularExpression::DotMatchesEverythingOption | QRegularExpression::MultilineOption);
            if(!str.contains(regex))
                continue;
        }

        if(!swList[i].plusExact.isEmpty())
        {
            for(int j = 0; j < swList[i].plusExact.count(); j++)
            {
                QRegularExpression regex(QString("\\b%1\\b").arg(swList[i].plusExact[j]), QRegularExpression::UseUnicodePropertiesOption | QRegularExpression::DotMatchesEverythingOption | QRegularExpression::MultilineOption);
                if(!str.contains(regex) && !text.contains(regex))
                {
                    continueFlag = true;
                    break;
                }
            }
            if(continueFlag)
                continue;
        }

        if(!swList[i].plus.isEmpty())
        {
            for(int j = 0; j < swList[i].plus.count(); j++)
            {
                QRegularExpression regex(QString("%1").arg(swList[i].plus[j]), QRegularExpression::UseUnicodePropertiesOption | QRegularExpression::DotMatchesEverythingOption | QRegularExpression::MultilineOption);
                if(!str.contains(regex))
                {
                    continueFlag = true;
                    break;
                }
            }
            if(continueFlag)
                continue;
        }

        if(!swList[i].negExact.isEmpty())
        {
            for(int j = 0; j < swList[i].negExact.count(); j++)
            {
                QRegularExpression regex(QString("\\b%1\\b").arg(swList[i].negExact[j]), QRegularExpression::UseUnicodePropertiesOption | QRegularExpression::DotMatchesEverythingOption | QRegularExpression::MultilineOption);
                if(str.contains(regex))
                {
                    continueFlag = true;
                    break;
                }
            }
            if(continueFlag)
                continue;
        }

        if(!swList[i].neg.isEmpty())
        {
            for(int j = 0; j < swList[i].neg.count(); j++)
            {
                QRegularExpression regex(QString("%1").arg(swList[i].neg[j]), QRegularExpression::UseUnicodePropertiesOption | QRegularExpression::DotMatchesEverythingOption | QRegularExpression::MultilineOption);
                if(str.contains(regex))
                {
                    continueFlag = true;
                    break;
                }
            }
            if(continueFlag)
                continue;
        }

        if(!swList[i].startExact.isEmpty())
        {
            QRegularExpression regex(QString("^[%1\\s]*\\b%2\\b").arg(Constants::OTHER_CHARS, swList[i].startExact), QRegularExpression::UseUnicodePropertiesOption | QRegularExpression::DotMatchesEverythingOption | QRegularExpression::MultilineOption);
            if(!str.contains(regex) && !text.contains(regex))
                continue;
        }

        if(!swList[i].start.isEmpty())
        {
            QRegularExpression regex(QString("^[%1\\s]*%2").arg(Constants::OTHER_CHARS, swList[i].start), QRegularExpression::UseUnicodePropertiesOption | QRegularExpression::DotMatchesEverythingOption | QRegularExpression::MultilineOption);
            if(!str.contains(regex))
                continue;
        }

        if(!swList[i].endExact.isEmpty())
        {
            QRegularExpression regex(QString("\\b%1\\b[%2\\s]*$").arg(swList[i].endExact, Constants::OTHER_CHARS), QRegularExpression::UseUnicodePropertiesOption | QRegularExpression::DotMatchesEverythingOption | QRegularExpression::MultilineOption);
            if(!str.contains(regex) && !text.contains(regex))
                continue;
        }

        if(!swList[i].end.isEmpty())
        {
            QRegularExpression regex(QString("%1[%2\\s]*$").arg(swList[i].end, Constants::OTHER_CHARS), QRegularExpression::UseUnicodePropertiesOption | QRegularExpression::DotMatchesEverythingOption | QRegularExpression::MultilineOption);
            if(!str.contains(regex))
                continue;
        }

        return true;
    }

    return false;
}

int wordCount(const QString &word, const QString &text)
{
    QString wordPure(word);
    QString str(removeZWNJ(text));
    QString strRegex;
    bool exact = false;
    int count = 0;

    QRegularExpressionMatch match;
    QRegularExpressionMatchIterator i, j;

    if(wordPure.startsWith("\"") && wordPure.endsWith("\""))
    {
        exact = true;
        wordPure.remove(0, 1);
        wordPure.remove(wordPure.size() - 1, 1);
        if(wordPure.isEmpty())
            return 0;
    }

    if(exact)
        strRegex = QString("\\b%1\\b").arg(wordPure);
    else
        strRegex = wordPure;

    QRegularExpression regex(strRegex, QRegularExpression::UseUnicodePropertiesOption | QRegularExpression::DotMatchesEverythingOption | QRegularExpression::MultilineOption);
    i = regex.globalMatch(str);
    j = regex.globalMatch(text);
    while(i.hasNext() || j.hasNext())
    {
        count++;
        if(i.hasNext())
            i.next();
        if(j.hasNext())
            j.next();
    }

    return count;
}

SearchWordLike searchWordLike(const SearchWord &sw, const QString &fieldStr)
{
    //    QString strQuery = QString("SELECT poem_id, text FROM verse WHERE text LIKE '%%1%' ORDER BY poem_id").arg(word);
    //    QString strQuery = QString("SELECT poem_id, text FROM verse WHERE text REGEXP '.*%1.*' ORDER BY poem_id").arg(word);  // Qt v5.10+: db.setConnectOptions("QSQLITE_ENABLE_REGEXP");  // Before open db

    QString word;
    QStringList orderAll, plusAll;
    QString start, end;

    orderAll << sw.orderExact << sw.order;
    plusAll << sw.plusExact << sw.plus;
    start = sw.startExact.isEmpty() ? sw.start : sw.startExact;
    end = sw.endExact.isEmpty() ? sw.end : sw.endExact;

    QString orderAllLike;
    if(!orderAll.isEmpty())
    {
        for(int i = 0; i < orderAll.count(); i++)
        {
            if((word = wordLikeRevision(orderAll[i])).isEmpty())
                continue;
            if(orderAllLike.isEmpty())
                orderAllLike = QString("%1 LIKE '%%2%").arg(fieldStr, word);
            else
                orderAllLike += QString("%1%").arg(word);
        }
        if(!orderAllLike.isEmpty())
            orderAllLike += "'";
    }

    QString plusAllLike;
    if(!plusAll.isEmpty())
    {
        for(int i = 0; i < plusAll.count(); i++)
        {
            if((word = wordLikeRevision(plusAll[i])).isEmpty())
                continue;
            if(plusAllLike.isEmpty())
                plusAllLike = QString("%1 LIKE '%%2%'").arg(fieldStr, word);
            else
                plusAllLike += QString(" AND %1 LIKE '%%2%'").arg(fieldStr, word);
        }
    }

    QString startLike;
    if(!start.isEmpty() && !(word = wordLikeRevision(start)).isEmpty())
        startLike = QString("%1 LIKE '%%2%'").arg(fieldStr, word);

    QString endLike;
    if(!end.isEmpty() && !(word = wordLikeRevision(end)).isEmpty())
        endLike = QString("%1 LIKE '%%2%'").arg(fieldStr, word);

    SearchWordLike swl;
    swl.orderAllLike = orderAllLike;
    swl.plusAllLike = plusAllLike;
    swl.startLike = startLike;
    swl.endLike = endLike;
    return swl;
}

QString counterMarkerFinder(const QString &text)
{
    QRegularExpression regex_marker(QString("^\\s*%1\\s*([^\\s]+.*)").arg(Constants::MARKER_COUNTER));
    QRegularExpressionMatch match = regex_marker.match(text);
    if(match.hasMatch())
        return match.captured(1).trimmed();
    return QString();
}

SearchWord searchPhraseParser(const QString &orPart)
{
    SearchWord sw;

    // *******************************************************************************
    QString sPhrase(orPart);
    QRegularExpression regex;
    QRegularExpressionMatch match;
    QRegularExpressionMatchIterator i;
    int j;
    // *******************************************************************************
    QStringList orderExact;
    regex.setPattern("\\s*\\\"([^\\\"]*)\\\"\\s*\\+{2}\\s*\\\"([^\\\"]*)\\\"");  // \s*\"([^\"]*)\"\s*\+{2}\s*\"([^\"]*)\"
    match = regex.match(sPhrase);
    j = 0;
    while(match.hasMatch())
    {
        if(j++)
            orderExact << match.captured(2);
        else
            orderExact << match.captured(1) << match.captured(2);
        match = regex.match(sPhrase, match.capturedEnd(1));
    }

    if(!orderExact.isEmpty() & 1)
    {
        sPhrase.remove(regex);
        sPhrase.remove(QRegularExpression("\\+{2}\\s*\\\"([^\\\"]*)\\\""));
        sPhrase.replace("++", "");
    }
    else
    {
        sPhrase.remove(regex);
        if(!orderExact.isEmpty())
            sPhrase.replace("++", "");
    }
    sPhrase = sPhrase.trimmed();
    // *******************************************************************************
    QStringList order;
    regex.setPattern("\\s*([^\\s\\+\\-]*)\\s*\\+{2}\\s*([^\\s\\+\\-]*)");  // \s*([^\s\+\-]*)\s*\+{2}\s*([^\s\+\-]*)
    match = regex.match(sPhrase);
    j = 0;
    while(match.hasMatch())
    {
        if(j++)
            order << match.captured(2);
        else
            order << match.captured(1) << match.captured(2);
        match = regex.match(sPhrase, match.capturedEnd(2));
    }
    sPhrase.remove(regex);
    sPhrase = sPhrase.trimmed();
    // *******************************************************************************
    QString startExact;
    regex.setPattern("[\\+]?\\s*[\\^]\\s*\\\"([^\\\"]+)\\\"");  // [\+]?\s*[\^]\s*\"([^\"]+)\"
    i = regex.globalMatch(sPhrase);
    while(i.hasNext())
        startExact = i.next().captured(1);
    sPhrase.remove(regex);
    sPhrase = sPhrase.trimmed();
    // *******************************************************************************
    QString start;
    regex.setPattern("[\\+]?\\s*[\\^]\\s*([^\\s\\-\\^\\$]+)");  // [\+]?\s*[\^]\s*([^\s\-\^\$]+)
    i = regex.globalMatch(sPhrase);
    while(i.hasNext())
        start = i.next().captured(1);
    sPhrase.remove(regex);
    sPhrase = sPhrase.trimmed();
    // *******************************************************************************
    QString endExact;
    regex.setPattern("[\\+]?\\s*[\\$]\\s*\\\"([^\\\"]+)\\\"");  // [\+]?\s*[\$]\s*\"([^\"]+)\"
    i = regex.globalMatch(sPhrase);
    while(i.hasNext())
        endExact = i.next().captured(1);
    sPhrase.remove(regex);
    sPhrase = sPhrase.trimmed();
    // *******************************************************************************
    QString end;
    regex.setPattern("[\\+]?\\s*[\\$]\\s*([^\\s\\-\\^\\$]+)");  // [\+]?\s*[\$]\s*([^\s\-\^\$]+)
    i = regex.globalMatch(sPhrase);
    while(i.hasNext())
        end = i.next().captured(1);
    sPhrase.remove(regex);
    sPhrase = sPhrase.trimmed();
    // *******************************************************************************
    QStringList negExact;
    regex.setPattern("[\\+]?\\s*[\\-]\\s*\\\"([^\\\"]+)\\\"");  // [\+]?\s*[\-]\s*\"([^\"]+)\"
    i = regex.globalMatch(sPhrase);
    while(i.hasNext())
        negExact << i.next().captured(1);
    sPhrase.remove(regex);
    sPhrase = sPhrase.trimmed();
    // *******************************************************************************
    QStringList neg;
    regex.setPattern("[\\+]?\\s*[\\-]\\s*([^\\s\\-\\^\\$]+)");  // [\+]?\s*[\-]\s*([^\s\-\^\$]+)
    i = regex.globalMatch(sPhrase);
    while(i.hasNext())
        neg << i.next().captured(1);
    sPhrase.remove(regex);
    sPhrase = sPhrase.trimmed();
    // *******************************************************************************
    QStringList plusExact;
    regex.setPattern("[\\+]?\\s*\\\"([^\\\"]+)\\\"");  // [\+]?\s*\"([^\"]+)\"
    i = regex.globalMatch(sPhrase);
    while(i.hasNext())
        plusExact << i.next().captured(1);
    sPhrase.remove(regex);
    sPhrase = sPhrase.trimmed();
    // *******************************************************************************
    QStringList plus;
    regex.setPattern("[\\+]?\\s*([^\\s\\+]+)");  // [\+]?\s*([^\s\+]+)
    i = regex.globalMatch(sPhrase);
    while(i.hasNext())
        plus << i.next().captured(1);
    sPhrase.remove(regex);
    sPhrase = sPhrase.trimmed();
    // *******************************************************************************

    if(!startExact.isEmpty())
        start.clear();
    if(!endExact.isEmpty())
        end.clear();

    sw.orderExact = orderExact;
    sw.order = order;
    sw.negExact = negExact;
    sw.neg = neg;
    sw.plusExact = plusExact;
    sw.plus = plus;
    sw.startExact = startExact;
    sw.start = start;
    sw.endExact = endExact;
    sw.end = end;

    return sw;
}

QString searchStrQuery(const QSqlDatabase &database, SearchMethod searchMethod, const QString &userStr, bool allItemsSelected, const QStringList &poetIDList, SearchTable searchTable, bool sDiacritics, bool sCharTypes)
{
    if(QString(userStr).remove("\"").contains(QRegularExpression(QString("^\\s*%1").arg(Constants::MARKER_RADIF))))
        return Constants::MARKER_RADIF;
    else if(QString(userStr).remove("\"").contains(QRegularExpression(QString("^\\s*%1").arg(Constants::MARKER_GHAFIE))))
        return Constants::MARKER_GHAFIE;

    if(searchMethod == SearchMethod::Method1)
        return sSQMethod1(database, userStr, allItemsSelected, poetIDList, searchTable, sDiacritics, sCharTypes);
    if(searchMethod == SearchMethod::Method2)
        return sSQMethod2(userStr, searchTable, sDiacritics, sCharTypes);
    return QString();
}

QString sSQMethod1(const QSqlDatabase &database, const QString &userStr, bool allItemsSelected, const QStringList &poetIDList, SearchTable searchTable, bool sDiacritics, bool sCharTypes)
{
    QString result;
    QString preStrQuery;
    QString range;
    QString finalLike;
    QString orderBy;
    QString searchPhrase(userStr);
    QStringList strOr;
    QString table, fieldId, fieldName, vorder;
    QString fieldStr;
    bool activeWordExists = false;

    if(searchTable == VerseTable)
    {
        table = "verse";
        fieldId = "poem_id";
        fieldName = "text";
        vorder = "vorder";
        orderBy = " ORDER BY poem_id";
    }
    else if(searchTable == PoemTable)
    {
        table = "poem";
        fieldId = "id";
        fieldName = "title";
        orderBy = " ORDER BY cat_id";
    }
    else if(searchTable == CatTable)
    {
        table = "cat";
        fieldId = "id";
        fieldName = "text";
        orderBy = " ORDER BY poet_id";
    }
    fieldStr = fieldName;

    if(sDiacritics)
    {
        searchPhrase = removeDiacritics(searchPhrase);
        searchPhrase = removeZWNJ(searchPhrase);
        fieldStr = skipDiacritics(fieldStr);
        fieldStr = skipZWNJ(fieldStr);
    }
    if(sCharTypes)
    {
        searchPhrase = replace_AEKVH_withUnderscore(searchPhrase);
    }

    preStrQuery = QString("SELECT %1, %2%3 FROM %4 WHERE ").arg(fieldId, fieldName, (searchTable == VerseTable ? ", " + vorder : ""), table);

    if(!allItemsSelected && !poetIDList.isEmpty())
        range = searchRange(database, poetIDList, searchTable);

    QString counterWord(counterMarkerFinder(searchPhrase));
    if(!counterWord.isEmpty())
    {
        counterWord = wordLikeRevision(quotationRemover(counterWord));
        if(!counterWord.isEmpty())
            result = preStrQuery + (range.isEmpty() ? "" : range + " AND ") + QString("%1 LIKE '%%2%'").arg(fieldStr, counterWord) + orderBy;
        else
            result = QString("SELECT %1, %2%3 FROM %4").arg(fieldId, fieldName, (searchTable == VerseTable ? ", " + vorder : ""), table) + (range.isEmpty() ? "" : " WHERE " + range) + orderBy;
        return result;
    }

    QStringList listOr = searchPhrase.split("|");
    for(int i = 0; i < listOr.count(); i++)
    {
        QStringList likes;
        SearchWord sw = searchPhraseParser(listOr[i]);
        SearchWordLike swl = searchWordLike(sw, fieldStr);
        likes << swl.orderAllLike << swl.plusAllLike << swl.startLike << swl.endLike;
        likes.removeAll("");
        strOr << likes.join(" AND ");
        if(!activeWordExists)
            activeWordExists = findActiveWord(sw);
    }

    strOr.removeAll("");
    if(strOr.count() == 1)
    {
        finalLike = strOr[0];
    }
    else if(strOr.count() > 1)
    {
        for(int i = 0; i < strOr.count(); i++)
            strOr[i] = "(" + strOr[i] + ")";
        finalLike = strOr.join(" OR ");
    }

    if(!finalLike.isEmpty())
    {
        result = preStrQuery + (range.isEmpty() ? "" : range + " AND ") + finalLike + orderBy;
    }
    else if(finalLike.isEmpty() && activeWordExists)
    {
        if(range.isEmpty())
            result = QString("SELECT %1, %2%3 FROM %4").arg(fieldId, fieldName, (searchTable == VerseTable ? ", " + vorder : ""), table) + orderBy;
        else
            result = QString("SELECT %1, %2%3 FROM %4 WHERE %5").arg(fieldId, fieldName, (searchTable == VerseTable ? ", " + vorder : ""), table, range) + orderBy;
    }

    return result;
}

QString sSQMethod2(const QString &userStr, SearchTable searchTable, bool sDiacritics, bool sCharTypes)
{
    QString result;
    QString preStrQuery;
    QString finalLike;
    QString orderBy;
    QString searchPhrase(userStr);
    QStringList strOr;
    QString table, fieldId, fieldName, vorder;
    QString fieldStr;
    QString whereId;
    bool activeWordExists = false;

    if(searchTable == CatTable)
    {
        table = "cat";
        fieldId = "id";
        fieldName = "text";
        whereId = "poet_id = ";
        orderBy = " ORDER BY id";
    }
    else if(searchTable == PoemTable)
    {
        table = "poem";
        fieldId = "id";
        fieldName = "title";
        whereId = "cat_id = ";
        orderBy = " ORDER BY id";
    }
    else if(searchTable == VerseTable)
    {
        table = "verse";
        fieldId = "poem_id";
        fieldName = "text";
        vorder = "vorder";
        whereId = "poem_id = ";
        orderBy = " ORDER BY vorder";
    }
    fieldStr = fieldName;
    whereId = whereId + Constants::SQL_ID_FILTER;

    if(sDiacritics)
    {
        searchPhrase = removeDiacritics(searchPhrase);
        searchPhrase = removeZWNJ(searchPhrase);
        fieldStr = skipDiacritics(fieldStr);
        fieldStr = skipZWNJ(fieldStr);
    }
    if(sCharTypes)
    {
        searchPhrase = replace_AEKVH_withUnderscore(searchPhrase);
    }

    preStrQuery = QString("SELECT %1, %2%3 FROM %4 WHERE %5 AND ").arg(fieldId, fieldName, (searchTable == VerseTable ? ", " + vorder : ""), table, whereId);

    QString counterWord(counterMarkerFinder(searchPhrase));
    if(!counterWord.isEmpty())
    {
        counterWord = wordLikeRevision(quotationRemover(counterWord));
        if(!counterWord.isEmpty())
            result = preStrQuery + QString("%1 LIKE '%%2%'").arg(fieldStr, counterWord) + orderBy;
        else
            result = QString("SELECT %1, %2%3 FROM %4 WHERE %5").arg(fieldId, fieldName, (searchTable == VerseTable ? ", " + vorder : ""), table, whereId) + orderBy;
        return result;
    }

    QStringList listOr = searchPhrase.split("|");
    for(int i = 0; i < listOr.count(); i++)
    {
        QStringList likes;
        SearchWord sw = searchPhraseParser(listOr[i]);
        SearchWordLike swl = searchWordLike(sw, fieldStr);
        likes << swl.orderAllLike << swl.plusAllLike << swl.startLike << swl.endLike;
        likes.removeAll("");
        strOr << likes.join(" AND ");
        if(!activeWordExists)
            activeWordExists = findActiveWord(sw);
    }

    strOr.removeAll("");
    if(strOr.count() == 1)
    {
        finalLike = strOr[0];
    }
    else if(strOr.count() > 1)
    {
        for(int i = 0; i < strOr.count(); i++)
            strOr[i] = "(" + strOr[i] + ")";
        finalLike = "(" + strOr.join(" OR ") + ")";
    }

    if(!finalLike.isEmpty())
        result = preStrQuery + finalLike + orderBy;
    else if(finalLike.isEmpty() && activeWordExists)
        result = QString("SELECT %1, %2%3 FROM %4 WHERE %5").arg(fieldId, fieldName, (searchTable == VerseTable ? ", " + vorder : ""), table, whereId) + orderBy;

    return result;
}

QString searchRange(const QSqlDatabase &database, const QStringList &poetIDList, SearchTable searchTable)
{
    QSqlQuery query(database), query2(database);
    QString cIN, pIN, vIN;

    if(poetIDList.isEmpty())
        return QString();

    if(searchTable == CatTable)
    {
        for(int i = 0; i < poetIDList.count(); i++)
            cIN += poetIDList[i] + ",";
        cIN = cIN.left(cIN.size() - 1);
        return "poet_id IN (" + cIN + ")";
    }

    for(int i = 0; i < poetIDList.count(); i++)
    {
        QString catID;
        query.exec("SELECT id FROM cat WHERE poet_id = " + poetIDList[i]);
        while(query.next())
        {
            pIN += (catID = query.value(0).toString()) + ",";
            if(searchTable == PoemTable)
                continue;

            query2.exec("SELECT id FROM poem WHERE cat_id = " + catID);
            while(query2.next())
                vIN += query2.value(0).toString() + ",";
        }
    }

    if(searchTable == PoemTable)
    {
        pIN = pIN.left(pIN.size() - 1);
        return "cat_id IN (" + pIN + ")";
    }

    if(searchTable == VerseTable)
    {
        vIN = vIN.left(vIN.size() - 1);
        return "poem_id IN (" + vIN + ")";
    }

    return QString();
}

bool findActiveWord(const SearchWord &sw)
{
    if(sw.orderExact.isEmpty() && sw.order.isEmpty() &&
       sw.plusExact.isEmpty() && sw.plus.isEmpty() &&
       sw.startExact.isEmpty() && sw.start.isEmpty() &&
       sw.endExact.isEmpty() && sw.end.isEmpty())
        return false;
    return true;
}

QString wordLikeRevision(const QString &text)
{
    QString str(text);
    if(!str.isEmpty())
    {
        QRegularExpression regex;
        QRegularExpressionMatch match;

        regex.setPattern("^([_%]+)([^_%].*)?$");
        match = regex.match(str);
        if(match.hasMatch())
            str.remove(match.capturedStart(1), match.capturedLength(1));

        regex.setPattern("^(.*[^_%])?([_%]+)$");
        match = regex.match(str);
        if(match.hasMatch())
            str.remove(match.capturedStart(2), match.capturedLength(2));
    }
    return str;
}

QString skipZWNJ(const QString &text)
{
    QString str(text);
    str = QString("REPLACE(%1, '%2', '')").arg(str).arg(Constants::ZWNJ);
    return str;
}

QString skipDiacritics(const QString &text)
{
    QString str(text);
    for(int i = 0; i < Constants::DIACRITICS.size(); i++)
        str = QString("REPLACE(%1, '%2', '')").arg(str).arg(Constants::DIACRITICS[i]);
    return str;
}

QString skipCharTypes(const QString &text)
{
    QString str(text);
    for(int i = 0; i < Constants::A_TYPES.size(); i++)
        str = QString("REPLACE(%1, '%2', '%3')").arg(str).arg(Constants::A_TYPES[i]).arg(Constants::A_PERSIAN);
    for(int i = 0; i < Constants::E_TYPES.size(); i++)
        str = QString("REPLACE(%1, '%2', '%3')").arg(str).arg(Constants::E_TYPES[i]).arg(Constants::E_PERSIAN);
    for(int i = 0; i < Constants::K_TYPES.size(); i++)
        str = QString("REPLACE(%1, '%2', '%3')").arg(str).arg(Constants::K_TYPES[i]).arg(Constants::K_PERSIAN);
    for(int i = 0; i < Constants::V_TYPES.size(); i++)
        str = QString("REPLACE(%1, '%2', '%3')").arg(str).arg(Constants::V_TYPES[i]).arg(Constants::V_PERSIAN);
    for(int i = 0; i < Constants::H_TYPES.size(); i++)
        str = QString("REPLACE(%1, '%2', '%3')").arg(str).arg(Constants::H_TYPES[i]).arg(Constants::H_PERSIAN);
    return str;
}

QString removeZWNJ(const QString &text)
{
    QString str(text);
    str.replace(Constants::ZWNJ_REGEX, "");
    return str;
}

QString removeDiacritics(const QString &text)
{
    QString str(text);
    str.replace(Constants::DIACRITICS_REGEX, "");
    return str;
}

QString removeCharTypes(const QString &text)
{
    QString str(text);
    str.replace(Constants::A_TYPES_REGEX, Constants::A_PERSIAN);
    str.replace(Constants::E_TYPES_REGEX, Constants::E_PERSIAN);
    str.replace(Constants::K_TYPES_REGEX, Constants::K_PERSIAN);
    str.replace(Constants::V_TYPES_REGEX, Constants::V_PERSIAN);
    str.replace(Constants::H_TYPES_REGEX, Constants::H_PERSIAN);
    return str;
}

QString removeOtherChars(const QString &text)
{
    QString str(text);
    str.replace(Constants::OTHER_CHARS_REGEX, "");
    return str;
}

QString replace_AEKVH_withUnderscore(const QString &text)
{
    QString str(text);
    str.replace(Constants::A_REGEX, "_");
    str.replace(Constants::E_REGEX, "_");
    str.replace(Constants::K_REGEX, "_");
    str.replace(Constants::V_REGEX, "_");
    str.replace(Constants::H_REGEX, "_");
    return str;
}

QStringList textListHighlight(const QString &searchPhrase)
{
    QStringList result;
    QString sPhrase(searchPhrase);

    // The order of the arguments in the following regex is important. If you want to change it, do it carefully.
    QRegularExpression regex(QString("(%1|%2|%3)\\s*[^\\s]+").arg(Constants::MARKER_COUNTER, Constants::MARKER_RADIF, Constants::MARKER_GHAFIE));
    QRegularExpressionMatch match = regex.match(searchPhrase);
    if(match.hasMatch())
        sPhrase.remove(match.capturedStart(1), match.capturedLength(1));

    QStringList listOr = sPhrase.split("|");
    for(int i = 0; i < listOr.count(); i++)
    {
        SearchWord sw = searchPhraseParser(listOr[i]);

        for(int j = 0; j < sw.orderExact.count(); j++)
            result << sw.orderExact[j];
        for(int j = 0; j < sw.order.count(); j++)
            result << sw.order[j];
        for(int j = 0; j < sw.plusExact.count(); j++)
            result << sw.plusExact[j];
        for(int j = 0; j < sw.plus.count(); j++)
            result << sw.plus[j];

        if(!sw.startExact.isEmpty() || !sw.start.isEmpty())
            result << (sw.startExact.isEmpty() ? sw.start : sw.startExact);
        if(!sw.endExact.isEmpty() || !sw.end.isEmpty())
            result << (sw.endExact.isEmpty() ? sw.end : sw.endExact);
    }

    return result;
}

QString quotationRemover(const QString &text)
{
    QString wordPure(text);
    if(wordPure.startsWith("\"") && wordPure.endsWith("\""))
    {
        wordPure.remove(0, 1);
        wordPure.remove(wordPure.size() - 1, 1);
    }
    return wordPure;
}

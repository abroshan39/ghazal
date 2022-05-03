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

// ATTENTION: MAX NESTED REPLACE OF SQL QUERY = 29

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

QString searchTableWidget(AppSettings *appSettings, QTableWidget *tableWidget, const QString &strQuery)
{
    appSettings->ss.searchState = true;

    const QSqlDatabase database = appSettings->mainDB;
    const QString userStr = appSettings->ss.searchPhrase;
    const SearchTable searchTable = appSettings->ss.table;
    bool sDiacritics = appSettings->ss.skipDiacritics;
    bool sCharTypes = appSettings->ss.skipCharTypes;

    tableWidget->model()->removeRows(0, tableWidget->model()->rowCount());
    tableWidget->model()->removeColumns(0, tableWidget->model()->columnCount());

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
    QString hashWord(hashSignFinder(searchPhrase));
    if(hashWord.isEmpty())
    {
        QStringList listOr = searchPhrase.split("|");
        for(int i = 0; i < listOr.count(); i++)
        {
            SearchWord sw = searchWordAnalyser(listOr[i]);
            swList.append(sw);
        }
    }

    tableWidget->setColumnCount(2);
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

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

    tableWidget->setHorizontalHeaderLabels(colList);

    int count = 0;
    int row_count = 0;
    while(appSettings->ss.searchState && query.next())
    {
        { // Filtering Block
            QString text = query.value(1).toString();
            if(sDiacritics)
                text = removeDiacritics(text);
            if(sCharTypes)
                text = removeCharTypes(text);

            if(hashWord.isEmpty())
            {
                if(!patternMatched(swList, text))
                    continue;
            }
            else
            {
                int n = wordCount(hashWord, text);
                count += n;
                if(n < 2)
                    continue;
            }
        } // Filtering Block

        tableWidget->insertRow(row_count);

        QTableWidgetItem *item1 = new QTableWidgetItem;
        QTableWidgetItem *item2 = new QTableWidgetItem;
        QString id = query.value(0).toString();
        GanjoorPath gp = recursiveIDs(database, level, id);
        int iLast = gp.text.count() - 1;
        QString str = gp.text[iLast] + ": " + gp.text[0];

        if(searchTable == VerseTable)
        {
            item1->setText(spaceReplace(str, "…", 6));
            item2->setText(query.value(1).toString());
        }
        else if(searchTable == PoemTable)
        {
            item1->setText(gp.text[iLast]);
            item2->setText(query.value(1).toString());
        }
        else if(searchTable == CatTable)
        {
            item1->setText(gp.text[iLast]);
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
                item2->setText(QString("%1 (%2)").arg(query.value(1).toString(), preCat));
            }
            else
                item2->setText(query.value(1).toString());
        }

        item1->setData(Qt::UserRole, gp.text[iLast]);
        item2->setData(Qt::UserRole, level + "-" + id);

        tableWidget->setItem(row_count, 0, item1);
        tableWidget->setItem(row_count, 1, item2);

        row_count++;
    }

    appSettings->ss.searchState = false;
    if(!hashWord.isEmpty())
        return QString("تعداد کل کلمه (عبارت): <b>%1</b><br />برابر است با: <b>%2</b><br />کلمه‌هایی (عبارت‌هایی) که بیش از یک بار (دو یا بیشتر) در یک رکورد وجود دارد، در جدول قرار گرفت.").arg(hashWord, persianNumber(count));
    return QString();
}

bool patternMatched(const QList<SearchWord> &swList, const QString &text)
{
    QString str(removeZWNJ(text));
    QString strRegex;
    bool continueFlag;

    for(int i = 0; i < swList.count(); i++)
    {
        continueFlag = false;

        if(swList[i].orderExact.count())
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

        if(swList[i].order.count())
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

        if(swList[i].plusExact.count())
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

        if(swList[i].plus.count())
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

        if(swList[i].negExact.count())
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

        if(swList[i].neg.count())
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

        return true;
    }

    return false;
}

int wordCount(const QString &word, const QString &text)
{
    QString wordPure(word);
    QString str(removeZWNJ(text));
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
    {
        QRegularExpression regex(QString("\\b%1\\b").arg(wordPure), QRegularExpression::UseUnicodePropertiesOption | QRegularExpression::DotMatchesEverythingOption | QRegularExpression::MultilineOption);
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
    }
    else
    {
        QRegularExpression regex(QString("%1").arg(wordPure), QRegularExpression::UseUnicodePropertiesOption | QRegularExpression::DotMatchesEverythingOption | QRegularExpression::MultilineOption);
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
    }

    return count;
}

SearchWordLike searchWordLike(const SearchWord &sw, const QString &fieldStr)
{
    //    QString strQuery = QString("SELECT poem_id, text FROM verse WHERE text LIKE '%%1%' ORDER BY poem_id").arg(word);
    //    QString strQuery = QString("SELECT poem_id, text FROM verse WHERE text REGEXP '.*%1.*' ORDER BY poem_id").arg(word);  // Qt v5.10+: db.setConnectOptions("QSQLITE_ENABLE_REGEXP");  // Before open db

    QString word;
    QStringList orderAll, plusAll;
    orderAll << sw.orderExact << sw.order;
    plusAll << sw.plusExact << sw.plus;

    QString orderAllLike;
    if(orderAll.count())
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
    if(plusAll.count())
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

    SearchWordLike swl;
    swl.orderAllLike = orderAllLike;
    swl.plusAllLike = plusAllLike;
    return swl;
}

QString hashSignFinder(const QString &text)
{
    QRegularExpression regex("#\\s*([^\\s]+.*)");  // #\s*([^\s]+.*)
    QRegularExpressionMatch match = regex.match(text);
    if(match.hasMatch())
        return match.captured(1).trimmed();
    return QString();
}

SearchWord searchWordAnalyser(const QString &orPart)
{
    SearchWord sw;

    // ******************************************************************************* // Srart
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

    if(orderExact.count() & 1)
    {
        sPhrase.remove(regex);
        sPhrase.remove(QRegularExpression("\\+{2}\\s*\\\"([^\\\"]*)\\\""));
        sPhrase.replace("++", "");
    }
    else
    {
        sPhrase.remove(regex);
        if(orderExact.count())
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
    QStringList negExact;
    regex.setPattern("[\\-]\\s*\\\"([^\\\"]+)\\\"");  // [\-]\s*\"([^\"]+)\"
    i = regex.globalMatch(sPhrase);
    while(i.hasNext())
        negExact << i.next().captured(1);
    sPhrase.remove(regex);
    sPhrase = sPhrase.trimmed();
    // *******************************************************************************
    QStringList neg;
    regex.setPattern("[\\-]\\s*([^\\s\\-]+)");  // [\-]\s*([^\s\-]+)
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
    // ******************************************************************************* // End

    sw.orderExact = orderExact;
    sw.order = order;
    sw.negExact = negExact;
    sw.neg = neg;
    sw.plusExact = plusExact;
    sw.plus = plus;

    return sw;
}

QString searchStrQuery(const QSqlDatabase &database, const QString &userStr, bool allItemsSelected, const QStringList &poetID, SearchTable searchTable, bool sDiacritics, bool sCharTypes)
{
    QString result;
    QString preStrQuery;
    QString range;
    QString finalLike;
    QString orderBy;
    QString searchPhrase(userStr);
    QStringList strOr;
    QString table, fieldId, fieldName;
    QString fieldStr;
    bool activeWordExist = false;

    if(searchTable == VerseTable)
    {
        table = "verse";
        fieldId = "poem_id";
        fieldName = "text";
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

    preStrQuery = QString("SELECT %1, %2 FROM %3 WHERE ").arg(fieldId, fieldName, table);

    if(!allItemsSelected && !poetID.isEmpty())
        range = searchRange(database, poetID, searchTable);

    QString hashWord(hashSignFinder(searchPhrase));
    if(!hashWord.isEmpty())
    {
        hashWord = wordLikeRevision(quotationRemover(hashWord));
        if(!hashWord.isEmpty())
            result = preStrQuery + (range.isEmpty() ? "" : range + " AND ") + QString("%1 LIKE '%%2%'").arg(fieldStr, hashWord) + orderBy;
        else
            result = QString("SELECT %1, %2 FROM %3").arg(fieldId, fieldName, table) + (range.isEmpty() ? "" : " WHERE " + range) + orderBy;
        return result;
    }

    QStringList listOr = searchPhrase.split("|");
    for(int i = 0; i < listOr.count(); i++)
    {
        QStringList likes;
        SearchWord sw = searchWordAnalyser(listOr[i]);
        SearchWordLike swl = searchWordLike(sw, fieldStr);
        likes << swl.orderAllLike << swl.plusAllLike;
        likes.removeAll("");
        strOr << likes.join(" AND ");
        if(!activeWordExist)
            activeWordExist = findActiveWord(sw);
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
        result = preStrQuery + (range.isEmpty() ? "" : range + " AND ") + finalLike + orderBy;

    if(finalLike.isEmpty() && activeWordExist)
    {
        if(range.isEmpty())
            result = QString("SELECT %1, %2 FROM %3%4").arg(fieldId, fieldName, table, orderBy);
        else
            result = QString("SELECT %1, %2 FROM %3 WHERE %4%5").arg(fieldId, fieldName, table, range, orderBy);
    }

    return result;
}

QString searchRange(const QSqlDatabase &database, const QStringList &poetID, SearchTable searchTable)
{
    QSqlQuery query(database), query2(database);
    QString cIN, pIN, vIN;

    if(poetID.isEmpty())
        return QString();

    if(searchTable == CatTable)
    {
        for(int i = 0; i < poetID.count(); i++)
            cIN += poetID[i] + ",";
        cIN = cIN.left(cIN.size() - 1);
        return "poet_id IN (" + cIN + ")";
    }

    for(int i = 0; i < poetID.count(); i++)
    {
        QString catID;
        query.exec("SELECT id FROM cat WHERE poet_id = " + poetID[i]);
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
       sw.plusExact.isEmpty() && sw.plus.isEmpty())
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

    QRegularExpression regex("(#)\\s*[^\\s]+.*");  // (#)\s*[^\s]+.*
    QRegularExpressionMatch match = regex.match(searchPhrase);
    if(match.hasMatch())
        sPhrase.remove(match.capturedStart(1), match.capturedLength(1));

    QStringList listOr = sPhrase.split("|");
    for(int i = 0; i < listOr.count(); i++)
    {
        SearchWord sw = searchWordAnalyser(listOr[i]);

        for(int j = 0; j < sw.orderExact.count(); j++)
            result << sw.orderExact[j];
        for(int j = 0; j < sw.order.count(); j++)
            result << sw.order[j];
        for(int j = 0; j < sw.plusExact.count(); j++)
            result << sw.plusExact[j];
        for(int j = 0; j < sw.plus.count(); j++)
            result << sw.plus[j];
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

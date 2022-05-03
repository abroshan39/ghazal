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
#include "appthemes.h"

#include <QFontDatabase>

void MainWindow::loadDefaultFonts()
{
    QFontDatabase::addApplicationFont(":/files/fonts/Sahel.ttf");
    QFontDatabase::addApplicationFont(":/files/fonts/Sahel-Bold.ttf");
    QFontDatabase::addApplicationFont(":/files/fonts/Sahel-FD.ttf");
    QFontDatabase::addApplicationFont(":/files/fonts/Sahel-Bold-FD.ttf");
}

void MainWindow::writeSettings()
{
    QSettings settings(QString("%1/%2/%3").arg(rosybitDir()).arg(appNameOS()).arg(Constants::SettingsFileName), QSettings::IniFormat);

    settings.setValue("General/MainDatabase", appSettings.mainDBPath);
    settings.setValue("General/PoemDisplayType", appSettings.pDisplayType);
    settings.setValue("General/ShowBookmarks", (int)appSettings.showBookmarks);
    settings.setValue("General/DarkMode", (int)appSettings.isDarkMode);
    settings.setValue("General/HemistichDistance", appSettings.hemistichDistance);
    settings.setValue("Font/AppFontName", appSettings.appFN);
    settings.setValue("Font/AppFontSize", appSettings.appFS);
    settings.setValue("Font/ListFontName", appSettings.listFN);
    settings.setValue("Font/ListFontSize", appSettings.listFS);
    settings.setValue("Font/ViewFontName", appSettings.viewFN);
    settings.setValue("Font/ViewFontSize", appSettings.viewFS);
    settings.setValue("Search/SkipDiacritics", (int)appSettings.ss.skipDiacritics);
    settings.setValue("Search/SkipCharTypes", (int)appSettings.ss.skipCharTypes);

    settings.setValue("Startup/PoetSplitterSizeI0", ui->splitter->sizes().at(0));
    settings.setValue("Startup/PoetSplitterSizeI1", ui->splitter->sizes().at(1));
    settings.setValue("Startup/Maximized", (int)isMaximized());
    if(!isMaximized())
    {
        settings.setValue("Startup/Size", size());
        settings.setValue("Startup/Pos", pos());
    }
}

void MainWindow::readSettings()
{
    QSettings settings(QString("%1/%2/%3").arg(rosybitDir()).arg(appNameOS()).arg(Constants::SettingsFileName), QSettings::IniFormat);

    appSettings.mainDBPath = settings.value("General/MainDatabase").toString();
    appSettings.pDisplayType = static_cast<PoemDisplayType>(settings.value("General/PoemDisplayType", "0").toInt());
    appSettings.showBookmarks = settings.value("General/ShowBookmarks", "1").toInt();
    appSettings.isDarkMode = settings.value("General/DarkMode", "0").toInt();
    appSettings.hemistichDistance = settings.value("General/HemistichDistance", "60").toInt();
    appSettings.appFN = settings.value("Font/AppFontName", "Sahel").toString();
    appSettings.appFS = settings.value("Font/AppFontSize", "10.5").toString();
    appSettings.listFN = settings.value("Font/ListFontName", "Sahel").toString();
    appSettings.listFS = settings.value("Font/ListFontSize", "11").toString();
    appSettings.viewFN = settings.value("Font/ViewFontName", "Sahel").toString();
    appSettings.viewFS = settings.value("Font/ViewFontSize", "11").toString();
    appSettings.ss.skipDiacritics = settings.value("Search/SkipDiacritics", "0").toInt();
    appSettings.ss.skipCharTypes = settings.value("Search/SkipCharTypes", "0").toInt();

    StartupSettings startupSettings;
    startupSettings.poetSplitterSize << settings.value("Startup/PoetSplitterSizeI0", "1000").toInt();
    startupSettings.poetSplitterSize << settings.value("Startup/PoetSplitterSizeI1", "4000").toInt();
    ui->splitter->setSizes(startupSettings.poetSplitterSize);
    startupSettings.isMaximized = settings.value("Startup/Maximized", "1").toInt();
    startupSettings.mainWindowSize = settings.value("Startup/Size", QSize(968, 520)).toSize();
    resize(startupSettings.mainWindowSize);
    startupSettings.mainWindowPos = settings.value("Startup/Pos", QStyle::alignedRect(Qt::RightToLeft, Qt::AlignCenter, size(), QGuiApplication::primaryScreen()->availableGeometry()).topLeft()).toPoint();
    move(startupSettings.mainWindowPos);
    if(startupSettings.isMaximized)
        setWindowState(Qt::WindowMaximized);

    appSettings.listWidget = ui->listWidget;
    appSettings.tabWidget = ui->tabWidget;
    appSettings.viewFSCurrent = appSettings.viewFS;
    appSettings.ss.allItemsSelected = true;
    appSettings.ss.searchState = false;
    appSettings.ss.isSearching = false;
    appSettings.isOpenWordSearchForm = false;
    appSettings.isOpenAbjadForm = false;
    appSettings.isOpenAbjadFormMini = false;

    if(!QFile::exists(appSettings.mainDBPath) || !isStdGanjoorDB(appSettings.mainDBPath))
        appSettings.mainDBPath = defaultDBPath();

    if(!(appSettings.pDisplayType == Joft || appSettings.pDisplayType == Tak))
        appSettings.pDisplayType = Joft;

    appSettings.hemistichDistanceMin = 0;
    appSettings.hemistichDistanceMax = 200;

    if(appSettings.hemistichDistance < appSettings.hemistichDistanceMin)
        appSettings.hemistichDistance = appSettings.hemistichDistanceMin;
    else if(appSettings.hemistichDistance > appSettings.hemistichDistanceMax)
        appSettings.hemistichDistance = appSettings.hemistichDistanceMax;

    appSettings.ss.table = VerseTable;
}

void MainWindow::writeHistory()
{
    QString historyFilePath = QString("%1/%2/%3").arg(rosybitDir()).arg(appNameOS()).arg(Constants::HistoryFileName);
    QFile::remove(historyFilePath);
    QSettings settings(historyFilePath, QSettings::IniFormat);

    settings.setValue("History/ActiveTab", ui->tabWidget->currentIndex() + 1);

    QString value;
    for(int i = 0; i < ui->tabWidget->count(); i++)
    {
        QWidget *tab = ui->tabWidget->widget(i);
        if(appSettings.tabLastLevelID.contains(tab))
            value = appSettings.tabLastLevelID.value(tab);
        else
            value = "NULL";

        settings.setValue(QString("History/Tab%1").arg(i + 1), value);
    }
}

void MainWindow::readHistory()
{
    QString historyFilePath = QString("%1/%2/%3").arg(rosybitDir()).arg(appNameOS()).arg(Constants::HistoryFileName);
    QSettings settings(historyFilePath, QSettings::IniFormat);
    QWidget *pActiveTab = nullptr;
    QString valueActiveTab;

    int activeTab = settings.value("History/ActiveTab", "1").toInt();

    int i = 1;
    QString value = settings.value(QString("History/Tab%1").arg(i)).toString();
    while(!value.isEmpty())
    {
        TabForm *newTab = new TabForm(&appSettings, this);
        ui->tabWidget->addTab(newTab, "برگ " + QString::number(ui->tabWidget->count() + 1));

        fontSizeChanged(newTab);
        tabFontSize.insert(newTab, appSettings.viewFSCurrent);
        tabIsDarkMode.insert(newTab, appSettings.isDarkMode);

        if(value != "NULL")
        {
            appSettings.tabLastLevelID.insert(newTab, value);
            historyOnLoad.insert(newTab, value);
            ui->tabWidget->setTabText(i - 1, "  " + idTitle(appSettings.mainDB, value) + "  ");

            if(i == activeTab)
            {
                pActiveTab = newTab;
                valueActiveTab = value;
            }
        }

        value = settings.value(QString("History/Tab%1").arg(++i)).toString();
    }

    ui->tabWidget->setCurrentIndex(activeTab ? --activeTab : activeTab);

     // محتواي تب از طريق تابع تغيير تب کنونی قرار داده مي‌شود. مگر اينکه تغيير تب وجود نداشته باشد. يعني:
     // activeTab == 0
     // که در اين صورت با بلاك زير قرار داده مي‌شود.
    if(pActiveTab != nullptr && !activeTab)
    {
        setContents(pActiveTab, valueActiveTab);
        historyOnLoad.remove(pActiveTab);
    }
}

void MainWindow::widgetsStartup()
{
    ui->splitter_2->setSizes({4000, 0});
    ui->progressBarSearch->hide();
    ui->progressBarSearch->setMaximum(0);
    ui->btnBookmarkForm->hide();
    ui->checkBoxDarkMode->setChecked(appSettings.isDarkMode);

    if(!ui->tabWidget->count())
        actionNewTab();
}

void MainWindow::applyStyleSheet()
{
    QString addStyle = appStyleSheet(appSettings.appFN, appSettings.appFS, appSettings.listFN, appSettings.listFS, appSettings.viewFN, appSettings.viewFSCurrent);
    if(appSettings.isDarkMode)
        QApplication::setStyle(new DarkStyle(addStyle));
    else
        QApplication::setStyle(new LightStyle(addStyle));
}

void MainWindow::applyStyleSheetListHeader()
{
    setStyleSheet("*[accessibleName=classViewListHeader]{font-family:" + appSettings.viewFN + ";font-size:" + appSettings.viewFSCurrent + "pt;}");
}

void MainWindow::checkDBExist()
{
    if(appSettings.mainDBPath.isEmpty())
        messageBox("توجه", "فایل پایگاه داده یافت نشد!<br />لطفا از طریق گزینهٔ <b>باز کردن</b> در منوی <b>پرونده</b>، فایل پایگاه داده را انتخاب کنید؛ یا از طریق گزینهٔ <b>دانلود از مخزن</b> در منوی <b>ابزارها</b>، پایگاه داده خود را بسازید.", Warning, this);
}

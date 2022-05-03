/*
    [Ghazal: The library of persian poetry]
    Publisher: Rosybit
    Url: http://www.rosybit.com
    GitHub: https://github.com/abroshan39/ghazal
    Version: 1.4
    Author: Aboutaleb Roshan [ab.roshan39@gmail.com]
    License: MIT License
*/

#include "tabform.h"
#include "ui_tabform.h"
#include "event_functions.h"

TabForm::TabForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TabForm)
{
    ui->setupUi(this);
}

TabForm::TabForm(AppSettings *appSettings, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TabForm)
{
    ui->setupUi(this);

    this->appSettings = appSettings;

    ui->textBrowser->hide();
    ui->labelhr->hide();

    ui->textBrowser->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->textBrowser, &QTextBrowser::customContextMenuRequested, this, &TabForm::contextMenuTextBrowser);

    ui->label->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->label, &QLabel::customContextMenuRequested, this, &TabForm::contextMenuLabel);

    connect(this, SIGNAL(sigTabLastLevelIDChanged()), parent, SLOT(slotTabLastLevelIDChanged()));
    connect(this, SIGNAL(sigBookmarkChanged()), parent, SLOT(slotBookmarkChanged()));
    connect(this, SIGNAL(sigSelectedText(const QString &)), parent, SLOT(slotSelectedText(const QString &)));
    connect(new KeyPress(ui->listWidget), &KeyPress::keyPressed, this, &TabForm::listWidgetKeyPressed);
    connect(new KeyPress(ui->textBrowser), &KeyPress::keyPressed, this, &TabForm::textBrowserKeyPressed);
}

TabForm::~TabForm()
{
    delete ui;
}

void TabForm::slotSetTabContent(const QString &levelID, bool setFocusListWidget, bool rememberScrollBarValue, const QStringList &highlightText, const QString &bookmarkVerseID)
{
    setContents(levelID, true, rememberScrollBarValue, highlightText, bookmarkVerseID);
    if(setFocusListWidget)
    {
        ui->listWidget->setFocus();
        ui->listWidget->setCurrentRow(0);
    }
}

void TabForm::on_listWidget_doubleClicked(const QModelIndex &index)
{
    QString levelID(index.data(Qt::UserRole).toString());
    previousItemRow << index.row();
    setContents(levelID, false);
}

void TabForm::on_label_linkActivated(const QString &link)
{
    setContents(link);
    ui->listWidget->setFocus();
}

void TabForm::setContents(const QString &levelID, bool clearPreItemRow, bool rememberScrollBarValue, const QStringList &highlightText, const QString &bookmarkVerseID)
{
    const QString level(levelID.left(1));
    const QString id(levelID.right(levelID.size() - 2));

    GanjoorPath gp = recursiveIDs(appSettings->mainDB, level, id);
    int iLast = gp.text.count() - 1;

    appSettings->tabCurrentPoem.remove(this);

    if(iLast < 0)
        return;

    ui->label->setText(getHyperLink(gp));

    if(!appSettings->isDarkMode)
        ui->labelhr->show();

    if(clearPreItemRow)
        previousItemRow.clear();

    if(level == "1" || level == "2")
    {
        listWidgetItemList(ui->listWidget, appSettings->mainDB, id);

        ui->listWidget->show();
        ui->textBrowser->hide();
        ui->textBrowser->clear();  // Increase the speed of changing theme
    }
    else if(level == "3")
    {
        int pos = ui->textBrowser->verticalScrollBar()->value();
        if(appSettings->pDisplayType == Tak)
            ui->textBrowser->setHtml(oldStyleHtml(appSettings->mainDB, id, appSettings->viewFSCurrent, appSettings->isDarkMode, highlightText, appSettings->showBookmarks, bookmarkVerseID));
        else if(appSettings->pDisplayType == Joft)
            ui->textBrowser->setHtml(newStyleHtml(appSettings->mainDB, id, appSettings->viewFSCurrent, appSettings->isDarkMode, highlightText, appSettings->showBookmarks, bookmarkVerseID, appSettings->hemistichDistance));
        ui->listWidget->hide();
        ui->listWidget->clear();  // Increase the speed of changing theme
        ui->textBrowser->show();
        ui->textBrowser->setFocus();
        if(rememberScrollBarValue)
            ui->textBrowser->verticalScrollBar()->setValue(pos);

        appSettings->tabCurrentPoem.insert(this, tabPath.last());
    }

    appSettings->tabLastLevelID.insert(this, tabPath.last());
    emit sigTabLastLevelIDChanged();
}

void TabForm::slotTabFormSize()
{
    if(appSettings->tabCurrentPoem.contains(this))
        setContents(appSettings->tabCurrentPoem.value(this), false);
}

void TabForm::slotTabTheme()
{
    slotTabHeaderLabel();
    if(appSettings->tabCurrentPoem.contains(this))
        setContents(appSettings->tabCurrentPoem.value(this), false);
}

void TabForm::slotTabHeaderLabel()
{
    QString hyperLinkText(ui->label->text());
    ui->label->clear();
    ui->label->setText(hyperLinkText);

    if(!appSettings->isDarkMode && appSettings->tabLastLevelID.contains(this))
        ui->labelhr->show();
    else
        ui->labelhr->hide();
}

QString TabForm::getHyperLink(const GanjoorPath &gp)
{
    QString hyperLinkText;

    for(int i = gp.text.count() - 1; i >= 0; i--)
    {
        QString level = gp.level[i];
        QString title = gp.text[i];
        QString id = gp.id[i];
        QString new_title(title);

        if(level == "1")
        {
            tabPath.clear();
            tabPath << level + "-" + id;
            hyperLinkText = "<!DOCTYPE html><html><head><style type=\"text/css\">\n";
            hyperLinkText += "a {\n";
            hyperLinkText += "text-decoration: none;\n";
            hyperLinkText += "}\n";
            hyperLinkText += "</style></head><body>\n";
            hyperLinkText += "<p align=\"right\" dir=\"rtl\"><a href=\"" + level + "-" + id + "\">" + title + "</a></p>\n";
            hyperLinkText += "</body></html>";
        }
        else
        {
            if(level == "3")
                new_title = spaceReplace(title, "…", 12);
            tabPath << level + "-" + id;
            hyperLinkText.replace("</p>", " | <a href=\"" + level + "-" + id + "\">" + new_title + "</a></p>");
        }
    }

    return hyperLinkText;
}

void TabForm::listWidgetKeyPressed(QObject *object, QKeyEvent *event)
{
    Q_UNUSED(object);  // (void)object;

    int key = event->key();

    if(key == Qt::Key_Return || key == Qt::Key_Enter || key == Qt::Key_Left)
    {
        if(ui->listWidget->currentRow() >= 0)
            on_listWidget_doubleClicked(ui->listWidget->currentIndex());
    }
    else if(key == Qt::Key_Space && ui->listWidget->count() && ui->listWidget->currentRow() < 0)
    {
        ui->listWidget->setCurrentRow(0);
    }
    else if(key == Qt::Key_Right)
    {
        if(tabPath.count() > 1)
        {
            tabPath.removeLast();
            setContents(tabPath.last(), false);

            if(previousItemRow.isEmpty())
            {
                ui->listWidget->setCurrentRow(0);
            }
            else
            {
                ui->listWidget->setCurrentRow(previousItemRow.last());
                previousItemRow.removeLast();
            }
        }
        else
        {
            appSettings->listWidget->setFocus();
        }
    }
}

void TabForm::textBrowserKeyPressed(QObject *object, QKeyEvent *event)
{
    Q_UNUSED(object);  // (void)object;

    int key = event->key();

    if(key == Qt::Key_Right)
    {
        listWidgetKeyPressed(nullptr, event);
        ui->listWidget->setFocus();
    }
}

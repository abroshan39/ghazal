/*
    [Ghazal: The library of persian poetry]
    Publisher: Rosybit
    Url: http://www.rosybit.com
    GitHub: https://github.com/abroshan39/ghazal
    Version: 1.4
    Author: Aboutaleb Roshan [ab.roshan39@gmail.com]
    License: MIT License
*/

#include "searchform.h"
#include "ui_searchform.h"
#include "event_functions.h"
#include "searchexamplesform.h"

#include <QMenu>

SearchForm::SearchForm(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SearchForm)
{
    ui->setupUi(this);
}

SearchForm::SearchForm(AppSettings *appSettings, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SearchForm)
{
    ui->setupUi(this);

    this->appSettings = appSettings;

    setGeometry(QStyle::alignedRect(Qt::RightToLeft, Qt::AlignCenter, size(), QGuiApplication::primaryScreen()->availableGeometry()));
    setWindowTitle("جست‌وجوی پیشرفته");
    setWindowIcon(QIcon(":/files/images/ghazal-256x256.png"));
    setWindowModality(Qt::WindowModal);

    searchRangeMenuCreator();
    listWidgetPoetList(ui->listWidget, appSettings->mainDB, true);
    ui->labelTotal->setText(QString("تعداد کل: %1 مورد   ").arg(ui->listWidget->count()));

    for(int i = 0; i < ui->listWidget->count(); i++)
        if(appSettings->ss.poetID.contains(ui->listWidget->item(i)->data(Qt::UserRole).toString()))
            ui->listWidget->item(i)->setCheckState(Qt::Checked);
    fromFormLoad = false;
    labelUpdate();

    skipDiacritics = appSettings->ss.skipDiacritics;
    skipCharTypes = appSettings->ss.skipCharTypes;
    ui->skipDiacriticsCheckBox->setChecked(skipDiacritics);
    ui->skipCharTypesCheckBox->setChecked(skipCharTypes);

    lineEditDirectionCorrector(ui->lineEditOr);
    lineEditDirectionCorrector(ui->lineEditNeg);
    lineEditDirectionCorrector(ui->lineEditHash);
    lineEditDirectionCorrector(ui->lineEditPlus);
    lineEditDirectionCorrector(ui->lineEditExact);
    lineEditDirectionCorrector(ui->lineEditOrder);
    ui->lineEditHash->setEnabled(false);

    connect(new ZWNJPress(ui->lineEditOr), &ZWNJPress::zwnjPressed, this, &SearchForm::lineEditsZWNJPressed);
    connect(new ZWNJPress(ui->lineEditNeg), &ZWNJPress::zwnjPressed, this, &SearchForm::lineEditsZWNJPressed);
    connect(new ZWNJPress(ui->lineEditHash), &ZWNJPress::zwnjPressed, this, &SearchForm::lineEditsZWNJPressed);
    connect(new ZWNJPress(ui->lineEditPlus), &ZWNJPress::zwnjPressed, this, &SearchForm::lineEditsZWNJPressed);
    connect(new ZWNJPress(ui->lineEditExact), &ZWNJPress::zwnjPressed, this, &SearchForm::lineEditsZWNJPressed);
    connect(new ZWNJPress(ui->lineEditOrder), &ZWNJPress::zwnjPressed, this, &SearchForm::lineEditsZWNJPressed);
}

SearchForm::~SearchForm()
{
    delete ui;
}

void SearchForm::keyPressEvent(QKeyEvent *e)
{
    if(e->key() == Qt::Key_Escape)
        on_btnClose_clicked();
    else if(e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter)
        on_btnSearch_clicked();
}

void SearchForm::on_btnClose_clicked()
{
    close();
    if(!strSearch.isEmpty())
    {
        appSettings->ss.searchPhrase = strSearch;
        emit sigSearch();
    }
}

void SearchForm::on_listWidget_doubleClicked(const QModelIndex &index)
{
    int row = index.row();

    if(ui->listWidget->item(row)->checkState() == Qt::Unchecked)
        ui->listWidget->item(row)->setCheckState(Qt::Checked);
    else
        ui->listWidget->item(row)->setCheckState(Qt::Unchecked);
}

void SearchForm::on_listWidget_itemChanged(QListWidgetItem *item)
{
    if(!fromFormLoad)
    {
        ui->selectAllCheckBox->setChecked(false);
        ui->selectNoneCheckBox->setChecked(false);

        if(item->checkState() == Qt::Checked)
            appSettings->ss.poetID << item->data(Qt::UserRole).toString();
        else
            appSettings->ss.poetID.removeOne(item->data(Qt::UserRole).toString());

        labelUpdate();
    }
}

void SearchForm::on_selectAllCheckBox_clicked(bool checked)
{
    ui->selectNoneCheckBox->setChecked(false);

    if(checked)
    {
        for(int i = 0; i < ui->listWidget->count(); i++)
            ui->listWidget->item(i)->setCheckState(Qt::Checked);
        ui->selectAllCheckBox->setChecked(true);
    }
    else
        for(int i = 0; i < ui->listWidget->count(); i++)
            ui->listWidget->item(i)->setCheckState(Qt::Unchecked);
}

void SearchForm::on_selectNoneCheckBox_clicked(bool checked)
{
    Q_UNUSED(checked);  // (void)checked;

    ui->selectAllCheckBox->setChecked(false);

    for(int i = 0; i < ui->listWidget->count(); i++)
        ui->listWidget->item(i)->setCheckState(Qt::Unchecked);
    ui->selectNoneCheckBox->setChecked(true);
}

void SearchForm::labelUpdate()
{
    ui->labelSelectedCount->setText(QString("انتخاب‌شده: %1 مورد   ").arg(appSettings->ss.poetID.count()));
    if(!appSettings->ss.poetID.count())
        ui->labelSelectedCount->setText("انتخاب‌شده:   ");
}

void SearchForm::lineEditsZWNJPressed(QObject *object, Qt::KeyboardModifier key)
{
    Q_UNUSED(key);  // (void)key;
    static_cast<QLineEdit *>(object)->insert(Constants::ZWNJ);
}

void SearchForm::on_skipDiacriticsCheckBox_clicked(bool checked)
{
    skipDiacritics = checked;
}

void SearchForm::on_skipCharTypesCheckBox_clicked(bool checked)
{
    skipCharTypes = checked;
}

void SearchForm::on_btnSearch_clicked()
{
    QString str;

    if(ui->checkBoxHash->isChecked())
    {
        str = ui->lineEditHash->text().trimmed().isEmpty() ? "" : "#" + ui->lineEditHash->text().trimmed();
    }
    else
    {
        QString strOr;
        strOr = ui->lineEditOr->text().split(QRegularExpression("[\\s\\|]"), SKIP_EMPTY_PARTS).join(" | ");
        str = ui->lineEditPlus->text().split(QRegularExpression("[\\s\\+]"), SKIP_EMPTY_PARTS).join(" + ");
        str += (str.trimmed().isEmpty() || ui->lineEditExact->text().trimmed().isEmpty() ? "" : " + ") + (ui->lineEditExact->text().trimmed().isEmpty() ? "" : QString("\"%1\"").arg(ui->lineEditExact->text()));
        str += (str.trimmed().isEmpty() || ui->lineEditOrder->text().trimmed().isEmpty() ? "" : " + ") + ui->lineEditOrder->text().split(QRegularExpression("[\\s]|\\+\\+"), SKIP_EMPTY_PARTS).join(" ++ ");
        str += str.trimmed().isEmpty() || ui->lineEditNeg->text().isEmpty() ? "" : " - " + ui->lineEditNeg->text().split(QRegularExpression("[\\s\\-]"), SKIP_EMPTY_PARTS).join(" - ");
        str = (strOr.trimmed().isEmpty() ? "" : strOr) + (str.trimmed().isEmpty() || strOr.trimmed().isEmpty() ? "" : " | ") + str;
    }

    strSearch = str.trimmed();
    on_btnOK_clicked();
}

void SearchForm::on_btnOK_clicked()
{
    int poetCount = appSettings->ss.poetID.count();
    if(poetCount == 0 || poetCount == ui->listWidget->count())
        appSettings->ss.allItemsSelected = true;
    else
        appSettings->ss.allItemsSelected = false;

    if(appSettings->ss.allItemsSelected)
        appSettings->ss.poetID.clear();

    appSettings->ss.skipDiacritics = skipDiacritics;
    appSettings->ss.skipCharTypes = skipCharTypes;
    on_btnClose_clicked();
}

void SearchForm::on_checkBoxHash_toggled(bool checked)
{
    if(checked)
    {
        ui->lineEditHash->setEnabled(true);
        ui->lineEditOr->setEnabled(false);
        ui->lineEditNeg->setEnabled(false);
        ui->lineEditPlus->setEnabled(false);
        ui->lineEditExact->setEnabled(false);
        ui->lineEditOrder->setEnabled(false);
    }
    else
    {
        ui->lineEditHash->setEnabled(false);
        ui->lineEditOr->setEnabled(true);
        ui->lineEditNeg->setEnabled(true);
        ui->lineEditPlus->setEnabled(true);
        ui->lineEditExact->setEnabled(true);
        ui->lineEditOrder->setEnabled(true);
    }
}

void SearchForm::on_btnExamples_clicked()
{
    SearchExamplesForm *searchExamplesForm = new SearchExamplesForm(appSettings, this);
    searchExamplesForm->show();
}

void SearchForm::searchRangeMenuCreator()
{
    QMenu *toolButtonMenu = new QMenu("Menu");
    catAction = new QAction("جستجو در فهرست‌ها");
    poemAction = new QAction("جستجو در عنوان‌ها");
    verseAction = new QAction("جستجو در متن‌ها");

    catAction->setCheckable(true);
    poemAction->setCheckable(true);
    verseAction->setCheckable(true);

    toolButtonMenu->addAction(catAction);
    toolButtonMenu->addAction(poemAction);
    toolButtonMenu->addAction(verseAction);

    ui->toolButton->setMenu(toolButtonMenu);

    connect(catAction, &QAction::triggered, this, &SearchForm::actionCat);
    connect(poemAction, &QAction::triggered, this, &SearchForm::actionPoem);
    connect(verseAction, &QAction::triggered, this, &SearchForm::actionVerse);

    if(appSettings->ss.table == CatTable)
        actionCat();
    else if(appSettings->ss.table == PoemTable)
        actionPoem();
    else if(appSettings->ss.table == VerseTable)
        actionVerse();
}

void SearchForm::actionCat()
{
    appSettings->ss.table = CatTable;
    catAction->setChecked(true);
    poemAction->setChecked(false);
    verseAction->setChecked(false);
    emit sigSearchTableChanged();
}

void SearchForm::actionPoem()
{
    appSettings->ss.table = PoemTable;
    catAction->setChecked(false);
    poemAction->setChecked(true);
    verseAction->setChecked(false);
    emit sigSearchTableChanged();
}

void SearchForm::actionVerse()
{
    appSettings->ss.table = VerseTable;
    catAction->setChecked(false);
    poemAction->setChecked(false);
    verseAction->setChecked(true);
    emit sigSearchTableChanged();
}

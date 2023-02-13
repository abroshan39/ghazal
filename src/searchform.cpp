/*
    [Ghazal: The library of persian poetry]
    Publisher: Rosybit
    Url: http://www.rosybit.com
    GitHub: https://github.com/abroshan39/ghazal
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

    resize((int)(596 * appSettings->screenRatio), (int)(596 * appSettings->screenRatio));
    setGeometry(QStyle::alignedRect(Qt::RightToLeft, Qt::AlignCenter, size(), QGuiApplication::primaryScreen()->availableGeometry()));
    setWindowTitle("جست‌وجوی پیشرفته");
    setWindowIcon(QIcon(":/files/images/ghazal-256x256.png"));
    setWindowModality(Qt::WindowModal);

    allItemsSelected = appSettings->searchSettings.allItemsSelected;
    poetIDList = appSettings->searchSettings.poetIDList;
    searchTable = appSettings->searchSettings.table;
    searchMethod = appSettings->searchSettings.method;
    skipDiacritics = appSettings->searchSettings.skipDiacritics;
    skipCharTypes = appSettings->searchSettings.skipCharTypes;
    showItemsDuringSearch = appSettings->searchSettings.showItemsDuringSearch;

    searchRangeMenuCreator();
    listWidgetPoetList(ui->listWidget, appSettings->mainDB, true);
    ui->labelTotal->setText(QString("تعداد کل: %1 مورد   ").arg(ui->listWidget->count()));

    if(allItemsSelected)
    {
        poetIDList.clear();
    }
    else
    {
        for(int i = 0; i < ui->listWidget->count(); i++)
            if(poetIDList.contains(ui->listWidget->item(i)->data(Qt::UserRole).toString()))
                ui->listWidget->item(i)->setCheckState(Qt::Checked);
    }

    ui->radioMethod1->setChecked(searchMethod == SearchMethod::Method1);
    ui->radioMethod2->setChecked(searchMethod == SearchMethod::Method2);
    ui->checkBoxSkipDiacritics->setChecked(skipDiacritics);
    ui->checkBoxSkipCharTypes->setChecked(skipCharTypes);
    ui->checkBoxShowItems->setChecked(showItemsDuringSearch);

    lineEditDirectionCorrector(ui->lineEditOr);
    lineEditDirectionCorrector(ui->lineEditNeg);
    lineEditDirectionCorrector(ui->lineEditSingle);
    lineEditDirectionCorrector(ui->lineEditPlus);
    lineEditDirectionCorrector(ui->lineEditExact);
    lineEditDirectionCorrector(ui->lineEditOrder);
    lineEditDirectionCorrector(ui->lineEditStart);
    lineEditDirectionCorrector(ui->lineEditEnd);
    ui->lineEditSingle->setEnabled(false);

    connect(new ZWNJPress(ui->lineEditOr), &ZWNJPress::zwnjPressed, this, &SearchForm::lineEditsZWNJPressed);
    connect(new ZWNJPress(ui->lineEditNeg), &ZWNJPress::zwnjPressed, this, &SearchForm::lineEditsZWNJPressed);
    connect(new ZWNJPress(ui->lineEditSingle), &ZWNJPress::zwnjPressed, this, &SearchForm::lineEditsZWNJPressed);
    connect(new ZWNJPress(ui->lineEditPlus), &ZWNJPress::zwnjPressed, this, &SearchForm::lineEditsZWNJPressed);
    connect(new ZWNJPress(ui->lineEditExact), &ZWNJPress::zwnjPressed, this, &SearchForm::lineEditsZWNJPressed);
    connect(new ZWNJPress(ui->lineEditOrder), &ZWNJPress::zwnjPressed, this, &SearchForm::lineEditsZWNJPressed);
    connect(new ZWNJPress(ui->lineEditStart), &ZWNJPress::zwnjPressed, this, &SearchForm::lineEditsZWNJPressed);
    connect(new ZWNJPress(ui->lineEditEnd), &ZWNJPress::zwnjPressed, this, &SearchForm::lineEditsZWNJPressed);
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
    QString poetID(item->data(Qt::UserRole).toString());

    ui->selectAllCheckBox->setChecked(false);
    ui->selectNoneCheckBox->setChecked(false);

    if(item->checkState() == Qt::Checked)
    {
        if(!poetIDList.contains(poetID))
            poetIDList << poetID;
    }
    else
    {
        if(poetIDList.contains(poetID))
            poetIDList.removeOne(poetID);
    }

    labelUpdate();
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
    ui->labelSelectedCount->setText(QString("انتخاب‌شده: %1 مورد   ").arg(poetIDList.count()));
    if(!poetIDList.count())
        ui->labelSelectedCount->setText("انتخاب‌شده:   ");
}

void SearchForm::lineEditsZWNJPressed(QObject *object, Qt::KeyboardModifier key)
{
    Q_UNUSED(key);  // (void)key;
    static_cast<QLineEdit *>(object)->insert(Constants::ZWNJ);
}

void SearchForm::on_checkBoxSkipDiacritics_clicked(bool checked)
{
    skipDiacritics = checked;
}

void SearchForm::on_checkBoxSkipCharTypes_clicked(bool checked)
{
    skipCharTypes = checked;
}

void SearchForm::on_checkBoxShowItems_clicked(bool checked)
{
    showItemsDuringSearch = checked;
}

void SearchForm::on_btnSearch_clicked()
{
    QString str;
    QString marker;

    if(ui->checkBoxCounter->isChecked())
        marker = Constants::MARKER_COUNTER;
    else if(ui->checkBoxRadif->isChecked())
        marker = Constants::MARKER_RADIF;
    else if(ui->checkBoxGhafie->isChecked())
        marker = Constants::MARKER_GHAFIE;

    if(!marker.isEmpty())
    {
        str = ui->lineEditSingle->text().trimmed().isEmpty() ? "" : marker + ui->lineEditSingle->text().trimmed();
    }
    else
    {
        QString strOr;
        strOr = ui->lineEditOr->text().split(QRegularExpression("[\\s\\|]"), SKIP_EMPTY_PARTS).join(" | ");
        str = ui->lineEditStart->text().trimmed().isEmpty() ? "" : "^" + ui->lineEditStart->text().trimmed();
        str += (str.trimmed().isEmpty() || ui->lineEditPlus->text().trimmed().isEmpty() ? "" : " + ") + ui->lineEditPlus->text().split(QRegularExpression("[\\s\\+]"), SKIP_EMPTY_PARTS).join(" + ");
        str += (str.trimmed().isEmpty() || ui->lineEditExact->text().trimmed().isEmpty() ? "" : " + ") + (ui->lineEditExact->text().trimmed().isEmpty() ? "" : QString("\"%1\"").arg(ui->lineEditExact->text()));
        str += (str.trimmed().isEmpty() || ui->lineEditOrder->text().trimmed().isEmpty() ? "" : " + ") + ui->lineEditOrder->text().split(QRegularExpression("[\\s]|\\+\\+"), SKIP_EMPTY_PARTS).join(" ++ ");
        str += (str.trimmed().isEmpty() || ui->lineEditNeg->text().isEmpty() ? "" : " - " + ui->lineEditNeg->text().split(QRegularExpression("[\\s\\-]"), SKIP_EMPTY_PARTS).join(" - "));
        str += (str.trimmed().isEmpty() || ui->lineEditEnd->text().trimmed().isEmpty() ? "" : " + ") + (ui->lineEditEnd->text().trimmed().isEmpty() ? "" : "$" + ui->lineEditEnd->text().trimmed());
        str = (strOr.trimmed().isEmpty() ? "" : strOr) + (str.trimmed().isEmpty() || strOr.trimmed().isEmpty() ? "" : " | ") + str;
    }

    strSearch = str.trimmed();
    on_btnOK_clicked();
}

void SearchForm::on_btnOK_clicked()
{
    int poetCount = poetIDList.count();
    if(poetCount == 0 || poetCount == ui->listWidget->count())
        allItemsSelected = true;
    else
        allItemsSelected = false;

    if(allItemsSelected)
        poetIDList.clear();
    else
        std::sort(poetIDList.begin(), poetIDList.end(), idComp);

    if(ui->radioMethod1->isChecked())
        searchMethod = SearchMethod::Method1;
    else if(ui->radioMethod2->isChecked())
        searchMethod = SearchMethod::Method2;

    appSettings->searchSettings.allItemsSelected = allItemsSelected;
    appSettings->searchSettings.poetIDList = poetIDList;
    appSettings->searchSettings.method = searchMethod;
    appSettings->searchSettings.skipDiacritics = skipDiacritics;
    appSettings->searchSettings.skipCharTypes = skipCharTypes;
    appSettings->searchSettings.showItemsDuringSearch = showItemsDuringSearch;

    if(searchTable != appSettings->searchSettings.table)
    {
        appSettings->searchSettings.table = searchTable;
        emit sigSearchTableChanged();
    }

    on_btnClose_clicked();
    if(!strSearch.isEmpty())
    {
        appSettings->searchSettings.searchPhrase = strSearch;
        emit sigSearch();
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

    if(searchTable == CatTable)
        actionCat();
    else if(searchTable == PoemTable)
        actionPoem();
    else if(searchTable == VerseTable)
        actionVerse();
}

void SearchForm::actionCat()
{
    searchTable = CatTable;
    catAction->setChecked(true);
    poemAction->setChecked(false);
    verseAction->setChecked(false);
}

void SearchForm::actionPoem()
{
    searchTable = PoemTable;
    catAction->setChecked(false);
    poemAction->setChecked(true);
    verseAction->setChecked(false);
}

void SearchForm::actionVerse()
{
    searchTable = VerseTable;
    catAction->setChecked(false);
    poemAction->setChecked(false);
    verseAction->setChecked(true);
}

void SearchForm::singleCheckBoxClicked(bool checked)
{
    if(checked)
    {
        ui->lineEditSingle->setEnabled(true);
        ui->lineEditOr->setEnabled(false);
        ui->lineEditNeg->setEnabled(false);
        ui->lineEditPlus->setEnabled(false);
        ui->lineEditExact->setEnabled(false);
        ui->lineEditOrder->setEnabled(false);
        ui->lineEditStart->setEnabled(false);
        ui->lineEditEnd->setEnabled(false);
    }
    else
    {
        ui->lineEditSingle->setEnabled(false);
        ui->lineEditOr->setEnabled(true);
        ui->lineEditNeg->setEnabled(true);
        ui->lineEditPlus->setEnabled(true);
        ui->lineEditExact->setEnabled(true);
        ui->lineEditOrder->setEnabled(true);
        ui->lineEditStart->setEnabled(true);
        ui->lineEditEnd->setEnabled(true);
    }
}

void SearchForm::on_checkBoxCounter_clicked(bool checked)
{
    ui->checkBoxRadif->setEnabled(!checked);
    ui->checkBoxGhafie->setEnabled(!checked);
    singleCheckBoxClicked(checked);
}

void SearchForm::on_checkBoxRadif_clicked(bool checked)
{
    ui->checkBoxCounter->setEnabled(!checked);
    ui->checkBoxGhafie->setEnabled(!checked);
    singleCheckBoxClicked(checked);
}

void SearchForm::on_checkBoxGhafie_clicked(bool checked)
{
    ui->checkBoxCounter->setEnabled(!checked);
    ui->checkBoxRadif->setEnabled(!checked);
    singleCheckBoxClicked(checked);
}

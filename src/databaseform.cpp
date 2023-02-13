/*
    [Ghazal: The library of persian poetry]
    Publisher: Rosybit
    Url: http://www.rosybit.com
    GitHub: https://github.com/abroshan39/ghazal
    Author: Aboutaleb Roshan [ab.roshan39@gmail.com]
    License: MIT License
*/

#include "databaseform.h"
#include "ui_databaseform.h"
#include "downloadform.h"

#include <QListWidgetItem>
#include <QFileDialog>

DatabaseForm::DatabaseForm(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DatabaseForm)
{
    ui->setupUi(this);
}

DatabaseForm::DatabaseForm(AppSettings *appSettings, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DatabaseForm)
{
    ui->setupUi(this);

    this->appSettings = appSettings;

    resize((int)(672 * appSettings->screenRatio), (int)(400 * appSettings->screenRatio));
    setGeometry(QStyle::alignedRect(Qt::RightToLeft, Qt::AlignCenter, size(), QGuiApplication::primaryScreen()->availableGeometry()));
    setWindowTitle("مدیریت پایگاه داده");
    setWindowIcon(QIcon(":/files/images/ghazal-256x256.png"));
    setWindowModality(Qt::WindowModal);

    ui->lineEdit->setText(QDir::toNativeSeparators(appSettings->mainDBPath));
    ui->lineEdit->setCursorPosition(0);
    ui->labelProgress->hide();
    ui->progressBar->hide();
    ui->progressBar->setMaximum(0);

    listWidgetPoetList(ui->listWidget, appSettings->mainDB, true);
    ui->labelTotal->setText(QString("تعداد کل: %1 مورد   ").arg(ui->listWidget->count()));
}

DatabaseForm::~DatabaseForm()
{
    delete ui;
}

void DatabaseForm::keyPressEvent(QKeyEvent *e)
{
    if(e->key() == Qt::Key_Escape)
        on_btnClose_clicked();
}

void DatabaseForm::closeEvent(QCloseEvent *e)
{
    if(isProcessing)
    {
        messageBox("توجه", "نرم‌افزار در حال پردازش است. لطفا شکیبا باشید!", Warning, this);
        e->ignore();
        return;
    }

    QWidget::closeEvent(e);
}

void DatabaseForm::on_btnClose_clicked()
{
    close();
}

void DatabaseForm::on_listWidget_doubleClicked(const QModelIndex &index)
{
    int row = index.row();

    if(ui->listWidget->item(row)->checkState() == Qt::Unchecked)
        ui->listWidget->item(row)->setCheckState(Qt::Checked);
    else
        ui->listWidget->item(row)->setCheckState(Qt::Unchecked);
}

void DatabaseForm::on_listWidget_itemChanged(QListWidgetItem *item)
{
    ui->selectAllCheckBox->setChecked(false);
    ui->selectNoneCheckBox->setChecked(false);

    if(item->checkState() == Qt::Checked)
        poetSelectedIDs << item->data(Qt::UserRole).toString();
    else
        poetSelectedIDs.removeOne(item->data(Qt::UserRole).toString());

    labelUpdate();
}

void DatabaseForm::on_selectAllCheckBox_clicked(bool checked)
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

void DatabaseForm::on_selectNoneCheckBox_clicked(bool checked)
{
    Q_UNUSED(checked);  // (void)checked;

    ui->selectAllCheckBox->setChecked(false);

    for(int i = 0; i < ui->listWidget->count(); i++)
        ui->listWidget->item(i)->setCheckState(Qt::Unchecked);
    ui->selectNoneCheckBox->setChecked(true);
}

void DatabaseForm::labelUpdate()
{
    ui->labelSelectedCount->setText(QString("انتخاب‌شده: %1 مورد   ").arg(poetSelectedIDs.count()));
    ui->labelTotal->setText(QString("تعداد کل: %1 مورد   ").arg(ui->listWidget->count()));
    if(!poetSelectedIDs.count())
    {
        ui->btnExport->setEnabled(false);
        ui->btnRemovePoet->setEnabled(false);
        ui->labelSelectedCount->setText("انتخاب‌شده:   ");
    }
    else
    {
        ui->btnExport->setEnabled(true);
        ui->btnRemovePoet->setEnabled(true);
    }
}

void DatabaseForm::on_btnDownloadForm_clicked()
{
    DownloadForm *downloadForm = new DownloadForm(appSettings, this);
    connect(downloadForm, &DownloadForm::sigMainDBChanged, this, &DatabaseForm::slotMainDBChanged);
    connect(downloadForm, &DownloadForm::sigUpdatePoetList, this, &DatabaseForm::slotUpdatePoetList);
    downloadForm->show();
}

void DatabaseForm::slotMainDBChanged()
{
    emit sigMainDBChanged();  // This line must be the first. Because the application database must be opened.
    listWidgetPoetList(ui->listWidget, appSettings->mainDB, true);
    labelUpdate();
    ui->lineEdit->setText(QDir::toNativeSeparators(appSettings->mainDBPath));
}

void DatabaseForm::slotUpdatePoetList()
{
    emit sigUpdatePoetList();
    listWidgetPoetList(ui->listWidget, appSettings->mainDB, true);
    labelUpdate();
}

void DatabaseForm::on_btnAddPoet_clicked()
{
    if(appSettings->mainDBPath.isEmpty())
    {
        messageBox("هشدار", "شما پایگاه دادهٔ فعالی ندارید. لطفا ابتدا یک پایگاه داده ایجاد کنید.", Warning, this);
        appSettings->mainDBPath = createDBDialog(this);
        if(!appSettings->mainDBPath.isEmpty())
            slotMainDBChanged();
        else
            return;
    }

    QString filter = "Supported files (*.gdb *.s3db *.db *.sqlite *.sqlite3 *.zip);;Database files (*.gdb *.s3db *.db *.sqlite *.sqlite3);;Compressed files (*.zip);;All files (*)";
    QStringList file_names = QFileDialog::getOpenFileNames(this, "Open", QDir::homePath(), filter);

    if(!file_names.isEmpty())
    {
        for(int i = 0; i < file_names.count(); i++)
        {
            if(!file_names[i].endsWith(".zip", Qt::CaseInsensitive) && !isStdGanjoorDB(file_names[i]))
            {
                messageBox("خطا", QString("<b>خطا</b>:<br />فایل انتخاب‌شده قالب استانداردی ندارد!<br />[%1]").arg(QDir::toNativeSeparators(file_names[i])), Critical, this);
                return;
            }
        }

        Worker *worker = new Worker(Worker::Importer, appSettings, file_names, true, 1000);
        threadStart(worker);
    }
}

void DatabaseForm::on_btnExport_clicked()
{
    if(poetSelectedIDs.count())
    {
        QString filter = "Ganjoor Database files (*.s3db);;Ganjoor Database files (*.gdb);;SQLite Database files (*.sqlite);;SQLite 3 Database files (*.sqlite3);;Database files (*.db)";
        QString db_file_name = QFileDialog::getSaveFileName(this, "Save As", QDir::homePath() + "/exportedDB_" + nowDate("-") + "_" + nowTime("") + ".s3db", filter);
        if(!db_file_name.isEmpty())
        {
            Worker *worker = new Worker(Worker::Exporter, appSettings, db_file_name, poetSelectedIDs, 1000);
            threadStart(worker);
        }
    }
}

void DatabaseForm::on_btnRemovePoet_clicked()
{
    if(poetSelectedIDs.count())
    {
        int reply = messageBox("حذف؟", "آیا از حذف موارد انتخاب‌شده مطمئن هستید؟     ", WarningQuestion, this);
        if(reply == QMessageBox::Yes)
        {
            Worker *worker = new Worker(Worker::Remover, appSettings, poetSelectedIDs);
            threadStart(worker);
        }
    }
}

void DatabaseForm::on_btnCompactDB_clicked()
{
    Worker *worker = new Worker(Worker::Vacuumer, appSettings);
    threadStart(worker);
}

void DatabaseForm::threadStart(Worker *worker)
{
    QThread *thread = new QThread;
    worker->moveToThread(thread);

    connect(thread, &QThread::started, worker, &Worker::process);
    connect(worker, &Worker::finished, this, &DatabaseForm::threadFinished);
    connect(worker, &Worker::finished, thread, &QThread::quit);
    connect(worker, &Worker::finished, worker, &Worker::deleteLater);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);

    isProcessing = true;
    setEnabled(false);
    ui->labelProgress->show();
    ui->progressBar->show();
    thread->start();
}

void DatabaseForm::threadFinished(Worker::WorkerType type, QVariant result)
{
    isProcessing = false;
    setEnabled(true);
    ui->labelProgress->hide();
    ui->progressBar->hide();

    if(type == Worker::Importer)
    {
        qDebug().noquote() << "Worker: Importer";
        slotUpdatePoetList();
    }
    else if(type == Worker::Exporter)
    {
        qDebug().noquote() << "Worker: Exporter";
    }
    else if(type == Worker::Remover)
    {
        qDebug().noquote() << "Worker: Remover";
        poetSelectedIDs.clear();
        slotUpdatePoetList();
    }
    else if(type == Worker::Vacuumer)
    {
        qDebug().noquote() << "Worker: Vacuumer";
        if(result.toBool())
            messageBox("گزارش", "پایگاه داده با موفقیت فشرده شد.", Information, this);
        else
            messageBox("خطا", "فشرده‌سازی پایگاه داده با خطا مواجه شد!", Critical, this);
    }
}

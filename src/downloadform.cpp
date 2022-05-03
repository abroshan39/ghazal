/*
    [Ghazal: The library of persian poetry]
    Publisher: Rosybit
    Url: http://www.rosybit.com
    GitHub: https://github.com/abroshan39/ghazal
    Version: 1.4
    Author: Aboutaleb Roshan [ab.roshan39@gmail.com]
    License: MIT License
*/

#include "downloadform.h"
#include "ui_downloadform.h"

#include <QFileInfo>
#include <QFileDialog>
#include <QFile>

#include <JlCompress.h>

DownloadForm::DownloadForm(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DownloadForm)
{
    ui->setupUi(this);
}

DownloadForm::DownloadForm(AppSettings *appSettings, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DownloadForm)
{
    ui->setupUi(this);

    this->appSettings = appSettings;

    downloadType = ImportToMainDB;
    ui->lineEditSaveLocation->setText(QDir::toNativeSeparators(appSettings->mainDBPath));

    setGeometry(QStyle::alignedRect(Qt::RightToLeft, Qt::AlignCenter, size(), QGuiApplication::primaryScreen()->availableGeometry()));
    setWindowTitle("دانلود از مخزن");
    setWindowIcon(QIcon(":/files/images/ghazal-256x256.png"));
    setWindowModality(Qt::WindowModal);

    startWidgets();

    ui->comboBoxSave->addItem("ذخیره در پایگاه داده اصلی", ImportToMainDB);
    ui->comboBoxSave->addItem("خروجی در یک پایگاه داده جدید", ExportToNewDB);
    ui->comboBoxSave->addItem("فقط دانلود فایل خام", OnlyDownloadFiles);

    fileDownloader = new FileDownloader();
    connect(fileDownloader, &FileDownloader::sigStartDownload, this, &DownloadForm::slotStartDownload);
    connect(fileDownloader, &FileDownloader::sigCancel, this, &DownloadForm::slotCancel);
    connect(fileDownloader, &FileDownloader::sigProgress, this, &DownloadForm::slotProgress);
    connect(fileDownloader, &FileDownloader::sigFinished, this, &DownloadForm::slotFinished);
    connect(fileDownloader, &FileDownloader::sigErorr, this, &DownloadForm::slotErorr);
}

DownloadForm::~DownloadForm()
{
    if(fileDownloader)
    {
       fileDownloader->deleteLater();
       fileDownloader = nullptr;
    }

    delete ui;
}

void DownloadForm::keyPressEvent(QKeyEvent *e)
{
    if(e->key() == Qt::Key_Escape)
        on_btnClose_clicked();
}

void DownloadForm::closeEvent(QCloseEvent *event)
{
    if(downloadFlag == IsDownloading)
    {
        int reply = messageBox("خروج؟", "نرم‌افزار در حال دانلود است. آیا می‌خواهید دانلود را متوقف کنید و از صفحهٔ دانلود خارج شوید؟", WarningQuestion, this);
        if(reply == QMessageBox::No)
        {
            event->ignore();
            return;
        }
        cancelClose = true;
        downloadFlag = None;
        fileDownloader->cancel();
    }

    QWidget::closeEvent(event);
}

void DownloadForm::on_btnClose_clicked()
{
    close();
}

void DownloadForm::startWidgets()
{
    qDir.setPath(QDir::tempPath());
    ui->lineEdit->setText("http://i.ganjoor.net/android/androidgdbs.xml");
    ui->labelSaveLocation->setText("<p align=\"left\">:محل ذخیره‌سازی</p>");
    ui->btnCancel->hide();
    ui->labelPoetName->hide();
    ui->labelFinish->hide();
    ui->progressBarOne->hide();
    ui->progressBarAll->hide();

    ui->tableWidget->setColumnCount(3);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    QStringList colList;
    colList << "نام شاعر یا نویسنده" << "حجم فایل" << "تاریخ انتشار";
    ui->tableWidget->setHorizontalHeaderLabels(colList);
    ui->tableWidget->horizontalHeader()->setDefaultSectionSize(180);
}

void DownloadForm::on_btnSearchOnGanjoor_clicked()
{
    isXml = true;
    downloadFlag = IsDownloading;

    refreshForm();
    ui->comboBox->setCurrentIndex(0);

    QString url = ui->lineEdit->text();
    QFileInfo xmlFN(url);

    xmlDirName = Constants::Rosybit.toLower() + "-xml-" + randString();
    xmlFileName = xmlFN.fileName();

    ui->tableWidget->model()->removeRows(0, ui->tableWidget->model()->rowCount());

    if(qDir.mkdir(xmlDirName))
    {
        fileDownloader->download(url, qDir.path() + "/" + xmlDirName);
        ui->btnSearchOnGanjoor->setEnabled(false);
    }
}

void DownloadForm::on_btnDownload_clicked()
{
    isXml = false;

    if(selectedPoets.count() && check_DB_DirPath())
    {
        startDownload();
        downloadLoop();
    }
}

void DownloadForm::downloadLoop()
{
    if(downloadType == ImportToMainDB || downloadType == ExportToNewDB)
    {
        if(!commonPoetsChecked)
        {
            QSqlQuery query(downloadType == ImportToMainDB ? appSettings->mainDB : exportDB.DB());
            QMapIterator<QString, QStringList> i(selectedPoets);

            while(i.hasNext())
            {
                i.next();
                query.exec("SELECT * FROM poet WHERE id = " + i.key());
                if(query.first())
                    commonPoets << i.key();
            }

            if(!commonPoets.isEmpty())
            {
                int reply = messageBox("جایگزینی؟", "در بین انتخاب‌شده‌ها، مواردی وجود دارد که از قبل در پایگاه داده موجود است. آیا می‌خواهید موارد قدیمی را <b>حذف</b> و با موارد جدید <b>جایگزین</b> کنید؟<br />در غیر این صورت فقط مواردی نصب می‌شوند که در پایگاه داده موجود نیستند.", WarningQuestionToAll, this, QMessageBox::NoToAll);
                if(reply == QMessageBox::YesToAll)
                    removePreVer = true;
                else if(reply == QMessageBox::NoToAll)
                    removePreVer = false;
                else
                {
                    startDownloadAbort();
                    downloadFlag = None;
                    return;
                }
            }

            commonPoetsChecked = true;
        }

        while(!removePreVer && commonPoets.contains(selectedPoets.firstKey()))
        {
            selectedPoets.remove(selectedPoets.firstKey());
            ui->progressBarAll->setMaximum(--allSelectedCount);
        }

        if(!downloadedDB && !allSelectedCount)
        {
            downloadFlag = None;
            downloadLoopEnd();
            messageBox("گزارش", "<b>توجه:</b><br />موردی برای دانلود یافت نشد!", Information, this);
            return;
        }
        else if(downloadedDB == allSelectedCount)
        {
            downloadFlag = Finished;
            downloadLoopEnd();
            return;
        }
    }

    QString url = selectedPoets.first()[2];
    QString poetName = selectedPoets.first()[1];
    QFileInfo dlFN(url);

    ui->labelPoetName->setText(QString("در حال دانلود دیتابیس مربوط به <b>%1</b>:").arg(poetName));

    dlDirName = Constants::Rosybit.toLower() + "-" + randString();
    dlFileName = dlFN.fileName();

    if(downloadType == ImportToMainDB || downloadType == ExportToNewDB)
    {
        if(qDir.mkdir(dlDirName))
            fileDownloader->download(url, qDir.path() + "/" + dlDirName);
    }
    else if(downloadType == OnlyDownloadFiles)
    {
        fileDownloader->download(url, onlyDownloadFilesDirPath);
    }
}

void DownloadForm::slotFinished()
{
    if(isXml)
    {
        ui->progressBarOne->hide();

        QDomDocument document;
        QFile file(qDir.path() + "/" + xmlDirName + "/" + xmlFileName);
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        document.setContent(&file);
        file.close();
        removeTempDir(xmlDirName);

        QDomElement root = document.documentElement();
        xmlPoetList.clear();
        xmlPoetList = xmlPoetListElements(root, "gdb");

        totalSize = 0;
        selectedPoets.clear();
        disconnect(ui->tableWidget, &QTableWidget::itemChanged, this, &DownloadForm::slotTableWidgetItemChanged);
        tableWidgetDownloadList(ui->tableWidget, appSettings->mainDB, xmlPoetList, preInstalled, notInstalled, appSettings->isDarkMode);
        connect(ui->tableWidget, &QTableWidget::itemChanged, this, &DownloadForm::slotTableWidgetItemChanged);
        ui->btnSearchOnGanjoor->setEnabled(true);
        ui->btnCancel->hide();
        downloadFlag = Finished;
    }
    else
    {
        if(downloadType == ImportToMainDB || downloadType == ExportToNewDB)
        {
            QString dlDirPath = qDir.path() + "/" + dlDirName;
            QString dlFilePath = dlDirPath + "/" + dlFileName;

            if(dlFilePath.endsWith(".zip", Qt::CaseInsensitive))
            {
                QStringList list = JlCompress::getFileList(dlFilePath);
                QStringList dbList;

                for(int i = 0; i < list.count(); i++)
                    if(dbExtCheck(list[i]))
                        dbList << list[i];

                dbList = JlCompress::extractFiles(dlFilePath, dbList, dlDirPath);

                for(int i = 0; i < dbList.count(); i++)
                    if(isStdGanjoorDB(dbList[i]))
                        writeToDB(dbList[i]);
            }
            else
            {
                if(isStdGanjoorDB(dlFilePath))
                    writeToDB(dlFilePath);
                else
                    qDebug().noquote() << "Cannot open downloaded file as a database file!";
            }
            removeTempDir(dlDirName);
        }

        ui->progressBarAll->setValue(++downloadedDB);
        downloadedSize += selectedPoets.first()[3].toInt();
        ui->labelDlInstalledCount->setText(QString("دانلود و نصب‌شده: %1 مورد (%2)   ").arg(downloadedDB).arg(byteToHuman(downloadedSize)));

        if(downloadedDB == allSelectedCount)
            downloadFlag = Finished;

        selectedPoets.remove(selectedPoets.firstKey());

        if(downloadFlag == IsDownloading || downloadFlag == Finished)
        {
            if(selectedPoets.count())
                downloadLoop();
            else
                downloadLoopEnd();
        }
    }
}

void DownloadForm::slotStartDownload()
{
    ui->progressBarOne->setValue(0);
    ui->progressBarOne->show();
    ui->btnCancel->show();
}

void DownloadForm::slotCancel()
{
    downloadFlag = Canceled;
    downloadLoopEnd();
}

void DownloadForm::slotProgress(const QString &fileName, qint64 total, qint64 received, const QString &sSpeed, int leftHour, int leftMin, int leftSec)
{
    Q_UNUSED(fileName);  // (void)fileName;
    Q_UNUSED(sSpeed);    // (void)sSpeed;
    Q_UNUSED(leftHour);  // (void)leftHour;
    Q_UNUSED(leftMin);   // (void)leftMin;
    Q_UNUSED(leftSec);   // (void)leftSec;

    ui->progressBarOne->setValue(received);
    ui->progressBarOne->setMaximum(total);
}

void DownloadForm::slotErorr(const QString &error)
{
    Q_UNUSED(error);  // (void)error;
    downloadFlag = Error;
    downloadLoopEnd();
}

void DownloadForm::on_btnCancel_clicked()
{
    int reply = messageBox("توقف دانلود؟", "آیا از توقف دانلود مطمئن هستید؟   ", WarningQuestion, this);
    if(reply == QMessageBox::Yes)
        fileDownloader->cancel();
}

void DownloadForm::on_tableWidget_doubleClicked(const QModelIndex &index)
{
    int row = index.row();

    if(ui->tableWidget->item(row, 0)->checkState() == Qt::Unchecked)
        ui->tableWidget->item(row, 0)->setCheckState(Qt::Checked);
    else
        ui->tableWidget->item(row, 0)->setCheckState(Qt::Unchecked);
}

void DownloadForm::slotTableWidgetItemChanged(QTableWidgetItem *item)
{
    QString itemPoetID(item->data(Qt::UserRole).toString());
    QStringList list;

    if(item->checkState() == Qt::Checked)
    {
        QString poetName = ui->tableWidget->item(item->row(), 0)->data(Qt::DisplayRole).toString();
        QString poetURL = ui->tableWidget->item(item->row(), 3)->data(Qt::DisplayRole).toString();
        QString poetSize = ui->tableWidget->item(item->row(), 4)->data(Qt::DisplayRole).toString();
        totalSize += poetSize.toInt();
        list << QString::number(item->row()) << poetName << poetURL << poetSize;
        selectedPoets.insert(itemPoetID, list);
    }
    else
    {
        QString poetSize = ui->tableWidget->item(item->row(), 4)->data(Qt::DisplayRole).toString();
        totalSize -= poetSize.toInt();
        selectedPoets.remove(itemPoetID);
    }

    ui->labelSelectedCount->setText(QString("انتخاب‌شده: %1 مورد   ").arg(selectedPoets.count()));
    ui->labelSelectedSize->setText(QString("حجم کل: %1   ").arg(byteToHuman(totalSize)));
    if(!selectedPoets.count())
        clearLabels();

    ui->selectAllCheckBox->setChecked(false);
    ui->selectNoneCheckBox->setChecked(false);
    ui->preInstalledCheckBox->setChecked(false);
    ui->notInstalledCheckBox->setChecked(false);
}

void DownloadForm::on_selectAllCheckBox_clicked(bool checked)
{
    ui->selectNoneCheckBox->setChecked(false);
    ui->preInstalledCheckBox->setChecked(false);
    ui->notInstalledCheckBox->setChecked(false);

    if(checked)
    {
        for(int i = 0; i < ui->tableWidget->rowCount(); i++)
            ui->tableWidget->item(i, 0)->setCheckState(Qt::Checked);
        ui->selectAllCheckBox->setChecked(true);
    }
    else
        for(int i = 0; i < ui->tableWidget->rowCount(); i++)
            ui->tableWidget->item(i, 0)->setCheckState(Qt::Unchecked);
}

void DownloadForm::on_selectNoneCheckBox_clicked(bool checked)
{
    Q_UNUSED(checked);  // (void)checked;

    ui->selectAllCheckBox->setChecked(false);
    ui->preInstalledCheckBox->setChecked(false);
    ui->notInstalledCheckBox->setChecked(false);

    for(int i = 0; i < ui->tableWidget->rowCount(); i++)
        ui->tableWidget->item(i, 0)->setCheckState(Qt::Unchecked);
    ui->selectNoneCheckBox->setChecked(true);
}

void DownloadForm::on_preInstalledCheckBox_clicked(bool checked)
{
    ui->selectAllCheckBox->setChecked(false);
    ui->selectNoneCheckBox->setChecked(false);
    ui->notInstalledCheckBox->setChecked(false);

    if(checked)
    {
        for(int i = 0; i < ui->tableWidget->rowCount(); i++)
            if(preInstalled.contains(i))
                ui->tableWidget->item(i, 0)->setCheckState(Qt::Checked);
            else
                ui->tableWidget->item(i, 0)->setCheckState(Qt::Unchecked);
        ui->preInstalledCheckBox->setChecked(true);
    }
    else
        for(int i = 0; i < preInstalled.count(); i++)
            ui->tableWidget->item(preInstalled[i], 0)->setCheckState(Qt::Unchecked);
}

void DownloadForm::on_notInstalledCheckBox_clicked(bool checked)
{
    ui->selectAllCheckBox->setChecked(false);
    ui->selectNoneCheckBox->setChecked(false);
    ui->preInstalledCheckBox->setChecked(false);

    if(checked)
    {
        for(int i = 0; i < ui->tableWidget->rowCount(); i++)
            if(notInstalled.contains(i))
                ui->tableWidget->item(i, 0)->setCheckState(Qt::Checked);
            else
                ui->tableWidget->item(i, 0)->setCheckState(Qt::Unchecked);
        ui->notInstalledCheckBox->setChecked(true);
    }
    else
        for(int i = 0; i < notInstalled.count(); i++)
            ui->tableWidget->item(notInstalled[i], 0)->setCheckState(Qt::Unchecked);
}

void DownloadForm::on_comboBox_currentIndexChanged(int index)
{
    if(!xmlPoetList.isEmpty())
    {
        totalSize = 0;
        selectedPoets.clear();
        disconnect(ui->tableWidget, &QTableWidget::itemChanged, this, &DownloadForm::slotTableWidgetItemChanged);
        if(index == 1)
            tableWidgetDownloadList(ui->tableWidget, appSettings->mainDB, xmlPoetList, preInstalled, notInstalled, appSettings->isDarkMode, NotInstalledItems);
        else if(index == 2)
            tableWidgetDownloadList(ui->tableWidget, appSettings->mainDB, xmlPoetList, preInstalled, notInstalled, appSettings->isDarkMode, PreInstalledItems);
        else
            tableWidgetDownloadList(ui->tableWidget, appSettings->mainDB, xmlPoetList, preInstalled, notInstalled, appSettings->isDarkMode);
        connect(ui->tableWidget, &QTableWidget::itemChanged, this, &DownloadForm::slotTableWidgetItemChanged);

        ui->selectAllCheckBox->setChecked(false);
        ui->selectNoneCheckBox->setChecked(false);
        ui->preInstalledCheckBox->setChecked(false);
        ui->notInstalledCheckBox->setChecked(false);

        clearLabels();
    }
}

void DownloadForm::startDownload()
{
    downloadFlag = IsDownloading;

    ui->btnSearchOnGanjoor->setEnabled(false);
    ui->lineEdit->setEnabled(false);
    ui->comboBox->setEnabled(false);
    ui->comboBoxSave->setEnabled(false);
    ui->notInstalledCheckBox->setEnabled(false);
    ui->preInstalledCheckBox->setEnabled(false);
    ui->selectAllCheckBox->setEnabled(false);
    ui->selectNoneCheckBox->setEnabled(false);
    ui->tableWidget->setEnabled(false);
    ui->btnDownload->setEnabled(false);

    ui->btnClose->setText("توقف و بستن");
    ui->btnCancel->show();
    ui->labelPoetName->show();
    ui->progressBarOne->show();
    ui->progressBarAll->show();

    allSelectedCount = selectedPoets.count();
    ui->progressBarAll->setRange(0, allSelectedCount);
    ui->progressBarAll->setValue(0);
}

void DownloadForm::startDownloadAbort()
{
    ui->btnSearchOnGanjoor->setEnabled(true);
    ui->lineEdit->setEnabled(true);
    ui->comboBox->setEnabled(true);
    ui->comboBoxSave->setEnabled(true);
    ui->notInstalledCheckBox->setEnabled(true);
    ui->preInstalledCheckBox->setEnabled(true);
    ui->selectAllCheckBox->setEnabled(true);
    ui->selectNoneCheckBox->setEnabled(true);
    ui->tableWidget->setEnabled(true);
    ui->btnDownload->setEnabled(true);

    ui->btnClose->setText("بستن");
    ui->btnCancel->hide();
    ui->labelPoetName->hide();
    ui->progressBarOne->hide();
    ui->progressBarAll->hide();

    exportDB.remove();
    commonPoets.clear();
}

void DownloadForm::downloadLoopEnd()
{
    ui->labelPoetName->hide();
    ui->labelFinish->show();
    ui->progressBarOne->hide();

    if(downloadFlag == Canceled || downloadFlag == Error)
    {
        if(isXml)
            removeTempDir(xmlDirName);
        else if(!isXml && (downloadType == ImportToMainDB || downloadType == ExportToNewDB))
            removeTempDir(dlDirName);
        else if(!isXml && downloadType == OnlyDownloadFiles)
            QFile::remove(onlyDownloadFilesDirPath + "/" + dlFileName);
    }

    if(downloadFlag == Finished)
        messageBox("گزارش", "<b>تبریک!</b><br />دانلود با موفقیت به پایان رسید.", Information, this);
    else if(downloadFlag == Canceled && !cancelClose)
        messageBox("گزارش", "دانلود توسط شما متوقف شد!", Information, this);
    else if(downloadFlag == Error)
        messageBox("خطا", "متأسفانه هنگام دانلود خطایی رخ داد و فرآیند دانلود متوقف شد!", Critical, this);

    if(appSettings->mainDBPath.isEmpty() && downloadType == ExportToNewDB)
    {
        appSettings->mainDBPath = exportDB.DBPath();
        emit sigMainDBChanged();
    }

    emit sigUpdatePoetList();
    downloadFlag = None;

    refreshForm();
    on_comboBox_currentIndexChanged(ui->comboBox->currentIndex());
}

void DownloadForm::refreshForm()
{
    exportDB.remove();
    selectedPoets.clear();
    preInstalled.clear();
    notInstalled.clear();
    commonPoetsChecked = false;
    commonPoets.clear();
    removePreVer = false;
    totalSize = 0;
    downloadedDB = 0;
    downloadedSize = 0;

    startDownloadAbort();
    clearLabels();

    ui->selectAllCheckBox->setChecked(false);
    ui->selectNoneCheckBox->setChecked(false);
    ui->preInstalledCheckBox->setChecked(false);
    ui->notInstalledCheckBox->setChecked(false);

    ui->labelPoetName->setText("");
    ui->labelFinish->hide();
}

void DownloadForm::writeToDB(const QString &databasePath, int speed)
{
    if(downloadType == ImportToMainDB)
        importDatabase(appSettings->mainDB, databasePath, removePreVer, speed);
    else if(downloadType == ExportToNewDB)
        importDatabase(exportDB.DB(), databasePath, removePreVer, speed);
}

void DownloadForm::on_comboBoxSave_currentIndexChanged(int index)
{
    downloadType = static_cast<DownloadType>(ui->comboBoxSave->itemData(index, Qt::UserRole).toInt());
    exportToNewDBPath.clear();
    onlyDownloadFilesDirPath.clear();

    if(downloadType == ImportToMainDB && !appSettings->mainDBPath.isEmpty())
        ui->lineEditSaveLocation->setText(QDir::toNativeSeparators(appSettings->mainDBPath));
    else
        ui->lineEditSaveLocation->clear();
}

bool DownloadForm::check_DB_DirPath()
{
    if(downloadType == ImportToMainDB)
    {
        if(!appSettings->mainDBPath.isEmpty())
        {
            return true;
        }
        else
        {
            messageBox("هشدار", "شما پایگاه دادهٔ فعالی ندارید. لطفا ابتدا یک پایگاه داده ایجاد کنید.", Warning, this);
            appSettings->mainDBPath = createDBDialog(this);
            if(!appSettings->mainDBPath.isEmpty())
            {
                emit sigMainDBChanged();
                ui->lineEditSaveLocation->setText(QDir::toNativeSeparators(appSettings->mainDBPath));
                return true;
            }
        }
    }
    else if(downloadType == ExportToNewDB)
    {
        if(exportToNewDBPath.isEmpty())
            exportToNewDBPath = createDBDialog(this, QDir::homePath() + "/exportedDB_" + nowDate("-") + "_" + nowTime("") + ".s3db");

        if(!exportToNewDBPath.isEmpty())
        {
            ui->lineEditSaveLocation->setText(QDir::toNativeSeparators(exportToNewDBPath));
            exportDB.setDatabase(exportToNewDBPath, "exportDatabase");
            return true;
        }
    }
    else if(downloadType == OnlyDownloadFiles)
    {
        if(!onlyDownloadFilesDirPath.isEmpty())
        {
            return true;
        }
        else
        {
            onlyDownloadFilesDirPath = writableDirDialog(this);
            if(!onlyDownloadFilesDirPath.isEmpty())
            {
                ui->lineEditSaveLocation->setText(QDir::toNativeSeparators(onlyDownloadFilesDirPath));
                return true;
            }
        }
    }

    return false;
}

void DownloadForm::clearLabels()
{
    ui->labelSelectedCount->setText("انتخاب‌شده:   ");
    ui->labelSelectedSize->setText("حجم کل:   ");
    ui->labelDlInstalledCount->setText("دانلود و نصب‌شده:   ");
}

/*
    [Ghazal: The library of persian poetry]
    Publisher: Rosybit
    Url: http://www.rosybit.com
    GitHub: https://github.com/abroshan39/ghazal
    Author: Aboutaleb Roshan [ab.roshan39@gmail.com]
    License: MIT License
*/

#include "settingsform.h"
#include "ui_settingsform.h"
#include "appthemes.h"

#include <QFileDialog>
#include <QDir>
#include <QFileInfo>
#include <QFontDialog>
#include <QFont>

SettingsForm::SettingsForm(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SettingsForm)
{
    ui->setupUi(this);
}

SettingsForm::SettingsForm(AppSettings *appSettings, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SettingsForm)
{
    ui->setupUi(this);

    this->appSettings = appSettings;

    resize((int)(755 * appSettings->screenRatio), (int)(455 * appSettings->screenRatio));
    setGeometry(QStyle::alignedRect(Qt::RightToLeft, Qt::AlignCenter, size(), QGuiApplication::primaryScreen()->availableGeometry()));
    setWindowTitle("تنظیمات");
    setWindowIcon(QIcon(":/files/images/ghazal-256x256.png"));
    setWindowModality(Qt::WindowModal);

    preCreator();

    newDataDir = dataDir();
    newMainDBPath = appSettings->mainDBPath;

    setLineEditPath(ui->lineEditDataDir, newDataDir, FileDirType::Dir);
    setLineEditPath(ui->lineEditDatabase, newMainDBPath, FileDirType::File);
    ui->spinBoxHemistich->setMinimum(appSettings->hemistichDistanceMin);
    ui->spinBoxHemistich->setMaximum(appSettings->hemistichDistanceMax);
    ui->checkBoxDarkMode->setChecked(appSettings->isDarkMode);
    ui->checkBoxSaveHistoryOnExit->setChecked(appSettings->saveHistoryOnExit);
    ui->spinBoxHemistich->setValue(appSettings->hemistichDistance);
    ui->labelText->setText(appSettings->viewFN);
    ui->spinBoxText->setValue(appSettings->viewFS.toDouble());
    ui->labelList->setText(appSettings->listFN);
    ui->spinBoxList->setValue(appSettings->listFS.toDouble());
    ui->labelGlo->setText(appSettings->appFN);
    ui->spinBoxGlo->setValue(appSettings->appFS.toDouble());

    if(appSettings->viewFN != "Sahel")
        ui->radioTextSys->setChecked(true);
    if(appSettings->listFN != "Sahel")
        ui->radioListSys->setChecked(true);
    if(appSettings->appFN != "Sahel")
        ui->radioGloSys->setChecked(true);

    QString historyFilePath = dataDir() + "/" + Constants::HistoryFileName;
    if(!QFileInfo(historyFilePath).isFile())
        ui->labelDeleteHistory->hide();
}

SettingsForm::~SettingsForm()
{
    delete ui;
}

void SettingsForm::keyPressEvent(QKeyEvent *e)
{
    if(e->key() == Qt::Key_Escape)
        on_btnClose_clicked();
}

void SettingsForm::preCreator()
{
    preIsDarkMode = appSettings->isDarkMode;
    preHemistichDistance = appSettings->hemistichDistance;
    preViewFN = appSettings->viewFN;
    preViewFS = appSettings->viewFS;
    preListFN = appSettings->listFN;
    preListFS = appSettings->listFS;
    preAppFN = appSettings->appFN;
    preAppFS = appSettings->appFS;
}

void SettingsForm::setLineEditPath(QLineEdit *lineEdit, const QString &path, FileDirType fileDirType)
{
    if(path.isEmpty())
    {
        lineEdit->clear();
        return;
    }

    QString pathAbsRel(createRelativePathIfPossible(path, fileDirType));
    if(QDir(pathAbsRel).isAbsolute())
        lineEdit->setText(QDir::toNativeSeparators(pathAbsRel));
    else
        lineEdit->setText(QString("%1   ->   %2").arg(QDir::toNativeSeparators(pathAbsRel), QDir::toNativeSeparators(absolutePath(pathAbsRel))));
    lineEdit->setCursorPosition(0);
}

void SettingsForm::on_btnClose_clicked()
{
    close();
}

void SettingsForm::on_btnText_clicked()
{
    bool ok;
    QFont font = QFontDialog::getFont(&ok, QFont(appSettings->viewFN, (int)ui->spinBoxText->text().toDouble()), this);
    if(ok)
    {
        ui->labelText->setText(font.family());
        ui->spinBoxText->setValue(font.pointSize());
        ui->radioTextSys->setChecked(true);
    }
}

void SettingsForm::on_btnList_clicked()
{
    bool ok;
    QFont font = QFontDialog::getFont(&ok, QFont(appSettings->listFN, (int)ui->spinBoxList->text().toDouble()), this);
    if(ok)
    {
        ui->labelList->setText(font.family());
        ui->spinBoxList->setValue(font.pointSize());
        ui->radioListSys->setChecked(true);
    }
}

void SettingsForm::on_btnGlo_clicked()
{
    bool ok;
    QFont font = QFontDialog::getFont(&ok, QFont(appSettings->appFN, (int)ui->spinBoxGlo->text().toDouble()), this);
    if(ok)
    {
        ui->labelGlo->setText(font.family());
        ui->spinBoxGlo->setValue(font.pointSize());
        ui->radioGloSys->setChecked(true);
    }
}

void SettingsForm::on_btnApply_clicked()
{
    appSettings->isDarkMode = ui->checkBoxDarkMode->isChecked();
    appSettings->saveHistoryOnExit = ui->checkBoxSaveHistoryOnExit->isChecked();
    appSettings->hemistichDistance = ui->spinBoxHemistich->value();
    appSettings->appFN = ui->labelGlo->text();
    appSettings->appFS = ui->spinBoxGlo->text();
    appSettings->listFN = ui->labelList->text();
    appSettings->listFS = ui->spinBoxList->text();
    appSettings->viewFN = ui->labelText->text();
    appSettings->viewFS = ui->spinBoxText->text();
    appSettings->viewFSCurrent = appSettings->viewFS;

    if(appSettings->appFN != preAppFN || appSettings->appFS != preAppFS || appSettings->listFN != preListFN ||
       appSettings->listFS != preListFS || appSettings->viewFN != preViewFN || appSettings->viewFS != preViewFS ||
       appSettings->isDarkMode != preIsDarkMode || appSettings->hemistichDistance != preHemistichDistance)
    {
        applyChanges();
        emit sigTabTheme();
        emit sigTabFormSize();
        emit sigThemeAndMenuFont();
    }

    if(newDataDir != dataDir())
    {
        setDataDir(createRelativePathIfPossible(newDataDir, FileDirType::Dir));
        emit sigWriteSettings();
    }

    if(newMainDBPath != appSettings->mainDBPath)
    {
        appSettings->mainDBPath = newMainDBPath;
        emit sigMainDBChanged();
    }

    preCreator();
}

void SettingsForm::applyChanges()
{
    QString addStyle = appStyleSheet(appSettings->appFN, appSettings->appFS, appSettings->listFN, appSettings->listFS, appSettings->viewFN, appSettings->viewFSCurrent);
    if(appSettings->isDarkMode)
        QApplication::setStyle(new DarkStyle(addStyle));
    else
        QApplication::setStyle(new LightStyle(addStyle));
}

void SettingsForm::on_btnOK_clicked()
{
    on_btnApply_clicked();
    on_btnClose_clicked();
}

void SettingsForm::on_btnDefault_clicked()
{
    ui->checkBoxDarkMode->setChecked(false);
    ui->checkBoxSaveHistoryOnExit->setChecked(true);
    ui->spinBoxHemistich->setValue(60);
    ui->labelGlo->setText("Sahel");
    ui->spinBoxGlo->setValue(10.5);
    ui->labelText->setText("Sahel");
    ui->spinBoxText->setValue(11);
    ui->labelList->setText("Sahel");
    ui->spinBoxList->setValue(11);

    newDataDir = dataDir();
    newMainDBPath = appSettings->mainDBPath;

    setLineEditPath(ui->lineEditDataDir, newDataDir, FileDirType::Dir);
    setLineEditPath(ui->lineEditDatabase, newMainDBPath, FileDirType::File);
    on_btnApply_clicked();
}

void SettingsForm::on_btnBrowseDataDir_clicked()
{
    QString dir_path = QFileDialog::getExistingDirectory(this, "Select Directory", dataDir(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if(!dir_path.isEmpty())
    {
        if(QFileInfo(dir_path).isWritable())
        {
            newDataDir = dir_path;
            setLineEditPath(ui->lineEditDataDir, dir_path, FileDirType::Dir);
        }
        else
        {
            messageBox("خطا", "این مسیر قابل نوشتن نیست! لطفا مسیر دیگری انتخاب کنید.", Warning, this);
        }
    }
}

void SettingsForm::on_btnBrowseDatabase_clicked()
{
    QString filter = "Database files (*.s3db *.db *.sqlite3 *.sqlite *.gdb);;All files (*)";
    QString file_name = QFileDialog::getOpenFileName(this, "Open", (appSettings->mainDBPath.isEmpty() ? QDir::homePath() : QFileInfo(appSettings->mainDBPath).path()), filter);
    if(!file_name.isEmpty())
    {
        if(isStdGanjoorDB(file_name))
        {
            newMainDBPath = file_name;
            setLineEditPath(ui->lineEditDatabase, file_name, FileDirType::File);
        }
        else
        {
            messageBox("خطا", "<b>خطا</b>:<br />فایل انتخاب‌شده قالب استانداردی ندارد!", Critical, this);
        }
    }
}

void SettingsForm::on_radioTextSahel_clicked()
{
    ui->labelText->setText("Sahel");
}

void SettingsForm::on_radioListSahel_clicked()
{
    ui->labelList->setText("Sahel");
}

void SettingsForm::on_radioGloSahel_clicked()
{
    ui->labelGlo->setText("Sahel");
}

void SettingsForm::on_labelDeleteHistory_linkActivated(const QString &link)
{
    Q_UNUSED(link);  // (void)link;

    int reply = messageBox("حذف؟", "آیا از حذف فایل تاریخچه مطمئن هستید؟  ", WarningQuestion, this);
    if(reply == QMessageBox::Yes)
    {
        QString historyFilePath = dataDir() + "/" + Constants::HistoryFileName;
        QFile::remove(historyFilePath);
        if(!QFileInfo(historyFilePath).isFile())
            ui->labelDeleteHistory->hide();
    }
}

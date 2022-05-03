/*
    [Ghazal: The library of persian poetry]
    Publisher: Rosybit
    Url: http://www.rosybit.com
    GitHub: https://github.com/abroshan39/ghazal
    Version: 1.4
    Author: Aboutaleb Roshan [ab.roshan39@gmail.com]
    License: MIT License
*/

#include "settingsform.h"
#include "ui_settingsform.h"
#include "appthemes.h"

#include <QFileDialog>
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

    setGeometry(QStyle::alignedRect(Qt::RightToLeft, Qt::AlignCenter, size(), QGuiApplication::primaryScreen()->availableGeometry()));
    setWindowTitle("تنظیمات");
    setWindowIcon(QIcon(":/files/images/ghazal-256x256.png"));
    setWindowModality(Qt::WindowModal);
    ui->spinBoxHemistich->setMinimum(appSettings->hemistichDistanceMin);
    ui->spinBoxHemistich->setMaximum(appSettings->hemistichDistanceMax);

    preCreator();

    ui->lineEdit->setText(QDir::toNativeSeparators(appSettings->mainDBPath));
    ui->darkModeCheckBox->setChecked(appSettings->isDarkMode);
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
}

SettingsForm::~SettingsForm()
{
    delete ui;
}

void SettingsForm::preCreator()
{
    preMainDBPath = appSettings->mainDBPath;
    preIsDarkMode = appSettings->isDarkMode;
    preHemistichDistance = appSettings->hemistichDistance;
    preViewFN = appSettings->viewFN;
    preViewFS = appSettings->viewFS;
    preListFN = appSettings->listFN;
    preListFS = appSettings->listFS;
    preAppFN = appSettings->appFN;
    preAppFS = appSettings->appFS;
}

void SettingsForm::keyPressEvent(QKeyEvent *e)
{
    if(e->key() == Qt::Key_Escape)
        on_btnClose_clicked();
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
    appSettings->mainDBPath = QDir::fromNativeSeparators(ui->lineEdit->text());
    appSettings->isDarkMode = ui->darkModeCheckBox->isChecked();
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
        emit sigAdjustMenuFont();
    }

    if(appSettings->mainDBPath != preMainDBPath)
        emit sigMainDBChanged();

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
    ui->darkModeCheckBox->setChecked(false);
    ui->spinBoxHemistich->setValue(60);
    ui->labelGlo->setText("Sahel");
    ui->spinBoxGlo->setValue(10.5);
    ui->labelText->setText("Sahel");
    ui->spinBoxText->setValue(11);
    ui->labelList->setText("Sahel");
    ui->spinBoxList->setValue(11);

    on_btnApply_clicked();
}

void SettingsForm::on_btnBrowse_clicked()
{
    QString filter = "Database files (*.s3db *.db *.sqlite3 *.sqlite *.gdb);;All files (*.*)";
    QString file_name = QFileDialog::getOpenFileName(this, "Open", QDir::homePath(), filter);
    if(!file_name.isEmpty())
    {
        if(isStdGanjoorDB(file_name))
            ui->lineEdit->setText(QDir::toNativeSeparators(file_name));
        else
            messageBox("خطا", "<b>خطا</b>:<br />فایل انتخاب‌شده قالب استانداردی ندارد!", Critical, this);
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

/*
    [Ghazal: The library of persian poetry]
    Publisher: Rosybit
    Url: http://www.rosybit.com
    GitHub: https://github.com/abroshan39/ghazal
    Author: Aboutaleb Roshan [ab.roshan39@gmail.com]
    License: MIT License
*/

#ifndef SETTINGSFORM_H
#define SETTINGSFORM_H

#include <QMainWindow>
#include <QLineEdit>
#include "common_functions.h"

namespace Ui {
class SettingsForm;
}

class SettingsForm : public QMainWindow
{
    Q_OBJECT

public:
    explicit SettingsForm(QWidget *parent = nullptr);
    SettingsForm(AppSettings *appSettings, QWidget *parent = nullptr);
    ~SettingsForm();

signals:
    void sigWriteSettings();
    void sigMainDBChanged();
    void sigTabTheme();
    void sigTabFormSize();
    void sigThemeAndMenuFont();

public slots:
    void applyChanges();
    void preCreator();
    void setLineEditPath(QLineEdit *lineEdit, const QString &path, FileDirType fileDirType);

private slots:
    void on_btnApply_clicked();
    void on_btnOK_clicked();
    void on_btnClose_clicked();
    void on_btnDefault_clicked();
    void on_btnBrowseDataDir_clicked();
    void on_btnBrowseDatabase_clicked();
    void on_btnGlo_clicked();
    void on_btnText_clicked();
    void on_btnList_clicked();
    void on_radioTextSahel_clicked();
    void on_radioListSahel_clicked();
    void on_radioGloSahel_clicked();
    void on_labelDeleteHistory_linkActivated(const QString &link);

protected:
    void keyPressEvent(QKeyEvent *e) override;

private:
    Ui::SettingsForm *ui;
    AppSettings *appSettings;
    QString newDataDir;
    QString newMainDBPath;
    QString preAppFN;
    QString preAppFS;
    QString preListFN;
    QString preListFS;
    QString preViewFN;
    QString preViewFS;
    bool preIsDarkMode;
    int preHemistichDistance;
};

#endif // SETTINGSFORM_H

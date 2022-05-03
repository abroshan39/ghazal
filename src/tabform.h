/*
    [Ghazal: The library of persian poetry]
    Publisher: Rosybit
    Url: http://www.rosybit.com
    GitHub: https://github.com/abroshan39/ghazal
    Version: 1.4
    Author: Aboutaleb Roshan [ab.roshan39@gmail.com]
    License: MIT License
*/

#ifndef TABFORM_H
#define TABFORM_H

#include <QWidget>
#include <QScrollBar>
#include "common_functions.h"

namespace Ui {
class TabForm;
}

class TabForm : public QWidget
{
    Q_OBJECT

public:
    explicit TabForm(QWidget *parent = nullptr);
    TabForm(AppSettings *appSettings, QWidget *parent = nullptr);
    ~TabForm();

signals:
    void sigTabLastLevelIDChanged();
    void sigBookmarkChanged();
    void sigSelectedText(const QString &text);

public slots:
    void slotSetTabContent(const QString &levelID, bool setFocusListWidget = false, bool rememberScrollBarValue = false, const QStringList &highlightText = QStringList(), const QString &bookmarkVerseID = QString());
    void slotTabFormSize();
    void slotTabTheme();
    void slotTabHeaderLabel();
    void listWidgetKeyPressed(QObject *object, QKeyEvent *event);
    void textBrowserKeyPressed(QObject *object, QKeyEvent *event);
    void contextMenuTextBrowser(const QPoint &pos);
    void contextMenuLabel(const QPoint &pos);
    void setContents(const QString &levelID, bool clearPreItemRow = true, bool rememberScrollBarValue = false, const QStringList &highlightText = QStringList(), const QString &bookmarkVerseID = QString());
    QString getHyperLink(const GanjoorPath &gp);
    void actionCopy();
    void actionCopyLabel();
    void actionCopyAddress();
    void actionCopyAddressLabel();
    void actionSelectAll();
    void actionSelectAllLabel();
    void actionBookmark();
    void actionAbjad();

private slots:
    void on_listWidget_doubleClicked(const QModelIndex &index);
    void on_label_linkActivated(const QString &link);
    void on_textBrowser_highlighted(const QUrl &arg1);
    void on_label_linkHovered(const QString &link);
    void on_textBrowser_selectionChanged();

private:
    Ui::TabForm *ui;
    AppSettings *appSettings;
    QList<int> previousItemRow;
    QStringList tabPath;

    QString fullAddress;
    QString hrefAnchor;
    QString hrefAnchorLabel;
    QString verse_id;
};

#endif // TABFORM_H

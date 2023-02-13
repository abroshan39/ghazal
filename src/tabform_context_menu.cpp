/*
    [Ghazal: The library of persian poetry]
    Publisher: Rosybit
    Url: http://www.rosybit.com
    GitHub: https://github.com/abroshan39/ghazal
    Author: Aboutaleb Roshan [ab.roshan39@gmail.com]
    License: MIT License
*/

#include "tabform.h"
#include "ui_tabform.h"
#include "abjadformmini.h"

#include <QTextDocumentFragment>
#include <QClipboard>
#include <QMenu>
#include <QShortcut>

void TabForm::contextMenuTextBrowser(const QPoint &pos)
{
    Q_UNUSED(pos);  // (void)pos;

    fullAddress.clear();

    QMenu *contextMenu;
    QAction *bookmarkAction;
    QAction *abjadAction;
    QAction *copyAction;
    QAction *copyAddressAction;
    QAction *selectAllAction;

    contextMenu = new QMenu("Context Menu", this);
    abjadAction = new QAction("محاسبه ابجد", this);
    copyAction = new QAction("کپی", this);
    copyAddressAction = new QAction("کپی آدرس", this);
    selectAllAction = new QAction("انتخاب همه", this);

    verse_id.clear();
    bool preValue = false;
    if(!hrefAnchor.isEmpty())
    {
        QStringList hrefAnchorList = hrefAnchor.split("-");
        if(hrefAnchorList.count() == 2)
            verse_id = "-1";
        else
            verse_id = hrefAnchorList.last();
        preValue = isBookmarked(appSettings->mainDB, appSettings->tabCurrentPoem.value(this), verse_id);
    }

    if(preValue)
        bookmarkAction = new QAction("حذف نشانه", this);
    else
        bookmarkAction = new QAction("نشانه‌گذاری", this);

    if(verse_id.isEmpty())
        bookmarkAction->setEnabled(false);

    if(!ui->textBrowser->textCursor().hasSelection())
    {
        abjadAction->setEnabled(false);
        copyAction->setEnabled(false);
    }

    copyAction->setShortcut(QKeySequence("Ctrl+C"));
    selectAllAction->setShortcut(QKeySequence("Ctrl+A"));

    contextMenu->addAction(bookmarkAction);
    contextMenu->addSeparator();
    contextMenu->addAction(abjadAction);
    contextMenu->addSeparator();
    contextMenu->addAction(copyAction);
    if(QGuiApplication::keyboardModifiers().testFlag(Qt::ShiftModifier) && !hrefAnchor.isEmpty())
    {
        QStringList hrefAnchorList = hrefAnchor.split("-");
        GanjoorPath gp = recursiveIDs(appSettings->mainDB, hrefAnchorList.at(0), hrefAnchorList.at(1));
        int iLast = gp.id.count() - 1;
        for(int i = iLast; i >= 0; i--)
            fullAddress += gp.id[i] + "->";
        if(hrefAnchorList.count() == 2)
            fullAddress = getPoetIDByCatID(appSettings->mainDB, gp.id[iLast]) + ": " + fullAddress.left(fullAddress.size() - 2);
        else
            fullAddress = getPoetIDByCatID(appSettings->mainDB, gp.id[iLast]) + ": " + fullAddress.left(fullAddress.size() - 2) + ": " + hrefAnchorList.at(2) + " [" + getPositionByPoemIDVorder(appSettings->mainDB, hrefAnchorList.at(1), hrefAnchorList.at(2)) + "]";
        contextMenu->addAction(copyAddressAction);
    }
    contextMenu->addAction(selectAllAction);

    connect(bookmarkAction, &QAction::triggered, this, &TabForm::actionBookmark);
    connect(abjadAction, &QAction::triggered, this, &TabForm::actionAbjad);
    connect(copyAction, &QAction::triggered, this, &TabForm::actionCopy);
    connect(copyAddressAction, &QAction::triggered, this, &TabForm::actionCopyAddress);
    connect(selectAllAction, &QAction::triggered, this, &TabForm::actionSelectAll);

    contextMenu->exec(QCursor::pos());

    disconnect(bookmarkAction, &QAction::triggered, this, &TabForm::actionBookmark);
    disconnect(abjadAction, &QAction::triggered, this, &TabForm::actionAbjad);
    disconnect(copyAction, &QAction::triggered, this, &TabForm::actionCopy);
    disconnect(copyAddressAction, &QAction::triggered, this, &TabForm::actionCopyAddress);
    disconnect(selectAllAction, &QAction::triggered, this, &TabForm::actionSelectAll);

    delete bookmarkAction;
    delete abjadAction;
    delete copyAction;
    delete copyAddressAction;
    delete selectAllAction;
    delete contextMenu;
}

void TabForm::actionCopy()
{
//    QClipboard *clipboard = QApplication::clipboard();
//    QTextDocumentFragment selectedText(ui->textBrowser->textCursor().selection());
//    clipboard->setText(selectedText.toPlainText());

    ui->textBrowser->copy();
}

void TabForm::actionCopyAddress()
{
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(fullAddress);
}

void TabForm::actionSelectAll()
{
    ui->textBrowser->selectAll();
}

void TabForm::actionBookmark()
{
    bool preValue = isBookmarked(appSettings->mainDB, appSettings->tabCurrentPoem.value(this), verse_id);
    setBookmarked(appSettings->mainDB, appSettings->tabCurrentPoem.value(this), verse_id, !preValue);

    if(verse_id != "-1" && appSettings->showBookmarks)
        setContents(appSettings->tabCurrentPoem.value(this), false, true);

    emit sigBookmarkChanged();
}

void TabForm::actionAbjad()
{
    if(!appSettings->isOpenAbjadFormMini)
    {
        AbjadFormMini *abjadFormMini = new AbjadFormMini(appSettings, this);
        abjadFormMini->show();
    }
    on_textBrowser_selectionChanged();
}

void TabForm::on_textBrowser_highlighted(const QUrl &arg1)
{
    hrefAnchor = arg1.toString();
}

void TabForm::contextMenuLabel(const QPoint &pos)
{
    Q_UNUSED(pos);  // (void)pos;

    fullAddress.clear();

    QMenu *contextMenu;
    QAction *copyAction;
    QAction *copyAddressAction;
    QAction *selectAllAction;

    contextMenu = new QMenu("Context Menu", this);
    copyAction = new QAction("کپی", this);
    copyAddressAction = new QAction("کپی آدرس", this);
    selectAllAction = new QAction("انتخاب همه", this);

    if(!ui->label->hasSelectedText())
        copyAction->setEnabled(false);

    copyAction->setShortcut(QKeySequence("Ctrl+C"));
    selectAllAction->setShortcut(QKeySequence("Ctrl+A"));

    contextMenu->addAction(copyAction);
    if(QGuiApplication::keyboardModifiers().testFlag(Qt::ShiftModifier) && !hrefAnchorLabel.isEmpty())
    {
        QStringList hrefAnchorList = hrefAnchorLabel.split("-");
        GanjoorPath gp = recursiveIDs(appSettings->mainDB, hrefAnchorList.at(0), hrefAnchorList.at(1));
        int iLast = gp.id.count() - 1;
        for(int i = iLast; i >= 0; i--)
            fullAddress += gp.id[i] + "->";
        fullAddress = getPoetIDByCatID(appSettings->mainDB, gp.id[iLast]) + ": " + fullAddress.left(fullAddress.size() - 2);
        contextMenu->addAction(copyAddressAction);
    }
    contextMenu->addAction(selectAllAction);

    connect(copyAction, &QAction::triggered, this, &TabForm::actionCopyLabel);
    connect(copyAddressAction, &QAction::triggered, this, &TabForm::actionCopyAddressLabel);
    connect(selectAllAction, &QAction::triggered, this, &TabForm::actionSelectAllLabel);

    contextMenu->exec(QCursor::pos());

    disconnect(copyAction, &QAction::triggered, this, &TabForm::actionCopyLabel);
    disconnect(copyAddressAction, &QAction::triggered, this, &TabForm::actionCopyAddressLabel);
    disconnect(selectAllAction, &QAction::triggered, this, &TabForm::actionSelectAllLabel);

    delete copyAction;
    delete copyAddressAction;
    delete selectAllAction;
    delete contextMenu;
}

void TabForm::actionCopyLabel()
{
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(ui->label->selectedText());
}

void TabForm::actionCopyAddressLabel()
{
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(fullAddress);
}

void TabForm::actionSelectAllLabel()
{
    QString plain = QTextDocumentFragment::fromHtml(ui->label->text()).toPlainText();
    ui->label->setSelection(0, plain.size());
}

void TabForm::on_label_linkHovered(const QString &link)
{
    hrefAnchorLabel = link;
}

void TabForm::on_textBrowser_selectionChanged()
{
    if(appSettings->isOpenAbjadForm || appSettings->isOpenAbjadFormMini)
    {
        QTextDocumentFragment selectedText(ui->textBrowser->textCursor().selection());
        emit sigSelectedText(correctHtmlTableText(selectedText.toPlainText()));
    }
}

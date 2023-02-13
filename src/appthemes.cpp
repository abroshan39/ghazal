/*
###############################################################################
#                                                                             #
# The MIT License                                                             #
#                                                                             #
# Copyright (C) 2017 by Juergen Skrotzky (JorgenVikingGod@gmail.com)          #
#               >> https://github.com/Jorgen-VikingGod                        #
#                                                                             #
# Sources: https://github.com/Jorgen-VikingGod/Qt-Frameless-Window-DarkStyle  #
#                                                                             #
###############################################################################
*/

#include "appthemes.h"

LightStyle::LightStyle(const QString &additionalStyle) : LightStyle(styleBase()) {addStyle = additionalStyle;}

LightStyle::LightStyle(QStyle *style) : QProxyStyle(style) {;}

QStyle *LightStyle::styleBase(QStyle *style) const
{
    QString styleName;

#if defined Q_OS_WIN
    if(QSysInfo::productType().toLower() == "windows" && QSysInfo::productVersion().toLower().contains("xp"))
        styleName = "windowsxp";
    else
        styleName = "windowsvista";
#elif defined Q_OS_MACOS
    styleName = "macintosh";
#else
    styleName = "fusion";
#endif

    QStyle *base = !style ? QStyleFactory::create(styleName) : style;
    return base;
}

QStyle *LightStyle::baseStyle() const {return styleBase();}

void LightStyle::polish(QPalette &palette)
{
    // fix a minor bug
    palette.setColor(QPalette::Link, Qt::blue);
}

void LightStyle::polish(QApplication *app)
{
    if(!app) return;

    // set stylesheet
    app->setStyleSheet(addStyle);
}

DarkStyle::DarkStyle(const QString &additionalStyle) : DarkStyle(styleBase()) {addStyle = additionalStyle;}

DarkStyle::DarkStyle(QStyle *style) : QProxyStyle(style) {;}

QStyle *DarkStyle::styleBase(QStyle *style) const
{
    QStyle *base = !style ? QStyleFactory::create("fusion") : style;
    return base;
}

QStyle *DarkStyle::baseStyle() const {return styleBase();}

void DarkStyle::polish(QPalette &palette)
{
    // modify palette to dark
    palette.setColor(QPalette::Window, QColor(53, 53, 53));
    palette.setColor(QPalette::WindowText, Qt::white);
    palette.setColor(QPalette::Disabled, QPalette::WindowText, QColor(127, 127, 127));
    palette.setColor(QPalette::Base, QColor(42, 42, 42));
    palette.setColor(QPalette::AlternateBase, QColor(66, 66, 66));
    palette.setColor(QPalette::ToolTipBase, Qt::white);
    palette.setColor(QPalette::ToolTipText, QColor(53, 53, 53));
    palette.setColor(QPalette::Text, Qt::white);
    palette.setColor(QPalette::Disabled, QPalette::Text, QColor(127, 127, 127));
    palette.setColor(QPalette::Dark, QColor(35, 35, 35));
    palette.setColor(QPalette::Shadow, QColor(20, 20, 20));
    palette.setColor(QPalette::Button, QColor(53, 53, 53));
    palette.setColor(QPalette::ButtonText, Qt::white);
    palette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(127, 127, 127));
    palette.setColor(QPalette::BrightText, Qt::red);
    palette.setColor(QPalette::Link, QColor(59, 120, 255));
    palette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    palette.setColor(QPalette::Disabled, QPalette::Highlight, QColor(80, 80, 80));
    palette.setColor(QPalette::HighlightedText, Qt::white);
    palette.setColor(QPalette::Disabled, QPalette::HighlightedText, QColor(127, 127, 127));
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)  // #if QT_VERSION >= 0x050C00
    palette.setColor(QPalette::PlaceholderText, QColor(127, 127, 127));
#endif
}

void DarkStyle::polish(QApplication *app)
{
    if(!app) return;

    // loadstylesheet
    QFile qfDarkstyle(QStringLiteral(":/darkstyle/darkstyle.qss"));
    if(qfDarkstyle.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        // set stylesheet
        QString qsStylesheet = QString::fromLatin1(qfDarkstyle.readAll());
        app->setStyleSheet(qsStylesheet + addStyle);
        qfDarkstyle.close();
    }
}

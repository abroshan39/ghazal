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

#ifndef APPTHEMES_HPP
#define APPTHEMES_HPP

#include <QApplication>
#include <QFile>
#include <QFont>
#include <QProxyStyle>
#include <QStyleFactory>

class LightStyle : public QProxyStyle
{
    Q_OBJECT

public:
    LightStyle(const QString &additionalStyle = "");
    explicit LightStyle(QStyle *style);

    QStyle *baseStyle() const;

    void polish(QPalette &palette) override;
    void polish(QApplication *app) override;

private:
    QStyle *styleBase(QStyle *style = Q_NULLPTR) const;
    QString addStyle;
};

class DarkStyle : public QProxyStyle
{
    Q_OBJECT

public:
    DarkStyle(const QString &additionalStyle = "");
    explicit DarkStyle(QStyle *style);

    QStyle *baseStyle() const;

    void polish(QPalette &palette) override;
    void polish(QApplication *app) override;

private:
    QStyle *styleBase(QStyle *style = Q_NULLPTR) const;
    QString addStyle;
};

#endif  // APPTHEMES_HPP

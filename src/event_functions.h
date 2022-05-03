/*
    [Ghazal: The library of persian poetry]
    Publisher: Rosybit
    Url: http://www.rosybit.com
    GitHub: https://github.com/abroshan39/ghazal
    Version: 1.4
    Author: Aboutaleb Roshan [ab.roshan39@gmail.com]
    License: MIT License
*/

#ifndef EVENTFUNCTIONS_H
#define EVENTFUNCTIONS_H

#include <QObject>
#include <QEvent>
#include <QKeyEvent>
#include <QSet>

class KeyPress : public QObject
{
    Q_OBJECT

public:
    explicit KeyPress(QObject *parent = nullptr);

signals:
    void keyPressed(QObject *, QKeyEvent *);

protected:
    bool eventFilter(QObject *object, QEvent *event) override;
};

class FocusWatcher : public QObject
{
    Q_OBJECT

public:
    explicit FocusWatcher(QObject *parent = nullptr);

signals:
    void focusChanged(QObject *, QEvent *);

protected:
    bool eventFilter(QObject *object, QEvent *event) override;
};

class ZWNJPress : public QObject
{
    Q_OBJECT

public:
    explicit ZWNJPress(QObject *parent = nullptr);

signals:
    void zwnjPressed(QObject *, Qt::KeyboardModifier);

protected:
    bool eventFilter(QObject *object, QEvent *event) override;

private:
    QSet<int> pressedKeys;
};

#endif // EVENTFUNCTIONS_H

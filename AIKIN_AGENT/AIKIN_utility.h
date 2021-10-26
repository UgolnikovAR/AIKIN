#pragma once
#include <QtGlobal> //Для упрощения подключается целый модуль. Позже надо разделить.
#include <QDebug>
#include <QMutex>

/*Здесь будут вспомогательные функции*/


class ThreadSavePrint
{
    QDebug pqdebug;
    static QMutex mut;

public:
    ThreadSavePrint()
        : pqdebug(qDebug())
    {
        QMutexLocker locker(&mut);
    }

    template<typename T>
    ThreadSavePrint& operator<<(T&& a)
    {

        pqdebug << a;
        return *this;
        /*Здесь было бы хорошо сделать неблокирующую
         * попытку блокировки QMutex::tryLock().
         * Данные от этих обращений должны накапливаться в односторонней очереди
         * и, при удачной блокировке, поступать на вывод в естесственном порядке.*/
    }
};
/*tsout создает временный потоко-безопасный синхронизированный объект QDebug.
 * Этот объект существует внутри стейтмента.*/
#define tsout ThreadSavePrint()

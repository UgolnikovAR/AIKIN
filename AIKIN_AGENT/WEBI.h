#pragma once


/* Реализация сетевого интерфейса:
 * LISTEN(ожидание)     - Блокировка, ожидание входящего соединения
 * CONNECT(соединение)  - Установка соединения с ожидающим объектом
 * ACCEPT(прием)        - Прием входящего соединения от объекта того же уровня
 * RECEIVE(прием)       - Блокировка, ожидание входящего сообщения
 * SEND(отправка)       - Отправка сообщения ожидающему объекту того же уровня
 * DISCONNECT(разрыв)   - Разрыв соединения
*/


#include <iostream>
#include <thread>
#include <chrono>

#include <QObject>
#include <QThread>
#include <QTcpSocket>
#include <QDebug>

#include "Spotter.h"
#include "../AIKIN_SERVER/MessageTypes.h"

class SocketWorker;

/* class WEBI. Модуль. Сетевой интерфейс.
 * Ожидает приём пакета.
 * При получении, расшифровывает пакет и формирует объект ArchitectState_data.
 * Объект ArchitectState_data передается в область вызывающей процедуры (Поток-1).
 */
//Интерфейс сетевого модуля

using messagesize_t = quint32;
class WEBI : public QThread
{
    Q_OBJECT //для сигнально-слотового соединения

public:
    explicit WEBI(Spotter*);
            ~WEBI();

    /* WEBI::run  -  Наблюдение за сетью,
     * вызов методов Spotter.*/
    void run(); //thread creates

private: //Процедуры работы с сетью
    void connectToServer(QString, quint16);
    void sentToServer(QByteArray&);

private slots:
    /*.. ..*/

private: //Организационные процедуры
    //static void exec(WEBI*); olds
    void at_work();
    void wait(quint32 msec);

private:
    SocketWorker* _sworker;
    QThread* _sw_thread;
    QTcpSocket* _socket;


    messagesize_t     _nNextBlockSize = 0;
    Spotter* sma_pspotter;

signals:
    void signalSentToServer(QString);
    void sigTest();
};


/*Класс обработки сигналов сокета WEBI*/
class SocketWorker
        : public QObject
{
    Q_OBJECT
    QTcpSocket* _socket;

public:
    SocketWorker(QTcpSocket*sock, Spotter* spot)
        : _socket(sock)
        , sma_pspotter(spot){}
    virtual ~SocketWorker(){};

public slots:
    void slotSentToServer(QString);
    void slotConnected();
    void slotReadyRead();
    void slotDisconnected();
    void pingToServer();


private:
    messagesize_t _nNextBlockSize = 0;
    Spotter* sma_pspotter;

signals:
    void registerDll(QString);
};

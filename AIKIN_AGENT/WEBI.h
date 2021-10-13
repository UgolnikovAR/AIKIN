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




/* class WEBI. Модуль. Сетевой интерфейс.
 * Ожидает приём пакета.
 * При получении, расшифровывает пакет и формирует объект ArchitectState_data.
 * Объект ArchitectState_data передается в область вызывающей процедуры (Поток-1).
 */
//Интерфейс сетевого модуля
class WEBI : public QThread
{
    Q_OBJECT //для сигнально-слотового соединения

public:
    explicit WEBI();
            ~WEBI();

    /* WEBI::run  -  Наблюдение за сетью,
     * вызов методов Spotter.*/
    void run(); //thread creates

private: //Процедуры работы с сетью
    void connectToServer(QString, quint16);
    void sentToServer(QByteArray&);
    void readLib();

private slots:
    void slotSentToServer(QString);
    void slotConnected();
    void slotReadyRead();
    void pingToServer();
    void slotError(QAbstractSocket::SocketError);
    void slotTest();


private: //Организационные процедуры
    //static void exec(WEBI*); olds
    void at_work();
    void wait(quint32 msec);

private:
    QTcpSocket* socket;

    using messagesize_t = quint32;
    messagesize_t     m_nNextBlockSize = 0;

signals:
    void signalSentToServer(QString);
    void sigTest();
};

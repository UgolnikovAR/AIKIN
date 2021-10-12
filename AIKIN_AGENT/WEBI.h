#pragma once

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
using nport_t = quint32;
//Интерфейс сетевого модуля
class WEBI : public QObject
{
    Q_OBJECT //для сигнально-слотового соединения

public:
    explicit WEBI();
            ~WEBI();
    QTcpSocket* getpSocket();

private: //Процедуры работы с сетью
    /*Соединение с сервером по, автоматически подобранным, параметрам*/
    void connectToServer();
    void connectToServer(QString, quint16);
    void sentToServer(QByteArray&);

private slots:
    void slotSentToServer(QString);
    void slotConnected();
    void slotReadyRead();
    void pingToServer();
    void slotError(QAbstractSocket::SocketError);

private: //Организационные процедуры
    QString actualHostName();
    nport_t actualPort();

private:
    QTcpSocket* socket;

    using messagesize_t = quint32;
    messagesize_t     m_nNextBlockSize = 0;

signals:
    void signalSentToServer(QString);
};


class WEBI_Thread
        : public QThread
{
    Q_OBJECT

    WEBI* webi;

public:
    WEBI_Thread();
    /*вызов методов Spotter.*/
    void run();

private: //Организационные процедуры
    void at_work();

signals:
    void signalSentToServer(QString);
};

#pragma once

#include <QWidget>
#include <QtWidgets>
#include <QTcpServer>
#include <QTcpSocket>


class TServer : public QWidget
{
    Q_OBJECT
private:
    QTcpServer* m_ptcpServer;
    QTextEdit*  m_ptxt;
    QPushButton* m_psentTxtFileButton;
    QPushButton* m_psentDllFileButton;

    using messagesize_t = quint32;
    messagesize_t     m_nNextBlockSize;

    QTcpSocket* m_tmpTcpSocket;

private:
    void sendToClient(QTcpSocket* pSocket, const QString& str);

public:
    TServer(int nPort, QWidget* pwgt = 0);

private slots:
    void slotSendTxtFileToClient();
    void slotSendDllFileToClient();

public slots:
    virtual void slotNewConnection();
            void slotReadClient   ();
};

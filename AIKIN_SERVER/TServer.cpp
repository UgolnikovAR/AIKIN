#include "TServer.h"
#include <QString>
#include <string>

TServer::TServer(int nPort, QWidget* pwgt /*= 0*/)
    : QWidget(pwgt)
    , m_nNextBlockSize(0)
{
    m_ptcpServer = new QTcpServer(this);
    //запускаем сервер команда listen
    if (!m_ptcpServer->listen(QHostAddress::Any, nPort)) {
            QMessageBox::critical(0,
                                  "Server Error",
                                  "Unable to start the server:"
                                  + m_ptcpServer->errorString()
                                 );
        m_ptcpServer->close();
        return;
    }

    connect(m_ptcpServer, SIGNAL(newConnection()),
            this,         SLOT(slotNewConnection())
           );

    m_ptxt = new QTextEdit;
    m_ptxt->setReadOnly(true);

    //Кнопка отправки текстового файла
    m_psentTxtFileButton = new QPushButton(this);
    m_psentTxtFileButton->setText("&sent text file");

    connect(m_psentTxtFileButton, SIGNAL(clicked()),
            this,      SLOT(slotSendTxtFileToClient())
           );

    //Кнопка отправки текстового файла
    m_psentDllFileButton = new QPushButton(this);
    m_psentDllFileButton->setText("&sent dll file");

    connect(m_psentDllFileButton, SIGNAL(clicked()),
            this,      SLOT(slotSendDllFileToClient())
            );

    //сетап лейаута
    QVBoxLayout* pvbxLayout = new QVBoxLayout;
    pvbxLayout->addWidget(new QLabel("<H1>Server</H1>"));
    pvbxLayout->addWidget(m_ptxt);
    pvbxLayout->addWidget(m_psentTxtFileButton);
    pvbxLayout->addWidget(m_psentDllFileButton);
    setLayout(pvbxLayout);
}


/*virtual*/ void TServer::slotNewConnection()
{
    m_tmpTcpSocket = m_ptcpServer->nextPendingConnection(); //временная абстракция от реализации системы зеркал
    auto& pClientSocket = m_tmpTcpSocket;
    //--------------------------------------------------

    connect(pClientSocket, SIGNAL(disconnected()),
            pClientSocket, SLOT(deleteLater())
            );
    connect(pClientSocket, SIGNAL(readyRead()),
            this,          SLOT(slotReadClient())
           );

    sendToClient(pClientSocket, "Server Response: Connected!");
    m_ptxt->append("Got connection");
}


void TServer::slotReadClient()
{
    QTcpSocket* m_tmpTcpSocket = (QTcpSocket*)sender(); //временная абстракция от реализации системы зеркал
    auto& pClientSocket = m_tmpTcpSocket;
    //--------------------------------------------------

    QDataStream in(pClientSocket);
    in.setVersion(QDataStream::Qt_5_3);
    for(;;) {
        if(!m_nNextBlockSize) {
            if(pClientSocket->bytesAvailable() < sizeof(messagesize_t)) {
                break;
            }
            in >> m_nNextBlockSize;
        }

        if(pClientSocket->bytesAvailable() < m_nNextBlockSize) {
            break;
        }
        QTime   time;
        QString str;
        in >> time >> str;

        QString strMessage =
                time.toString() + " " + "Client has sent - " + str;
        m_ptxt->append(strMessage);

        m_nNextBlockSize = 0;

        sendToClient(pClientSocket,
                     "Server Response: Received \"" + str + "\""
                    );
    }
}


void TServer::sendToClient(QTcpSocket* pSocket, const QString& str)
{
    quint16 message_t = 1; //1 - "string in buffer" type //refactor later

    QByteArray  arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_3);
    out << messagesize_t(0) << message_t << QTime::currentTime() << str;

    //корректировка размера сообщения
    out.device()->seek(0);
    out << messagesize_t(arrBlock.size() - sizeof(messagesize_t));

    pSocket->write(arrBlock);
    pSocket->flush();
}


void TServer::slotSendTxtFileToClient()
{
    auto& pSocket = m_tmpTcpSocket; //временная абстракция от реализации системы зеркал
    //---------------------------------

    quint16 message_t = 2; //2 - "txt file in buffer" type
    m_ptxt->append("*click*");

    QByteArray buff;
    QDataStream out(&buff, QIODevice::WriteOnly);

    //Открываем файл
    QFile lib("test.txt");
    if(!lib.exists()) qDebug() << " DBG file Not Exist";
    if(!lib.isOpen()) {
        lib.open(QIODevice::ReadOnly);
    } else m_ptxt->append("file already open");

    //Резервируем messagesize_t байт для размера файла
    //Читаем его целиком в буфер
    quint32 fsize = lib.size();
    out << messagesize_t{0} << message_t << lib.readAll();

    //дебаг размера буфера------------
    std::string str123 = std::to_string(fsize);
    QString dbgsize = QString::fromStdString(str123);
    dbgsize = QString("DBG size = ") + dbgsize;
    m_ptxt->append(dbgsize);
    //--------------------------------

    //Освобождаем файл
    lib.close();

    //Находим и перезаписываем размер буфера
    out.device()->seek(0);
    out << messagesize_t(buff.size() - sizeof(messagesize_t));

    //Пишем буфер в сокет
    pSocket->write(buff);
    pSocket->flush();

    m_ptxt->append("Txt sends");
}


void TServer::slotSendDllFileToClient()
{
    auto& pSocket = m_tmpTcpSocket; //временная абстракция от реализации системы зеркал
    //---------------------------------

    quint16 message_t = 3; //3 - "dll file in buffer" type
    m_ptxt->append("*click*");

    QByteArray buff;
    QDataStream out(&buff, QIODevice::WriteOnly);

    //Открываем файл
    QFile lib("AIKIN_LIB.dll");
    if(!lib.exists()) qDebug() << " DBG file Not Exist";
    if(!lib.isOpen()) {
        lib.open(QIODevice::ReadOnly);
    } else m_ptxt->append("file already open");

    //Резервируем messagesize_t байт для размера файла
    //Читаем его целиком в буфер
    quint32 fsize = lib.size();
    out << messagesize_t{0} << message_t << fsize << lib.readAll();

    /*дебаг размера буфера------------  //olds
    std::string str123 = std::to_string(fsize);
    QString dbgsize = QString::fromStdString(str123);
    dbgsize = QString("DBG sending file.. Size = ") + dbgsize;
    m_ptxt->append(dbgsize);
    */ //olds

    //Освобождаем файл
    lib.close();

    //Находим и перезаписываем размер буфера
    out.device()->seek(0);
    //auto DBG_MessageSize = messagesize_t(buff.size() - sizeof(messagesize_t)); //olds
    out << messagesize_t(buff.size() - sizeof(messagesize_t));

    //Пишем буфер в сокет
    pSocket->write(buff);
    pSocket->flush();

    m_ptxt->append("Lib sends");
}






















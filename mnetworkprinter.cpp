/* ********************************
 * 通过 wifi 连接网络小票打印机
 * 2019-03-05
 * Magic
 * ********************************/
#include "mnetworkprinter.h"
#include <QTcpSocket>
#include <QHostAddress>

MNetworkPrinter::MNetworkPrinter(QObject *parent) : QObject(parent)
{
    m_tcpSocket = new QTcpSocket(this);
    connect(m_tcpSocket, &QTcpSocket::connected, [=](){
        qWarning()<<__FILE__<<__FUNCTION__<<__LINE__<<"-------------------------------QTcpSocket::connected"<<m_port;
    });
    connect(m_tcpSocket, static_cast<void(QAbstractSocket::*)(QAbstractSocket::SocketError)>(&QAbstractSocket::error),
            [=](QAbstractSocket::SocketError socketError){
        qWarning()<<__FILE__<<__FUNCTION__<<__LINE__<<"==============================QAbstractSocket::SocketError"<<socketError<<m_port;
    });

    m_port = 8266; // 目前通过遍历 恩叶 NP58-W/NP58-WB 票据 + 标签 两用打印机 端口号为： 8266 可用
}

void MNetworkPrinter::connectToPrint(const QString &ipAddress, qint16 port)
{
    m_addressStr = ipAddress;
    m_port = port;
    m_tcpSocket->connectToHost(QHostAddress(m_addressStr), m_port);
}

QTcpSocket *MNetworkPrinter::tcpSocket()
{
    return m_tcpSocket;
}

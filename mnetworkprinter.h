#ifndef MNETWORKPRINTER_H
#define MNETWORKPRINTER_H

#include <QObject>

class QTcpSocket;
class MNetworkPrinter : public QObject
{
    Q_OBJECT
public:
    explicit MNetworkPrinter(QObject *parent = nullptr);

    Q_INVOKABLE void connectToPrint(const QString &ipAddress, qint16 port);
    QTcpSocket *tcpSocket();

signals:

public slots:

private:
    QTcpSocket *m_tcpSocket;
    QString m_addressStr;
    qint16 m_port;
};

#endif // MNETWORKPRINTER_H

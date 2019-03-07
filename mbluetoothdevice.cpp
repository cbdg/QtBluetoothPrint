/* ************************************
 * 蓝牙设备，方便存储已经发现的蓝牙设备，连接蓝牙，配对，发送打印命令等等
 * 2019-02-20
 * magic
 * ***********************************/
#include "mbluetoothdevice.h"
#include "mbluetoothdevice_p.h"
#include <QTcpSocket>
#include <QHostAddress>

/* **********************************
 * 私有数据类
 * **********************************/
MBluetoothDevicePrivate::MBluetoothDevicePrivate(MBluetoothDevice *parent)
{
    Q_Q(MBluetoothDevice);
    q_ptr = parent;

    isPaired = false;
    isConnected = false;
    isCurrent = false;
    socketConnectState = MBluetoothDevice::Unconnect;

    deviceType = 0;

    bluetoothSocket = nullptr;
    tcpSocket = nullptr;
}


/* ***************************************
 *
 *
 *
 *
 * **************************************/
MBluetoothDevice::MBluetoothDevice(QObject *parent)
    : QObject(parent)
    , d_ptr(new MBluetoothDevicePrivate(this))
{
    Q_D(MBluetoothDevice);

    // bluetoothsocekt connect
    d->bluetoothSocket = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol,this);
    connect(d->bluetoothSocket, SIGNAL(connected()), this, SLOT(bluetoothSocketConnected()));
    connect(d->bluetoothSocket, SIGNAL(error(QBluetoothSocket::SocketError)), this, SLOT(bluetoothSocketError(QBluetoothSocket::SocketError)));
    connect(d->bluetoothSocket, SIGNAL(disconnected()), this, SLOT(bluetoothSocketDisconnected()));

    // tcpsocket
    d->tcpSocket = new QTcpSocket(this);
    connect(d->tcpSocket, SIGNAL(connected()), this, SLOT(tcpSocketConnected()));
    connect(d->tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(tcpSocketError(QAbstractSocket::SocketError)));
    connect(d->tcpSocket, SIGNAL(disconnected()), this, SLOT(tcpSocketDisconnected()));
}

MBluetoothDevice::~MBluetoothDevice()
{

}

QString MBluetoothDevice::deviceName()
{
    Q_D(MBluetoothDevice);
    if (d->bluetoothDeviceInfo.isValid())
        return d->bluetoothDeviceInfo.name();
    else
        return d->deviceName;
}

void MBluetoothDevice::setDeviceName(const QString &name)
{
    Q_D(MBluetoothDevice);
    d->deviceName = name;
}

QString MBluetoothDevice::address()
{
    Q_D(MBluetoothDevice);
    if (d->bluetoothDeviceInfo.isValid())
        return d->bluetoothDeviceInfo.address().toString();
    else
        return d->address;
}

void MBluetoothDevice::setAddress(const QString &address)
{
    Q_D(MBluetoothDevice);
    d->address = address;
}

QString MBluetoothDevice::uuid()
{
    Q_D(MBluetoothDevice);
    if (d->bluetoothDeviceInfo.isValid())
        return d->bluetoothDeviceInfo.deviceUuid().toString();
    else
        return d->uuid;
}

void MBluetoothDevice::setUuid(const QString &uuid)
{
    Q_D(MBluetoothDevice);
    d->uuid = uuid;
}

bool MBluetoothDevice::isPaired()
{
    Q_D(MBluetoothDevice);
    return d->isPaired;
}

void MBluetoothDevice::setIsPaired(bool paired)
{
    Q_D(MBluetoothDevice);
    d->isPaired = paired;
    emit isPairedChanged();
}

bool MBluetoothDevice::isConnected()
{
    Q_D(MBluetoothDevice);
    return d->isConnected;
}

void MBluetoothDevice::setIsConnected(bool connected)
{
    Q_D(MBluetoothDevice);
    d->isConnected = connected;
    emit isConnectedChanged();
}

bool MBluetoothDevice::isCurrentDevice()
{
    Q_D(MBluetoothDevice);
    return d->isCurrent;
}

bool MBluetoothDevice::setIsCurrentDevice(bool current)
{
    Q_D(MBluetoothDevice);
    d->isCurrent = current;
    emit isCurrentDeviceChanged();

}

void MBluetoothDevice::setDeviceInfo(const QBluetoothDeviceInfo &deviceInfo)
{
    Q_D(MBluetoothDevice);
    d->bluetoothDeviceInfo = deviceInfo;
}

void MBluetoothDevice::setDeviceAddress(const QBluetoothAddress &deviceAddress)
{
    Q_D(MBluetoothDevice);
    d->bluetoothAddress = deviceAddress;
}

void MBluetoothDevice::setDeviceUuid(const QBluetoothUuid &uuid)
{
    Q_D(MBluetoothDevice);
    d->bluetoothUuid = uuid;
}

MBluetoothDevice::SOCKETSTATE MBluetoothDevice::socketConnectState()
{
    Q_D(MBluetoothDevice);
    return d->socketConnectState;
}

void MBluetoothDevice::setSocketConnectState(MBluetoothDevice::SOCKETSTATE connecteState)
{
    Q_D(MBluetoothDevice);
    d->socketConnectState = connecteState;
    emit socketConnectStateChanged();
}

quint8 MBluetoothDevice::majorDeviceClass()
{
    Q_D(MBluetoothDevice);
    return quint8(d->bluetoothDeviceInfo.majorDeviceClass());
}

quint8 MBluetoothDevice::minorDeviceClass()
{
    Q_D(MBluetoothDevice);
    return quint8(d->bluetoothDeviceInfo.minorDeviceClass());
}

QString MBluetoothDevice::ipAddress()
{
    Q_D(MBluetoothDevice);
    return d->ipAddressStr;
}

void MBluetoothDevice::setIpAddress(const QString &ip)
{
    Q_D(MBluetoothDevice);

    if (d->ipAddressStr == ip)
        return;
    d->ipAddressStr = ip;
    emit ipAddressChanged();
}

quint16 MBluetoothDevice::port()
{
    Q_D(MBluetoothDevice);
    return d->port;
}

void MBluetoothDevice::setPort(int p)
{
    Q_D(MBluetoothDevice);

    if(d->port == quint16(p))
        return;

    d->port = quint16(p);
    emit portChanged();
}

int MBluetoothDevice::deviceType()
{
    Q_D(MBluetoothDevice);
    return d->deviceType;
}

void MBluetoothDevice::setDeviceType(int type)
{
    Q_D(MBluetoothDevice);
    if (d->deviceType == type)
        return;

    d->deviceType = type;
}

void MBluetoothDevice::connectSocket()
{
    Q_D(MBluetoothDevice);
    qWarning()<<__FILE__<<__FUNCTION__<<__LINE__<<"====================="<<d->deviceType;
    if (d->deviceType == 0) {
        qWarning()<<__FILE__<<__FUNCTION__<<__LINE__<<"=====================";
        QString remoteAddressStr = address();
        if (d->bluetoothSocket->state() == QBluetoothSocket::UnconnectedState) {
            qWarning()<<__FILE__<<__FUNCTION__<<__LINE__<<"====================="<<remoteAddressStr;
            setSocketConnectState(MBluetoothDevice::Connecting);
            QBluetoothUuid uuid(QString("00001101-0000-1000-8000-00805F9B34FB"));
            d->bluetoothSocket->connectToService(QBluetoothAddress(remoteAddressStr), uuid);
        } else {
            if (d->bluetoothSocket->state() == QBluetoothSocket::ConnectedState) {
                //d->bluetoothSocket->disconnectFromService();
                setIsConnected(true);
                setSocketConnectState(MBluetoothDevice::Connected);
            }
        }
    } else if (d->deviceType == 1) {
        qWarning()<<__FILE__<<__FUNCTION__<<__LINE__<<"====================="<<d->deviceType<<d->tcpSocket->state();
        if (d->tcpSocket->state() == QTcpSocket::UnconnectedState) {
            qWarning()<<__FILE__<<__FUNCTION__<<__LINE__<<"====================="<<d->deviceType;
            setSocketConnectState(MBluetoothDevice::Connecting);
            d->tcpSocket->connectToHost(QHostAddress(d->ipAddressStr), d->port);
        } else {
            qWarning()<<__FILE__<<__FUNCTION__<<__LINE__<<"====================="<<d->deviceType;
            if (d->tcpSocket->state() == QTcpSocket::ConnectedState) {
                setIsConnected(true);
                setSocketConnectState(MBluetoothDevice::Connected);
            }
        }
    }
}

void MBluetoothDevice::disconnectSocket()
{
    Q_D(MBluetoothDevice);
    qWarning()<<__FILE__<<__FUNCTION__<<__LINE__<<"=================";
    if (d->deviceType == 0) {
        qWarning()<<__FILE__<<__FUNCTION__<<__LINE__<<"=================";
        if (d->bluetoothSocket->state() == QBluetoothSocket::ConnectedState) {
            qWarning()<<__FILE__<<__FUNCTION__<<__LINE__<<"=================";
            d->bluetoothSocket->disconnectFromService();
        }
    } else if (d->deviceType == 1) {
        qWarning()<<__FILE__<<__FUNCTION__<<__LINE__<<"=================";
        if (d->tcpSocket->state() == QTcpSocket::ConnectedState) {
            qWarning()<<__FILE__<<__FUNCTION__<<__LINE__<<"=================";
            d->tcpSocket->disconnectFromHost();
        }
    }
}

QIODevice *MBluetoothDevice::deviceSocket()
{
    Q_D(MBluetoothDevice);
    if (d->deviceType == 0) {
        return d->bluetoothSocket;
    } else if (d->deviceType == 1) {
        return d->tcpSocket;
    }
}

void MBluetoothDevice::bluetoothSocketConnected()
{
    Q_D(MBluetoothDevice);
    qWarning()<<__FILE__<<__FUNCTION__<<__LINE__<<"XXXXXXXXXXXXXXXXXXXX QBluetoothSocket::connected";

    setIsConnected(true);
    setSocketConnectState(MBluetoothDevice::Connected);
}

void MBluetoothDevice::bluetoothSocketError(QBluetoothSocket::SocketError error)
{
    Q_D(MBluetoothDevice);
    qWarning()<<__FILE__<<__FUNCTION__<<__LINE__<<"EEEEEEEEEEEEEEEEEEEEEEE SocketError"<<error<<d->address;
    setSocketConnectState(MBluetoothDevice::Unconnect);
}

void MBluetoothDevice::bluetoothSocketDisconnected()
{
    Q_D(MBluetoothDevice);
    qWarning()<<__FILE__<<__FUNCTION__<<__LINE__<<"EEEEEEEEEEEEEEEEEEEEEEE disconnected"<<d->bluetoothSocket->state();
    setIsConnected(false);
    setSocketConnectState(MBluetoothDevice::Unconnect);
}

void MBluetoothDevice::tcpSocketConnected()
{
    Q_D(MBluetoothDevice);
    qWarning()<<__FILE__<<__FUNCTION__<<__LINE__<<"-------------------------------QTcpSocket::connected"<<d->port;
    setIsConnected(true);
    setSocketConnectState(MBluetoothDevice::Connected);
}

void MBluetoothDevice::tcpSocketError(QAbstractSocket::SocketError error)
{
    Q_D(MBluetoothDevice);
    qWarning()<<__FILE__<<__FUNCTION__<<__LINE__<<"==============================QAbstractSocket::SocketError"<<error<<d->port;
    setSocketConnectState(MBluetoothDevice::Unconnect);
}

void MBluetoothDevice::tcpSocketDisconnected()
{
    Q_D(MBluetoothDevice);
    qWarning()<<__FILE__<<__FUNCTION__<<__LINE__<<"==============================QAbstractSocket::Disconnected"<<d->port;
    setIsConnected(false);
    setSocketConnectState(MBluetoothDevice::Unconnect);
}

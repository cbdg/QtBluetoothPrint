/* ************************************
 * 蓝牙设备，方便存储已经发现的蓝牙设备，连接蓝牙，配对，发送打印命令等等
 * 2019-02-20
 * magic
 * ***********************************/
#include "mprintdevice.h"
#include "mprintdevice_p.h"
#include <QTcpSocket>
#include <QHostAddress>

/* **********************************
 * 私有数据类
 * **********************************/
MPrintDevicePrivate::MPrintDevicePrivate(MPrintDevice *parent)
{
    Q_Q(MPrintDevice);
    q_ptr = parent;

    isPaired = false;
    isConnected = false;
    isCurrent = false;
    socketConnectState = MPrintDevice::Unconnect;

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
MPrintDevice::MPrintDevice(QObject *parent)
    : QObject(parent)
    , d_ptr(new MPrintDevicePrivate(this))
{
    Q_D(MPrintDevice);

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

MPrintDevice::~MPrintDevice()
{

}

QString MPrintDevice::deviceName()
{
    Q_D(MPrintDevice);
    if (d->bluetoothDeviceInfo.isValid())
        return d->bluetoothDeviceInfo.name();
    else
        return d->deviceName;
}

void MPrintDevice::setDeviceName(const QString &name)
{
    Q_D(MPrintDevice);
    d->deviceName = name;
}

QString MPrintDevice::address()
{
    Q_D(MPrintDevice);
    if (d->bluetoothDeviceInfo.isValid())
        return d->bluetoothDeviceInfo.address().toString();
    else
        return d->address;
}

void MPrintDevice::setAddress(const QString &address)
{
    Q_D(MPrintDevice);
    d->address = address;
}

QString MPrintDevice::uuid()
{
    Q_D(MPrintDevice);
    if (d->bluetoothDeviceInfo.isValid())
        return d->bluetoothDeviceInfo.deviceUuid().toString();
    else
        return d->uuid;
}

void MPrintDevice::setUuid(const QString &uuid)
{
    Q_D(MPrintDevice);
    d->uuid = uuid;
}

bool MPrintDevice::isPaired()
{
    Q_D(MPrintDevice);
    return d->isPaired;
}

void MPrintDevice::setIsPaired(bool paired)
{
    Q_D(MPrintDevice);
    d->isPaired = paired;
    emit isPairedChanged();
}

bool MPrintDevice::isConnected()
{
    Q_D(MPrintDevice);
    return d->isConnected;
}

void MPrintDevice::setIsConnected(bool connected)
{
    Q_D(MPrintDevice);
    d->isConnected = connected;
    emit isConnectedChanged();
}

bool MPrintDevice::isCurrentDevice()
{
    Q_D(MPrintDevice);
    return d->isCurrent;
}

void MPrintDevice::setIsCurrentDevice(bool current)
{
    Q_D(MPrintDevice);
    d->isCurrent = current;
    emit isCurrentDeviceChanged();
}

void MPrintDevice::setDeviceInfo(const QBluetoothDeviceInfo &deviceInfo)
{
    Q_D(MPrintDevice);
    d->bluetoothDeviceInfo = deviceInfo;
}

void MPrintDevice::setDeviceAddress(const QBluetoothAddress &deviceAddress)
{
    Q_D(MPrintDevice);
    d->bluetoothAddress = deviceAddress;
}

void MPrintDevice::setDeviceUuid(const QBluetoothUuid &uuid)
{
    Q_D(MPrintDevice);
    d->bluetoothUuid = uuid;
}

MPrintDevice::SOCKETSTATE MPrintDevice::socketConnectState()
{
    Q_D(MPrintDevice);
    return d->socketConnectState;
}

void MPrintDevice::setSocketConnectState(MPrintDevice::SOCKETSTATE connecteState)
{
    Q_D(MPrintDevice);
    d->socketConnectState = connecteState;
    emit socketConnectStateChanged();
}

quint8 MPrintDevice::majorDeviceClass()
{
    Q_D(MPrintDevice);
    return quint8(d->bluetoothDeviceInfo.majorDeviceClass());
}

quint8 MPrintDevice::minorDeviceClass()
{
    Q_D(MPrintDevice);
    return quint8(d->bluetoothDeviceInfo.minorDeviceClass());
}

QString MPrintDevice::ipAddress()
{
    Q_D(MPrintDevice);
    return d->ipAddressStr;
}

void MPrintDevice::setIpAddress(const QString &ip)
{
    Q_D(MPrintDevice);

    if (d->ipAddressStr == ip)
        return;
    d->ipAddressStr = ip;
    emit ipAddressChanged();
}

quint16 MPrintDevice::port()
{
    Q_D(MPrintDevice);
    return d->port;
}

void MPrintDevice::setPort(int p)
{
    Q_D(MPrintDevice);

    if(d->port == quint16(p))
        return;

    d->port = quint16(p);
    emit portChanged();
}

int MPrintDevice::deviceType()
{
    Q_D(MPrintDevice);
    return d->deviceType;
}

void MPrintDevice::setDeviceType(int type)
{
    Q_D(MPrintDevice);
    if (d->deviceType == type)
        return;

    d->deviceType = type;
}

void MPrintDevice::connectSocket()
{
    Q_D(MPrintDevice);
    if (d->deviceType == 0) {
        QString remoteAddressStr = address();
        if (d->bluetoothSocket->state() == QBluetoothSocket::UnconnectedState) {
            setSocketConnectState(MPrintDevice::Connecting);
            QBluetoothUuid uuid(QString("00001101-0000-1000-8000-00805F9B34FB"));
            d->bluetoothSocket->connectToService(QBluetoothAddress(remoteAddressStr), uuid);
        } else {
            if (d->bluetoothSocket->state() == QBluetoothSocket::ConnectedState) {
                //d->bluetoothSocket->disconnectFromService();
                setIsConnected(true);
                setSocketConnectState(MPrintDevice::Connected);
            }
        }
    } else if (d->deviceType == 1) {
        if (d->tcpSocket->state() == QTcpSocket::UnconnectedState) {
            setSocketConnectState(MPrintDevice::Connecting);
            d->tcpSocket->connectToHost(QHostAddress(d->ipAddressStr), d->port);
        } else {
            if (d->tcpSocket->state() == QTcpSocket::ConnectedState) {
                setIsConnected(true);
                setSocketConnectState(MPrintDevice::Connected);
            }
        }
    }
}

void MPrintDevice::disconnectSocket()
{
    Q_D(MPrintDevice);
    if (d->deviceType == 0) {
        if (d->bluetoothSocket->state() == QBluetoothSocket::ConnectedState) {
            d->bluetoothSocket->disconnectFromService();
        }
    } else if (d->deviceType == 1) {
        if (d->tcpSocket->state() == QTcpSocket::ConnectedState) {
            d->tcpSocket->disconnectFromHost();
        }
    }
}

QIODevice *MPrintDevice::deviceSocket()
{
    Q_D(MPrintDevice);
    if (d->deviceType == 0) {
        return d->bluetoothSocket;
    } else if (d->deviceType == 1) {
        return d->tcpSocket;
    }
}

void MPrintDevice::bluetoothSocketConnected()
{
    Q_D(MPrintDevice);
    qWarning()<<__FILE__<<__FUNCTION__<<__LINE__<<"XXXXXXXXXXXXXXXXXXXX QBluetoothSocket::connected";

    setIsConnected(true);
    setSocketConnectState(MPrintDevice::Connected);
}

void MPrintDevice::bluetoothSocketError(QBluetoothSocket::SocketError error)
{
    Q_D(MPrintDevice);
    qWarning()<<__FILE__<<__FUNCTION__<<__LINE__<<"EEEEEEEEEEEEEEEEEEEEEEE SocketError"<<error<<d->address;
    setSocketConnectState(MPrintDevice::Unconnect);
}

void MPrintDevice::bluetoothSocketDisconnected()
{
    Q_D(MPrintDevice);
    qWarning()<<__FILE__<<__FUNCTION__<<__LINE__<<"EEEEEEEEEEEEEEEEEEEEEEE disconnected"<<d->bluetoothSocket->state();
    setIsConnected(false);
    setSocketConnectState(MPrintDevice::Unconnect);
}

void MPrintDevice::tcpSocketConnected()
{
    Q_D(MPrintDevice);
    qWarning()<<__FILE__<<__FUNCTION__<<__LINE__<<"-------------------------------QTcpSocket::connected"<<d->port;
    setIsConnected(true);
    setSocketConnectState(MPrintDevice::Connected);
}

void MPrintDevice::tcpSocketError(QAbstractSocket::SocketError error)
{
    Q_D(MPrintDevice);
    qWarning()<<__FILE__<<__FUNCTION__<<__LINE__<<"==============================QAbstractSocket::SocketError"<<error<<d->ipAddressStr<<d->port;
    setSocketConnectState(MPrintDevice::Unconnect);
}

void MPrintDevice::tcpSocketDisconnected()
{
    Q_D(MPrintDevice);
    qWarning()<<__FILE__<<__FUNCTION__<<__LINE__<<"==============================QAbstractSocket::Disconnected"<<d->ipAddressStr<<d->port;
    setIsConnected(false);
    setSocketConnectState(MPrintDevice::Unconnect);
}

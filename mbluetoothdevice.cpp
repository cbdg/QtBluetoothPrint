/* ************************************
 * 蓝牙设备，方便存储已经发现的蓝牙设备，连接蓝牙，配对，发送打印命令等等
 * 2019-02-20
 * magic
 * ***********************************/
#include "mbluetoothdevice.h"
#include "mbluetoothdevice_p.h"

/* **********************************
 * 私有数据类
 * **********************************/
MBluetoothDevicePrivate::MBluetoothDevicePrivate(MBluetoothDevice *parent)
{
    Q_Q(MBluetoothDevice);
    q_ptr = parent;

    isPaired = false;
    isConnected = false;
    socketConnectState = MBluetoothDevice::Unconnect;
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

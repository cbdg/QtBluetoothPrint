#ifndef MBLUETOOTHDEVICE_P_H
#define MBLUETOOTHDEVICE_P_H

#include "mbluetoothdevice.h"
#include <QBluetoothAddress>
#include <QBluetoothUuid>
#include <QBluetoothDeviceInfo>

class QBluetoothSocket;
class QTcpSocket;
class MBluetoothDevicePrivate
{
public:
    MBluetoothDevicePrivate(MBluetoothDevice *parent);
    MBluetoothDevice *q_ptr;

    QBluetoothAddress bluetoothAddress;
    QBluetoothUuid bluetoothUuid;
    QBluetoothDeviceInfo bluetoothDeviceInfo;
    QString deviceName;
    QString address;
    QString uuid;

    QString ipAddressStr;
    qint16 port;

    bool isPaired;
    bool isConnected;
    bool isCurrent;

    int deviceType; // 0 bluetooth; 1 wifi

    MBluetoothDevice::SOCKETSTATE socketConnectState;

    QBluetoothSocket *bluetoothSocket;
    QTcpSocket *tcpSocket;

private:
    Q_DECLARE_PUBLIC(MBluetoothDevice)
};

#endif // MBLUETOOTHDEVICE_P_H

#ifndef MPRINTHDEVICE_P_H
#define MPRINTHDEVICE_P_H

#include "mprintdevice.h"
#include <QBluetoothAddress>
#include <QBluetoothUuid>
#include <QBluetoothDeviceInfo>

class QBluetoothSocket;
class QTcpSocket;
class MPrintDevicePrivate
{
public:
    MPrintDevicePrivate(MPrintDevice *parent);
    MPrintDevice *q_ptr;

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

    MPrintDevice::SOCKETSTATE socketConnectState;

    QBluetoothSocket *bluetoothSocket;
    QTcpSocket *tcpSocket;

private:
    Q_DECLARE_PUBLIC(MPrintDevice)
};

#endif // MPRINTHDEVICE_P_H

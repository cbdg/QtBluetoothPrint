#ifndef MBLUETOOTHDEVICE_P_H
#define MBLUETOOTHDEVICE_P_H

#include "mbluetoothdevice.h"
#include <QBluetoothAddress>
#include <QBluetoothUuid>
#include <QBluetoothDeviceInfo>

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

    bool isPaired;
    bool isConnected;

    MBluetoothDevice::SOCKETSTATE socketConnectState;

private:
    Q_DECLARE_PUBLIC(MBluetoothDevice)
};

#endif // MBLUETOOTHDEVICE_P_H

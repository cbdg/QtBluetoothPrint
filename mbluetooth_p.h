#ifndef MBLUETOOTH_P_H
#define MBLUETOOTH_P_H

#include "mbluetooth.h"
#include <QBluetoothLocalDevice>

class QBluetoothDeviceDiscoveryAgent;
class QBluetoothTransferManager;
class QBluetoothSocket;
class MBluetoothPrivate
{
public:
    MBluetoothPrivate (MBluetooth *parent);

    MBluetooth *q_ptr;
    QBluetoothLocalDevice locaDevice;
    QBluetoothDeviceDiscoveryAgent *discoveryAgent;
    QBluetoothSocket *bluetoothSocket;
    QStringList discoveredDevice;
    QStringList pairedDevice;
    QBluetoothTransferManager *transferManager;
private:
    Q_DECLARE_PUBLIC(MBluetooth)
};

#endif // MBLUETOOTH_P_H

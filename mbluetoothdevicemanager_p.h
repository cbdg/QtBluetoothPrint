#ifndef MBLUETOOTHDEVICEMANAGER_P_H
#define MBLUETOOTHDEVICEMANAGER_P_H

#include "mbluetoothdevicemanager.h"
#include <QBluetoothLocalDevice>
#include "mbluetoothdevice.h"
#include "mlistmodel.h"

class QBluetoothDeviceDiscoveryAgent;
class QBluetoothSocket;
class MBluetoothPrintCommand;
class MBluetoothDeviceManagerPrivate
{
public:
    MBluetoothDeviceManagerPrivate(MBluetoothDeviceManager *parent);

    MBluetoothDeviceManager *q_ptr;

    QBluetoothLocalDevice *localDevice;
    QBluetoothDeviceDiscoveryAgent *discoveryAgent;
    QBluetoothSocket *bluetoothSocket;
    MBluetoothPrintCommand *bluetoothPrintCommand;

    QList<MBluetoothDevice*> *discoveredDevices;
    QList<MBluetoothDevice*> *pairedDevices;

    MListModel *discoveredDeviceModel;
    MListModel *pairedDeviceModel;

    MBluetoothDevice *selectPrint;

private:
    Q_DECLARE_PUBLIC(MBluetoothDeviceManager)

};

#endif // MBLUETOOTHDEVICEMANAGER_P_H

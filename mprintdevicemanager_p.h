#ifndef MPRINTDEVICEMANAGER_P_H
#define MPRINTDEVICEMANAGER_P_H

#include "mprintdevicemanager.h"
#include <QBluetoothLocalDevice>
#include "mprintdevice.h"
#include "mlistmodel.h"

class QBluetoothDeviceDiscoveryAgent;
class QBluetoothSocket;
class MPrintCommand;
class MPrintDeviceManagerPrivate
{
public:
    MPrintDeviceManagerPrivate(MPrintDeviceManager *parent);

    MPrintDeviceManager *q_ptr;

    QBluetoothLocalDevice *localDevice;
    QBluetoothDeviceDiscoveryAgent *discoveryAgent;
    QBluetoothSocket *bluetoothSocket;
    MPrintCommand *bluetoothPrintCommand;

    QList<MPrintDevice*> *discoveredDevices;
    QList<MPrintDevice*> *pairedDevices;

    MListModel *discoveredDeviceModel;
    MListModel *pairedDeviceModel;

    MPrintDevice *selectPrint;
    MPrintDevice *wifiPrint;
    MPrintDevice *bluetoothPrint;

    bool isWaitingConnectNewDevice;

    int printType;// 1 bluetooth; 2 wifi; 0 unknow;
    int receiptsType; // 0 32英文/行; 1 40英文/行; 2 48英文/行

    QString lastBluetoothName;
    QString lastBluetoothAddress;
    QString lastBluetoothUuid;

    QString lastWifiIpAddress;
    quint16 lastWifiPor;

private:
    Q_DECLARE_PUBLIC(MPrintDeviceManager)

};

#endif // MPRINTDEVICEMANAGER_P_H

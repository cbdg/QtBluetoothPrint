#ifndef MBLUETOOTHDEVICEMANAGER_P_H
#define MBLUETOOTHDEVICEMANAGER_P_H

#include "mbluetoothdevicemanager.h"
#include <QBluetoothLocalDevice>
#include "mbluetoothdevice.h"
#include "mlistmodel.h"

class QBluetoothDeviceDiscoveryAgent;
class QBluetoothSocket;
class MBluetoothPrintCommand;
class QTcpSocket;
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

    QTcpSocket *tcpSocket;

    bool isWaitingConnectNewDevice;

    int printType;// 1 bluetooth; 2 wifi; 0 unknow;
    int receiptsType; // 0 32英文/行; 1 40英文/行; 2 48英文/行

    QString lastBluetoothName;
    QString lastBluetoothAddress;
    QString lastBluetoothUuid;

    QString lastWifiIpAddress;
    quint16 lastWifiPor;

private:
    Q_DECLARE_PUBLIC(MBluetoothDeviceManager)

};

#endif // MBLUETOOTHDEVICEMANAGER_P_H

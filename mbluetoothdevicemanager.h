#ifndef MBLUETOOTHDEVICEMANAGER_H
#define MBLUETOOTHDEVICEMANAGER_H

#include <QObject>
#include <QBluetoothLocalDevice>
#include <QBluetoothDeviceInfo>
#include "mbluetoothdevice.h"
#include "mlistmodel.h"

class MBluetoothDeviceManagerPrivate;
class MBluetoothDeviceManager : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString localDevice READ localDevice CONSTANT)
    Q_PROPERTY(int localDeviceStatus READ localDeviceStatus NOTIFY localDeviceStatusChanged)
    Q_PROPERTY(bool discoverIsActive READ discoverIsActive NOTIFY discoverIsActiveChanged)
//    Q_PROPERTY(MBluetoothDeviceList remoteDevice READ remoteDevice NOTIFY remoteDeviceChanged)
//    Q_PROPERTY(MBluetoothDeviceList pairedDevice READ pairedDevice NOTIFY pairedDeviceChanged)

public:
    MBluetoothDeviceManager(QObject *parent = nullptr);
    ~MBluetoothDeviceManager();

    QString localDevice();
    int localDeviceStatus();
    bool discoverIsActive();
//    MBluetoothDeviceList remoteDevice();
//    MBluetoothDeviceList pairedDevice();

    Q_INVOKABLE void switchLocalDeviceHostmodel();
    Q_INVOKABLE void startDiscoverDevice();
    Q_INVOKABLE void pairingToRemote(MBluetoothDevice *device);
    Q_INVOKABLE void connectPrinterWithSocket(MBluetoothDevice *device);
    Q_INVOKABLE void printTest();
    Q_INVOKABLE void printTakeoutOrder(const QJsonObject &jsonObj);

    Q_INVOKABLE MListModel *remoteDevice();
    Q_INVOKABLE MListModel *pairedDevice();

    Q_INVOKABLE void setPrintDevice(MBluetoothDevice *device);

signals:
    void localDeviceStatusChanged();
    void discoverIsActiveChanged();
    void remoteDeviceChanged();
    void pairedDeviceChanged();

public slots:
    void newDeviceDiscovered(const QBluetoothDeviceInfo &deviceInfo);
    void devicePaired(const QBluetoothAddress &address, QBluetoothLocalDevice::Pairing pairing);

private:
    int retrieveDevice(QList<MBluetoothDevice*> *deviceList, const QBluetoothDeviceInfo &deviceInfo);
    int retrieveDeviceWithAddress(QList<MBluetoothDevice*> *deviceList, const QString &address);
    void readSelectDevice();
    void writeSelectDevice();

private:
    Q_DECLARE_PRIVATE(MBluetoothDeviceManager)
    Q_DISABLE_COPY(MBluetoothDeviceManager)
    QScopedPointer<MBluetoothDeviceManagerPrivate> d_ptr;
};

#endif // MBLUETOOTHDEVICEMANAGER_H

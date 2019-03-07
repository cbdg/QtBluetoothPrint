#ifndef MBLUETOOTHDEVICEMANAGER_H
#define MBLUETOOTHDEVICEMANAGER_H

#include <QObject>
#include <QBluetoothLocalDevice>
#include <QBluetoothDeviceInfo>
#include <QAbstractSocket>
#include "mbluetoothdevice.h"
#include "mlistmodel.h"

class MBluetoothDeviceManagerPrivate;
class MBluetoothDeviceManager : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString localDevice READ localDevice CONSTANT)
    Q_PROPERTY(int localDeviceStatus READ localDeviceStatus NOTIFY localDeviceStatusChanged)
    Q_PROPERTY(bool discoverIsActive READ discoverIsActive NOTIFY discoverIsActiveChanged)
    Q_PROPERTY(int printType READ printType WRITE setPrintType NOTIFY printTypeChanged)
    Q_PROPERTY(int receiptsType READ receiptsType WRITE setReceiptsType NOTIFY receiptsTypeChanged)
    Q_PROPERTY(quint16 port READ port WRITE setPort NOTIFY portChanged)
    Q_PROPERTY(QString ipAddress READ ipAddress WRITE setIpAddress NOTIFY ipAddressChanged)
    Q_PROPERTY(QString bluetoothName READ bluetoothName WRITE setBluetoothName NOTIFY bluetoothNameChanged)

//    Q_PROPERTY(MBluetoothDeviceList remoteDevice READ remoteDevice NOTIFY remoteDeviceChanged)
//    Q_PROPERTY(MBluetoothDeviceList pairedDevice READ pairedDevice NOTIFY pairedDeviceChanged)

public:
    MBluetoothDeviceManager(QObject *parent = nullptr);
    ~MBluetoothDeviceManager();

    QString localDevice();
    int localDeviceStatus();
    bool discoverIsActive();

    int printType();
    void setPrintType(int type);

    int receiptsType();
    void setReceiptsType(int type);

    int port();
    void setPort(int p);

    QString ipAddress();
    void setIpAddress(const QString &ip);

    QString bluetoothName();
    void setBluetoothName(const QString &bleName);

    Q_INVOKABLE void switchLocalDeviceHostmodel();
    Q_INVOKABLE void startDiscoverDevice();
    Q_INVOKABLE void stopDiscoverDevice();
    Q_INVOKABLE void pairingToRemote(MBluetoothDevice *device);
    Q_INVOKABLE void connectPrinterWithSocket(MBluetoothDevice *device);
    Q_INVOKABLE void printTest();
    Q_INVOKABLE void printTakeoutOrder(const QJsonObject &jsonObj);

    Q_INVOKABLE void wifiPrint();

    Q_INVOKABLE MListModel *remoteDevice();
    Q_INVOKABLE MListModel *pairedDevice();

    Q_INVOKABLE void setPrintDevice(MBluetoothDevice *device);

signals:
    void localDeviceStatusChanged();
    void discoverIsActiveChanged();
    void remoteDeviceChanged();
    void pairedDeviceChanged();
    void printTypeChanged();
    void receiptsTypeChanged();
    void portChanged();
    void ipAddressChanged();
    void bluetoothNameChanged();

public slots:
    void newDeviceDiscovered(const QBluetoothDeviceInfo &deviceInfo);
    void devicePaired(const QBluetoothAddress &address, QBluetoothLocalDevice::Pairing pairing);
    void tcpSocketError(QAbstractSocket::SocketError &error);

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

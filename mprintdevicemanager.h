#ifndef MPRINTDEVICEMANAGER_H
#define MPRINTDEVICEMANAGER_H

#include <QObject>
#include <QBluetoothLocalDevice>
#include <QBluetoothDeviceInfo>
#include <QAbstractSocket>
#include "mprintdevice.h"
#include "mlistmodel.h"

class MPrintDeviceManagerPrivate;
class MPrintDeviceManager : public QObject
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
    Q_PROPERTY(MPrintDevice* currentDevice READ currentDevice WRITE setCurrentDevice NOTIFY currentDeviceChanged)
    Q_PROPERTY(MPrintDevice* wifiDevice READ wifiDevice WRITE setWifiDevice NOTIFY wifiDeviceChanged)
    Q_PROPERTY(MPrintDevice* bluetoothDevice READ bluetoothDevice WRITE setBluetoothDevice NOTIFY bluetoothDeviceChanged)

public:
    MPrintDeviceManager(QObject *parent = nullptr);
    ~MPrintDeviceManager();

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

    MPrintDevice *currentDevice();
    void setCurrentDevice(MPrintDevice *device);

    MPrintDevice *wifiDevice();
    void setWifiDevice(MPrintDevice *device);

    MPrintDevice *bluetoothDevice();
    void setBluetoothDevice(MPrintDevice *device);

    Q_INVOKABLE void switchLocalDeviceHostmodel();
    Q_INVOKABLE void startDiscoverDevice();
    Q_INVOKABLE void stopDiscoverDevice();
    Q_INVOKABLE void pairingToRemote(MPrintDevice *device);
    Q_INVOKABLE void connectPrinterWithSocket(MPrintDevice *device);
    Q_INVOKABLE void printTest();
    Q_INVOKABLE void printTakeoutOrder(const QJsonObject &jsonObj);
    Q_INVOKABLE void resetBluetoothPrint();

    Q_INVOKABLE MListModel *remoteDevice();
    Q_INVOKABLE MListModel *pairedDevice();

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
    void currentDeviceChanged();
    void wifiDeviceChanged();
    void bluetoothDeviceChanged();

public slots:
    void newDeviceDiscovered(const QBluetoothDeviceInfo &deviceInfo);
    void devicePaired(const QBluetoothAddress &address, QBluetoothLocalDevice::Pairing pairing);

private:
    int retrieveDevice(QList<MPrintDevice*> *deviceList, const QBluetoothDeviceInfo &deviceInfo);
    int retrieveDeviceWithAddress(QList<MPrintDevice*> *deviceList, const QString &address);
    void readSelectDevice();
    void writeSelectDevice();
    void createPrintDevice();

private:
    Q_DECLARE_PRIVATE(MPrintDeviceManager)
    Q_DISABLE_COPY(MPrintDeviceManager)
    QScopedPointer<MPrintDeviceManagerPrivate> d_ptr;
};

#endif // MPRINTDEVICEMANAGER_H

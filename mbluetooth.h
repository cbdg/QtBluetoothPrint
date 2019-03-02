#ifndef MBLUETOOTH_H
#define MBLUETOOTH_H

#include <QObject>
#include <QBluetoothDeviceInfo>
#include <QBluetoothTransferReply>

class MBluetoothPrivate;
class MBluetooth : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString localDevice READ localDevice NOTIFY localDeviceChanged)
    Q_PROPERTY(int localDeviceStatus READ localDeviceStatus NOTIFY localDeviceStatusChanged)
    Q_PROPERTY(QStringList remoteDevice READ remoteDevice NOTIFY remoteDeviceChanged)
    Q_PROPERTY(QStringList pairedDevice READ pairedDevice NOTIFY pairedDeviceChanged)
    Q_PROPERTY(bool discoverIsActive READ discoverIsActive NOTIFY discoverIsActiveChanged)
public:
    MBluetooth(QObject *parent = Q_NULLPTR);
    ~MBluetooth(); // Q_D private 必需要有析构函数

    QString localDevice();
    int localDeviceStatus();
    QStringList remoteDevice();
    QStringList pairedDevice();
    bool discoverIsActive();


    Q_INVOKABLE QStringList connectDevice();

    Q_INVOKABLE void switchLocalDeviceHostmodel();
    Q_INVOKABLE void startDiscoverDevice();
    Q_INVOKABLE void sendFileToRemote(const QString &address);
    Q_INVOKABLE void pairingToRemote(const QString &address);
    Q_INVOKABLE void connectPrinter(const QString &address);

public slots:
    void deviceDiscovered(const QBluetoothDeviceInfo &device);
    void transferFinished(QBluetoothTransferReply *reply);
    void transferError(QBluetoothTransferReply::TransferError errorType);

signals:
    void localDeviceChanged();
    void localDeviceStatusChanged();
    void remoteDeviceChanged();
    void pairedDeviceChanged();
    void discoverIsActiveChanged();

private:
    Q_DECLARE_PRIVATE(MBluetooth)
    Q_DISABLE_COPY(MBluetooth)
    QScopedPointer<MBluetoothPrivate> d_ptr;
};

#endif // MBLUETOOTH_H

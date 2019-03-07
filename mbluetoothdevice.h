#ifndef MBLUETOOTHDEVICE_H
#define MBLUETOOTHDEVICE_H

#include <QObject>
#include <QBluetoothDeviceInfo>

class MBluetoothDevicePrivate;
class MBluetoothDevice : public QObject
{
    Q_OBJECT
    Q_ENUMS(SOCKETSTATE)
    Q_PROPERTY(QString deviceName READ deviceName WRITE setDeviceName NOTIFY deviceNameChanged)
    Q_PROPERTY(QString address READ address WRITE setAddress NOTIFY addressChanged)
    Q_PROPERTY(QString uuid READ uuid WRITE setUuid NOTIFY uuidChanged)
    Q_PROPERTY(bool isPaired READ isPaired WRITE setIsPaired NOTIFY isPairedChanged)
    Q_PROPERTY(bool isConnected READ isConnected WRITE setIsConnected NOTIFY isConnectedChanged)
    Q_PROPERTY(SOCKETSTATE socketConnectState READ socketConnectState WRITE setSocketConnectState NOTIFY socketConnectStateChanged)
    Q_PROPERTY(quint8 majorDeviceClass READ majorDeviceClass CONSTANT)
    Q_PROPERTY(quint8 minorDeviceClass READ minorDeviceClass CONSTANT)

    Q_PROPERTY(QString ipAddress READ ipAddress WRITE setIpAddress NOTIFY ipAddressChanged)
    Q_PROPERTY(quint16 port READ port WRITE setPort NOTIFY portChanged)
    Q_PROPERTY(int deviceType READ deviceType WRITE setDeviceType NOTIFY deviceTypeChanged)
public:

    enum SOCKETSTATE {
        Unconnect,
        Connected,
        Connecting,
        Unknow
    };

    explicit MBluetoothDevice(QObject *parent = nullptr);
    // Q_D private 必需要有析构函数
    // qscopedpointer.h:57: error: invalid application of 'sizeof' to incomplete type 'MBluetoothDevicePrivate'
    // typedef char IsIncompleteType[ sizeof(T) ? 1 : -1 ];
    ~MBluetoothDevice();

    QString deviceName();
    void setDeviceName(const QString &name);

    QString address();
    void setAddress(const QString &address);

    QString uuid();
    void setUuid(const QString &uuid);

    bool isPaired();
    void setIsPaired(bool paired);

    bool isConnected();
    void setIsConnected(bool connected);

    void setDeviceInfo(const QBluetoothDeviceInfo &deviceInfo);
    void setDeviceAddress(const QBluetoothAddress &deviceAddress);
    void setDeviceUuid(const QBluetoothUuid &uuid);

    SOCKETSTATE socketConnectState();
    void setSocketConnectState(SOCKETSTATE connecteState);

    quint8 majorDeviceClass();
    quint8 minorDeviceClass();

    QString ipAddress();
    void setIpAddress(const QString &ip);

    quint16 port();
    void setPort(int p);

    int deviceType();
    void setDeviceType(int type);

signals:
    void deviceNameChanged();
    void addressChanged();// 没有任何作用，只是为了消除 qml 烦人的警告
    void uuidChanged();// 没有任何作用，只是为了消除 qml 烦人的警告
    void isPairedChanged();
    void isConnectedChanged();
    void socketConnectStateChanged();
    void ipAddressChanged();
    void portChanged();
    void deviceTypeChanged();

public slots:

private:
    Q_DECLARE_PRIVATE(MBluetoothDevice)
    Q_DISABLE_COPY(MBluetoothDevice)
    QScopedPointer<MBluetoothDevicePrivate> d_ptr;
};

typedef QList<MBluetoothDevice*> MBluetoothDeviceList;

#endif // MBLUETOOTHDEVICE_H

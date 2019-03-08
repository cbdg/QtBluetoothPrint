#ifndef MPRINTDEVICE_H
#define MPRINTDEVICE_H

#include <QObject>
#include <QBluetoothDeviceInfo>
#include <QBluetoothSocket>

class MPrintDevicePrivate;
class MPrintDevice : public QObject
{
    Q_OBJECT
    Q_ENUMS(SOCKETSTATE)
    Q_PROPERTY(QString deviceName READ deviceName WRITE setDeviceName NOTIFY deviceNameChanged)
    Q_PROPERTY(QString address READ address WRITE setAddress NOTIFY addressChanged)
    Q_PROPERTY(QString uuid READ uuid WRITE setUuid NOTIFY uuidChanged)
    Q_PROPERTY(bool isPaired READ isPaired WRITE setIsPaired NOTIFY isPairedChanged)
    Q_PROPERTY(bool isConnected READ isConnected WRITE setIsConnected NOTIFY isConnectedChanged)
    Q_PROPERTY(bool isCurrentDevice READ isCurrentDevice WRITE setIsCurrentDevice NOTIFY isCurrentDeviceChanged)
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

    explicit MPrintDevice(QObject *parent = nullptr);
    // Q_D private 必需要有析构函数
    // qscopedpointer.h:57: error: invalid application of 'sizeof' to incomplete type 'MPrintDevicePrivate'
    // typedef char IsIncompleteType[ sizeof(T) ? 1 : -1 ];
    ~MPrintDevice();

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

    bool isCurrentDevice();
    void setIsCurrentDevice(bool current);

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

    void connectSocket();
    void disconnectSocket();

    QIODevice* deviceSocket();

signals:
    void deviceNameChanged();
    void addressChanged();// 没有任何作用，只是为了消除 qml 烦人的警告
    void uuidChanged();// 没有任何作用，只是为了消除 qml 烦人的警告
    void isPairedChanged();
    void isConnectedChanged();
    void isCurrentDeviceChanged();
    void socketConnectStateChanged();
    void ipAddressChanged();
    void portChanged();
    void deviceTypeChanged();

public slots:
    void bluetoothSocketConnected();
    void bluetoothSocketError(QBluetoothSocket::SocketError error);
    void bluetoothSocketDisconnected();

    void tcpSocketConnected();
    void tcpSocketError(QAbstractSocket::SocketError error);
    void tcpSocketDisconnected();

private:
    Q_DECLARE_PRIVATE(MPrintDevice)
    Q_DISABLE_COPY(MPrintDevice)
    QScopedPointer<MPrintDevicePrivate> d_ptr;
};

typedef QList<MPrintDevice*> MPrintDeviceList;

#endif // MPRINTDEVICE_H

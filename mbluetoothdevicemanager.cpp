/* ***************************************
 * 蓝牙设备管理类，实现蓝牙设备搜索，配对，连接等功能
 * 2019-02-20
 * magic
 * *************************************/

#include "mbluetoothdevicemanager.h"
#include "mbluetoothdevicemanager_p.h"
#include <QbluetoothSocket>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QTextCodec>
#include <QSettings>
#include <QDir>
#include <QTcpSocket>
#include <QHostAddress>
#include "mbluetoothprintcommand.h"
#include "morderprinter.h"

/* ************************************
 * 私有数据类
 * ************************************/
MBluetoothDeviceManagerPrivate::MBluetoothDeviceManagerPrivate(MBluetoothDeviceManager *parent)
{
    Q_Q(MBluetoothDeviceManager);
    q_ptr = parent;
    localDevice = nullptr;
    discoveryAgent = nullptr;
    bluetoothSocket = nullptr;
    discoveredDevices = nullptr;
    pairedDevices = nullptr;
    pairedDeviceModel = nullptr;
    discoveredDeviceModel = nullptr;
    bluetoothPrintCommand = nullptr;
    selectPrint = nullptr;
    isWaitingConnectNewDevice = false;

    // wifi
    tcpSocket = nullptr;

    printType = 1;
    receiptsType = 0;

    lastBluetoothName = "";
    lastBluetoothAddress = "";
    lastBluetoothUuid = "";

    lastWifiIpAddress = "";
    lastWifiPor = 8266; // 目前通过遍历 恩叶 NP58-W/NP58-WB 票据 + 标签 两用打印机 端口号为： 8266 可用
}


/* ****************************************
 * MBluetoothDeviceManager 接口实现
 * ****************************************/
MBluetoothDeviceManager::MBluetoothDeviceManager(QObject *parent)
    : QObject(parent)
    , d_ptr(new MBluetoothDeviceManagerPrivate(this))
{
    Q_D(MBluetoothDeviceManager);

    d->localDevice = new QBluetoothLocalDevice(this);
    d->discoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);
    d->bluetoothSocket = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol,this);
    d->bluetoothPrintCommand = new MBluetoothPrintCommand(d->bluetoothSocket, this);

    // 本地蓝牙状态更改通知
    connect(d->localDevice, &QBluetoothLocalDevice::hostModeStateChanged
            , [=](){emit localDeviceStatusChanged();});

    // 扫描附近可用蓝牙设备通知
    connect(d->discoveryAgent, &QBluetoothDeviceDiscoveryAgent::finished
            , [=](){emit discoverIsActiveChanged();});
    connect(d->discoveryAgent, SIGNAL(deviceDiscovered(QBluetoothDeviceInfo))
            ,this, SLOT(newDeviceDiscovered(QBluetoothDeviceInfo)));

    // pairing device
    connect(d->localDevice, SIGNAL(pairingFinished(QBluetoothAddress,QBluetoothLocalDevice::Pairing))
            , this ,SLOT(devicePaired(QBluetoothAddress,QBluetoothLocalDevice::Pairing)));
    //    connect(d->localDevice, &QBluetoothLocalDevice::pairingFinished, [=](const QBluetoothAddress &address, QBluetoothLocalDevice::Pairing pairing){
    //        qWarning()<<__FILE__<<__FUNCTION__<<__LINE__<<"00000000000000000000 pairing finished"<<address<<pairing;
    //        if (d->localDevice.pairingStatus(device.address()) == QBluetoothLocalDevice::Unpaired) {
    //            if (!d->discoveredDevice.contains(newDevice)){
    //                d->discoveredDevice << QString("%1(%2)").arg(device.name()).arg(device.address().toString());
    //                emit remoteDeviceChanged();
    //            }
    //        } else {
    //            if (!d->pairedDevice.contains(newDevice)){
    //                d->pairedDevice << QString("%1(%2)").arg(device.name()).arg(device.address().toString());
    //                emit pairedDeviceChanged();
    //            }
    //        }
    //    });
    connect(d->localDevice, &QBluetoothLocalDevice::pairingDisplayConfirmation, [=](const QBluetoothAddress &address, QString pin){
        //qWarning()<<__FILE__<<__FUNCTION__<<__LINE__<<"-------------------- pairingDisplayConfirmation"<<address<<pin;
    });
    connect(d->localDevice, &QBluetoothLocalDevice::pairingDisplayPinCode, [=](const QBluetoothAddress &address, QString pin){
        //qWarning()<<__FILE__<<__FUNCTION__<<__LINE__<<"XXXXXXXXXXXXXXXXXXXX pairingDisplayPinCode"<<address<<pin;
    });


    // socekt connect
    connect(d->bluetoothSocket, &QBluetoothSocket::connected, [=](){
        qWarning()<<__FILE__<<__FUNCTION__<<__LINE__<<"XXXXXXXXXXXXXXXXXXXX QBluetoothSocket::connected";
        d->bluetoothPrintCommand->setBluetoothSocket(d->bluetoothSocket);
        emit bluetoothNameChanged();
        if (d->selectPrint) {
            d->selectPrint->setIsConnected(true);
            d->selectPrint->setSocketConnectState(MBluetoothDevice::Connected);
        }
    });
    connect(d->bluetoothSocket, static_cast<void (QBluetoothSocket::*)(QBluetoothSocket::SocketError)>(&QBluetoothSocket::error)
            , [=](QBluetoothSocket::SocketError error){
        qWarning()<<__FILE__<<__FUNCTION__<<__LINE__<<"EEEEEEEEEEEEEEEEEEEEEEE SocketError"<<error;
    });
    connect(d->bluetoothSocket, &QBluetoothSocket::disconnected, [=](){
        qWarning()<<__FILE__<<__FUNCTION__<<__LINE__<<"EEEEEEEEEEEEEEEEEEEEEEE disconnected"<<d->bluetoothSocket->state();
        if (d->isWaitingConnectNewDevice) {


            qWarning()<<__FILE__<<__FUNCTION__<<__LINE__<<"======================="<<d->bluetoothSocket->state();
            if (d->selectPrint) {
                QString remoteAddressStr = d->selectPrint->address();
                QBluetoothUuid uuid(QString("00001101-0000-1000-8000-00805F9B34FB"));
                //        QBluetoothUuid uuid(QString("8401d6f1-2ce5-84af-3adc-06409e62d896"));
                d->bluetoothSocket->connectToService(QBluetoothAddress(remoteAddressStr), uuid);
            }
        }
    });

    // 附近可用蓝牙列表初始化
    d->discoveredDeviceModel = new MListModel(this);
    d->discoveredDevices = new MBluetoothDeviceList();
    d->discoveredDeviceModel->setDataList(d->discoveredDevices);
    d->discoveredDeviceModel->resetAll();

    d->pairedDeviceModel = new MListModel(this);
    d->pairedDevices = new MBluetoothDeviceList();
    d->pairedDeviceModel->setDataList(d->discoveredDevices);
    d->pairedDeviceModel->resetAll();

    readSelectDevice();
    if (d->selectPrint) {
        connectPrinterWithSocket(d->selectPrint);
    }

    d->tcpSocket = new QTcpSocket(this);
    connect(d->tcpSocket, &QTcpSocket::connected, [=](){
        qWarning()<<__FILE__<<__FUNCTION__<<__LINE__<<"-------------------------------QTcpSocket::connected"<<d->lastWifiPor;
    });
    //    connect(d->tcpSocket, static_cast<void(QAbstractSocket::*)(QAbstractSocket::SocketError)>(&QAbstractSocket::error),
    //            [=](QAbstractSocket::SocketError socketError){
    //        qWarning()<<__FILE__<<__FUNCTION__<<__LINE__<<"==============================QAbstractSocket::SocketError"<<socketError<<d->lastWifiPor;
    //        d->lastWifiPor++;
    //        if (d->lastWifiPor < 9100) //65535
    //            d->tcpSocket->connectToHost(QHostAddress("192.168.31.157"), d->lastWifiPor);
    //    });
    connect(d->tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(tcpSocketError(QAbstractSocket::SocketError)));
}

MBluetoothDeviceManager::~MBluetoothDeviceManager()
{
    Q_D(MBluetoothDeviceManager);

    if (d->localDevice) d->localDevice->deleteLater();

    if (d->discoveryAgent) d->discoveryAgent->deleteLater();

    if (d->bluetoothSocket) d->bluetoothSocket->deleteLater();

    if (d->bluetoothPrintCommand) d->bluetoothPrintCommand->deleteLater();

    while (!d->pairedDevices->isEmpty()) {
        d->pairedDevices->takeLast()->deleteLater();
    }
    delete d->pairedDevices;

    while (!d->discoveredDevices->isEmpty()) {
        d->discoveredDevices->takeLast()->deleteLater();
    }
    delete d->discoveredDevices;

    if (d->pairedDeviceModel) d->pairedDeviceModel->deleteLater();
    if (d->discoveredDeviceModel) d->discoveredDeviceModel->deleteLater();
}

QString MBluetoothDeviceManager::localDevice()
{
    Q_D(MBluetoothDeviceManager);
    QString deviceName;
    if (d->localDevice)
    {
        deviceName = d->localDevice->name();
    }
    return deviceName;
}

int MBluetoothDeviceManager::localDeviceStatus()
{
    Q_D(MBluetoothDeviceManager);
    if(d->localDevice) {
        return d->localDevice->hostMode();
    }
    return 0;
}

bool MBluetoothDeviceManager::discoverIsActive()
{
    Q_D(MBluetoothDeviceManager);
    if (d->discoveryAgent) {
        return d->discoveryAgent->isActive();
    }
    return false;
}

int MBluetoothDeviceManager::printType()
{
    Q_D(MBluetoothDeviceManager);
    return d->printType;
}

void MBluetoothDeviceManager::setPrintType(int type)
{
    Q_D(MBluetoothDeviceManager);
    if (d->printType == type)
        return;

    d->printType = type;
    emit printTypeChanged();
    writeSelectDevice();
}

int MBluetoothDeviceManager::receiptsType()
{
    Q_D(MBluetoothDeviceManager);

    return d->receiptsType;
}

void MBluetoothDeviceManager::setReceiptsType(int type)
{
    Q_D(MBluetoothDeviceManager);
    if (d->receiptsType == type)
        return;

    d->receiptsType = type;
    emit receiptsTypeChanged();
    writeSelectDevice();
}

int MBluetoothDeviceManager::port()
{
    Q_D(MBluetoothDeviceManager);

    return d->lastWifiPor;
}

void MBluetoothDeviceManager::setPort(int p)
{
    Q_D(MBluetoothDeviceManager);
    if (d->lastWifiPor == quint16(p))
        return;

    d->lastWifiPor = quint16(p);
    emit portChanged();
}

QString MBluetoothDeviceManager::ipAddress()
{
    Q_D(MBluetoothDeviceManager);

    return d->lastWifiIpAddress.isEmpty() ? QStringLiteral("设置WIFI打印机") : d->lastWifiIpAddress;
}

void MBluetoothDeviceManager::setIpAddress(const QString &ip)
{
    Q_D(MBluetoothDeviceManager);
    qWarning()<<__FILE__<<__FUNCTION__<<__LINE__<<"=================="<<ip;
    if (d->lastWifiIpAddress == ip)
        return;

    d->lastWifiIpAddress = ip;
    emit ipAddressChanged();
}

QString MBluetoothDeviceManager::bluetoothName()
{
    Q_D(MBluetoothDeviceManager);
    return d->lastBluetoothName.isEmpty() ? QStringLiteral("设置蓝牙打印机") : d->lastBluetoothName;
}

void MBluetoothDeviceManager::setBluetoothName(const QString &bleName)
{

}

void MBluetoothDeviceManager::switchLocalDeviceHostmodel()
{
    Q_D(MBluetoothDeviceManager);
    if (d->localDevice) {
        if (d->localDevice->hostMode() == QBluetoothLocalDevice::HostPoweredOff) {
            d->localDevice->setHostMode(QBluetoothLocalDevice::HostDiscoverable);
        } else {
            d->localDevice->setHostMode(QBluetoothLocalDevice::HostPoweredOff);
        }
    }
}

void MBluetoothDeviceManager::startDiscoverDevice()
{
    Q_D(MBluetoothDeviceManager);

    d->discoveryAgent->stop();

    MBluetoothDeviceList *list;
    qWarning()<<__FILE__<<__FUNCTION__<<__LINE__<<"=================";
    list = d->pairedDevices;

    d->pairedDevices = new MBluetoothDeviceList();
    d->pairedDeviceModel->setDataList(d->pairedDevices);
    d->pairedDeviceModel->resetAll();
    while (!list->isEmpty()) {
        list->takeLast()->deleteLater();
    }
    qWarning()<<__FILE__<<__FUNCTION__<<__LINE__<<"=================";
    delete list;
    qWarning()<<__FILE__<<__FUNCTION__<<__LINE__<<"=================";

    list = d->discoveredDevices;

    qWarning()<<__FILE__<<__FUNCTION__<<__LINE__<<"=================";
    d->discoveredDevices = new MBluetoothDeviceList();
    d->discoveredDeviceModel->setDataList(d->discoveredDevices);
    d->discoveredDeviceModel->resetAll();
    while (!list->isEmpty()) {
        list->takeLast()->deleteLater();
    }
    qWarning()<<__FILE__<<__FUNCTION__<<__LINE__<<"=================";
    delete list;
    qWarning()<<__FILE__<<__FUNCTION__<<__LINE__<<"=================";

    d->discoveryAgent->start();
    emit discoverIsActiveChanged();
}

void MBluetoothDeviceManager::stopDiscoverDevice()
{
    Q_D(MBluetoothDeviceManager);
    d->discoveryAgent->stop();
}

void MBluetoothDeviceManager::pairingToRemote(MBluetoothDevice *device)
{
    Q_D(MBluetoothDeviceManager);
    QString remoteAddressStr = device->address();

    QBluetoothAddress remoteAddress(remoteAddressStr);
    if (d->localDevice->pairingStatus(remoteAddress) == QBluetoothLocalDevice::Paired) {
        d->localDevice->requestPairing(remoteAddress, QBluetoothLocalDevice::Unpaired);
    } else {
        d->localDevice->requestPairing(remoteAddress, QBluetoothLocalDevice::Paired);
    }
}

void MBluetoothDeviceManager::connectPrinterWithSocket(MBluetoothDevice *device)
{
    Q_D(MBluetoothDeviceManager);
    if (device->deviceType() == 0) {
        QString remoteAddressStr = device->address();
        if (d->bluetoothSocket->state() == QBluetoothSocket::UnconnectedState) {
            QBluetoothUuid uuid(QString("00001101-0000-1000-8000-00805F9B34FB"));
            d->bluetoothSocket->connectToService(QBluetoothAddress(remoteAddressStr), uuid);
        } else {

            if (d->bluetoothSocket->state() == QBluetoothSocket::ConnectedState) {
                device->setSocketConnectState(MBluetoothDevice::Connecting);
                d->bluetoothSocket->disconnectFromService();
                d->isWaitingConnectNewDevice = true;
            }
        }
    } else {
        d->tcpSocket->connectToHost(QHostAddress(device->ipAddress()), device->port());
    }
}

void MBluetoothDeviceManager::printTest()
{
    Q_D(MBluetoothDeviceManager);
    MOrderPrinter orderPrinter(this);
    orderPrinter.initPrintCom(d->bluetoothSocket);
    orderPrinter.printTest();
}

void MBluetoothDeviceManager::printTakeoutOrder(const QJsonObject &jsonObj)
{
    Q_D(MBluetoothDeviceManager);
    MOrderPrinter orderPrinter(this);
    orderPrinter.initPrintCom(d->bluetoothSocket);
    QObject obj;
    orderPrinter.printTakeoutOrder(&obj);
}

void MBluetoothDeviceManager::wifiPrint()
{
    Q_D(MBluetoothDeviceManager);

    MOrderPrinter orderPrinter(this);
    orderPrinter.initPrintCom(d->tcpSocket);
    orderPrinter.printTest();
}

MListModel *MBluetoothDeviceManager::remoteDevice()
{
    Q_D(MBluetoothDeviceManager);
    return d->discoveredDeviceModel;
}

MListModel *MBluetoothDeviceManager::pairedDevice()
{
    Q_D(MBluetoothDeviceManager);
    return d->pairedDeviceModel;
}

void MBluetoothDeviceManager::setPrintDevice(MBluetoothDevice *device)
{
    Q_D(MBluetoothDeviceManager);
    if (device) {
        if (d->selectPrint) {
            d->selectPrint->setIsConnected(false);
            d->selectPrint->setSocketConnectState(MBluetoothDevice::Unconnect);
        }
        d->selectPrint = device;
        //        d->selectPrint->setIsConnected(true);
        if (device->deviceType() == 0) {
            d->lastBluetoothName = device->deviceName();
            d->lastBluetoothAddress = device->address();
            d->lastBluetoothUuid = device->uuid();
        } else {
            d->lastWifiIpAddress = device->ipAddress();
            d->lastWifiPor = device->port();
        }

        writeSelectDevice();
        connectPrinterWithSocket(device);
    }
}

void MBluetoothDeviceManager::newDeviceDiscovered(const QBluetoothDeviceInfo &deviceInfo)
{
    Q_D(MBluetoothDeviceManager);
    QString newDevice;
    newDevice = QString("%1(%2)(%3)")
            .arg(deviceInfo.name())
            .arg(deviceInfo.address().toString())
            .arg(deviceInfo.deviceUuid().toString());
    qWarning()<<__FILE__<<__FUNCTION__<<__LINE__<<"=============>>>>"<<newDevice<<deviceInfo.majorDeviceClass()<<deviceInfo.minorDeviceClass();
    //    qWarning()<<__FILE__<<__FUNCTION__<<__LINE__<<"=============>>>>"<<d->localDevice->pairingStatus(deviceInfo.address());

    if (d->localDevice->pairingStatus(deviceInfo.address()) == QBluetoothLocalDevice::Unpaired) {
        if (retrieveDevice(d->discoveredDevices, deviceInfo) < 0) {
            MBluetoothDevice *btDevice = new MBluetoothDevice(this);
            btDevice->setDeviceInfo(deviceInfo);
            int first = d->discoveredDevices->size();
            d->discoveredDevices->append(btDevice);
            int last = d->discoveredDevices->size();
            d->discoveredDeviceModel->updateInertRow(first, --last);
        }
    } else {
        if (retrieveDevice(d->pairedDevices, deviceInfo) < 0) {
            MBluetoothDevice *btDevice = new MBluetoothDevice(this);
            btDevice->setDeviceInfo(deviceInfo);
            int first = d->pairedDevices->size();
            d->pairedDevices->append(btDevice);
            int last = d->pairedDevices->size();
            d->pairedDeviceModel->updateInertRow(first, --last);

            // 默认上次选择的设备
            if (d->selectPrint) {
                if (d->selectPrint->address() == btDevice->address()) {
                    btDevice->setIsConnected(true);

                    // TODO: delete first one
                    //d->selectPrint->deleteLater();
                    d->selectPrint = btDevice;
                }
            }
        }
    }
}

void MBluetoothDeviceManager::devicePaired(const QBluetoothAddress &address, QBluetoothLocalDevice::Pairing pairing)
{
    Q_D(MBluetoothDeviceManager);
    qWarning()<<__FILE__<<__FUNCTION__<<__LINE__<<"00000000000000000000 pairing finished"<<address<<pairing;
    if (pairing == QBluetoothLocalDevice::Unpaired) {

        int pairedIndex = retrieveDeviceWithAddress(d->pairedDevices, address.toString());
        if (pairedIndex >=0) {
            MBluetoothDevice *btDevice = d->pairedDevices->takeAt(pairedIndex);

            int first = d->discoveredDevices->size();
            d->discoveredDevices->prepend(btDevice);
            int last = d->discoveredDevices->size();
            d->discoveredDeviceModel->updateInertRow(0, 0);

            d->pairedDeviceModel->updateRemoveRow(pairedIndex, pairedIndex);
        }
    } else {
        int discoveredIndex = retrieveDeviceWithAddress(d->discoveredDevices, address.toString());
        if (discoveredIndex >= 0) {
            MBluetoothDevice *btDevice = d->discoveredDevices->takeAt(discoveredIndex);

            int first = d->pairedDevices->size();
            d->pairedDevices->append(btDevice);
            int last = d->pairedDevices->size();
            d->pairedDeviceModel->updateInertRow(first, --last);

            d->discoveredDeviceModel->updateRemoveRow(discoveredIndex, discoveredIndex);
        }
    }
}

void MBluetoothDeviceManager::tcpSocketError(QAbstractSocket::SocketError &error)
{
    Q_D(MBluetoothDeviceManager);
    qWarning()<<__FILE__<<__FUNCTION__<<__LINE__<<"==============================QAbstractSocket::SocketError"<<error<<d->lastWifiPor;
    //    d->lastWifiPor++;
    //    if (d->lastWifiPor < 9100) //65535
    //        d->tcpSocket->connectToHost(QHostAddress("192.168.31.157"), d->lastWifiPor);
}

int MBluetoothDeviceManager::retrieveDevice(QList<MBluetoothDevice *> *deviceList, const QBluetoothDeviceInfo &deviceInfo)
{
#if defined(Q_OS_OSX) || defined(Q_OS_IOS)
    for(int i = 0; i < deviceList->size(); i++) {
        if (deviceList->at(i)->uuid() == deviceInfo.deviceUuid().toString()) {
            return i;
        }
    }
#else
    for(int i = 0; i < deviceList->size(); i++) {
        if (deviceList->at(i)->address() == deviceInfo.address().toString()) {
            return i;
        }
    }
#endif
    return -1;
}

int MBluetoothDeviceManager::retrieveDeviceWithAddress(QList<MBluetoothDevice *> *deviceList, const QString &address)
{
#if defined(Q_OS_OSX) || defined(Q_OS_IOS)
    for(int i = 0; i < deviceList->size(); i++) {
        if (deviceList->at(i)->uuid() == address) {
            return i;
        }
    }
#else
    for(int i = 0; i < deviceList->size(); i++) {
        if (deviceList->at(i)->address() == address) {
            return i;
        }
    }
#endif
    return -1;
}

void MBluetoothDeviceManager::readSelectDevice()
{
    Q_D(MBluetoothDeviceManager);
    QString settingpath;
#ifdef ANDROID
    settingpath = QString("%1/printer.ini").arg(getenv("HOME"));
#else
    settingpath = QDir::currentPath() + "/printer.ini";
#endif

    QSettings settings(settingpath, QSettings::IniFormat);

    settings.beginGroup("Print");
    d->printType = settings.value("PrintType", 1).toInt();
    d->receiptsType = settings.value("ReceiptsType", 0).toInt();
    settings.endGroup();

    settings.beginGroup("Bluetooth");
    d->lastBluetoothName = settings.value("BluetoothName").toString();
    d->lastBluetoothAddress = settings.value("BluetoothAddress").toString();
    d->lastBluetoothUuid = settings.value("BluetoothUuid").toString();
    settings.endGroup();
    if (!d->lastBluetoothAddress.isEmpty() || !d->lastBluetoothUuid.isEmpty()) {
        d->selectPrint = new MBluetoothDevice(this);
        d->selectPrint->setDeviceName(d->lastBluetoothName);
        d->selectPrint->setAddress(d->lastBluetoothAddress);
        d->selectPrint->setUuid(d->lastBluetoothUuid);
    }

    settings.beginGroup("Wifi");
    d->lastWifiIpAddress = settings.value("IpAddress").toString();
    d->lastWifiPor = settings.value("Por", 1024).toInt();
    settings.endGroup();

    qWarning()<<__FILE__<<__FUNCTION__<<__LINE__<<d->printType<<d->receiptsType<<d->lastBluetoothName<<d->lastBluetoothAddress<<d->lastBluetoothUuid<<d->lastWifiIpAddress<<d->lastWifiPor;
}

void MBluetoothDeviceManager::writeSelectDevice()
{
    Q_D(MBluetoothDeviceManager);
    QString settingpath;
#ifdef ANDROID
    settingpath = QString("%1/printer.ini").arg(getenv("HOME"));
#else
    settingpath = QDir::currentPath() + "/printer.ini";
#endif
    QSettings settings(settingpath, QSettings::IniFormat);
    settings.beginGroup("Print");
    settings.setValue("PrintType", d->printType);
    settings.setValue("ReceiptsType", d->receiptsType);
    settings.endGroup();

    settings.beginGroup("Bluetooth");
    settings.setValue("BluetoothName", d->lastBluetoothName);
    settings.setValue("BluetoothAddress", d->lastBluetoothAddress);
    settings.setValue("BluetoothUuid", d->lastBluetoothUuid);
    settings.endGroup();

    settings.beginGroup("Wifi");
    settings.setValue("IpAddress", d->lastWifiIpAddress);
    settings.setValue("Por", d->lastWifiPor);
    settings.endGroup();
}

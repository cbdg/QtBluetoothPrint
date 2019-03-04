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

    MBluetoothDeviceList *list;

    list = d->pairedDevices;

    d->pairedDevices = new MBluetoothDeviceList();
    d->pairedDeviceModel->setDataList(d->pairedDevices);
    d->pairedDeviceModel->resetAll();
    while (!list->isEmpty()) {
        list->takeLast()->deleteLater();
    }
    delete list;

    list = d->discoveredDevices;

    d->discoveredDevices = new MBluetoothDeviceList();
    d->discoveredDeviceModel->setDataList(d->discoveredDevices);
    d->discoveredDeviceModel->resetAll();
    while (!list->isEmpty()) {
        list->takeLast()->deleteLater();
    }
    delete list;

    d->discoveryAgent->stop();
    d->discoveryAgent->start();
    emit discoverIsActiveChanged();
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
//    qWarning()<<__FILE__<<__FUNCTION__<<__LINE__<<"=============>>>>"<<newDevice<<deviceInfo.majorDeviceClass()<<deviceInfo.minorDeviceClass();
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
    settingpath = QString("%1/bluetooth.ini").arg(getenv("HOME"));
#else
    settingpath = QDir::currentPath() + "/bluetooth.ini";
#endif
    QString printName;
    QString printAddress;
    QString printUuid;
    QSettings settings(settingpath, QSettings::IniFormat);
    settings.beginGroup("Print");
    printName = settings.value("PrintName").toString();
    printAddress = settings.value("PrintAddress").toString();
    printUuid = settings.value("PrintUuid").toString();
    settings.endGroup();
    if (!printAddress.isEmpty() || !printUuid.isEmpty()) {
        d->selectPrint = new MBluetoothDevice(this);
        d->selectPrint->setDeviceName(printName);
        d->selectPrint->setAddress(printAddress);
        d->selectPrint->setUuid(printUuid);
    }
}

void MBluetoothDeviceManager::writeSelectDevice()
{
    Q_D(MBluetoothDeviceManager);
    QString settingpath;
#ifdef ANDROID
    settingpath = QString("%1/bluetooth.ini").arg(getenv("HOME"));
#else
    settingpath = QDir::currentPath() + "/bluetooth.ini";
#endif
    QSettings settings(settingpath, QSettings::IniFormat);
    settings.beginGroup("Print");
    settings.setValue("PrintName", d->selectPrint->deviceName());
    settings.setValue("PrintAddress", d->selectPrint->address());
    settings.setValue("PrintUuid", d->selectPrint->uuid());
    settings.endGroup();
}

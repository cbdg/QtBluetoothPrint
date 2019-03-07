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
    discoveredDevices = nullptr;
    pairedDevices = nullptr;
    pairedDeviceModel = nullptr;
    discoveredDeviceModel = nullptr;
    bluetoothPrintCommand = nullptr;
    selectPrint = nullptr;
    wifiPrint = nullptr;
    bluetoothPrint = nullptr;
    isWaitingConnectNewDevice = false;

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
    d->bluetoothPrintCommand = new MBluetoothPrintCommand(nullptr, this);

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

    connect(d->localDevice, &QBluetoothLocalDevice::pairingDisplayConfirmation, [=](const QBluetoothAddress &address, QString pin){
        //qWarning()<<__FILE__<<__FUNCTION__<<__LINE__<<"-------------------- pairingDisplayConfirmation"<<address<<pin;
    });
    connect(d->localDevice, &QBluetoothLocalDevice::pairingDisplayPinCode, [=](const QBluetoothAddress &address, QString pin){
        //qWarning()<<__FILE__<<__FUNCTION__<<__LINE__<<"XXXXXXXXXXXXXXXXXXXX pairingDisplayPinCode"<<address<<pin;
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
    if (type == 1)
        d->selectPrint = d->bluetoothPrint;
    else if (type == 2 )
        d->selectPrint = d->wifiPrint;
    else
        d->selectPrint = nullptr;

    connectPrinterWithSocket(d->selectPrint);
    emit currentDeviceChanged();
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
    writeSelectDevice();

    if (d->wifiPrint) {
        d->wifiPrint->setIsConnected(false);
        d->wifiPrint->disconnectSocket();
        if (d->selectPrint == d->wifiPrint) {
            d->wifiPrint = new MBluetoothDevice(this);
            d->wifiPrint->setDeviceType(1);
            d->wifiPrint->setIpAddress(d->lastWifiIpAddress);
            d->wifiPrint->setPort(d->lastWifiPor);
            d->selectPrint = d->wifiPrint;
            d->selectPrint->connectSocket();
            d->wifiPrint->setIsCurrentDevice(true);
            emit currentDeviceChanged();
        } else {
            d->wifiPrint = new MBluetoothDevice(this);
            d->wifiPrint->setDeviceType(1);
            d->wifiPrint->setIpAddress(d->lastWifiIpAddress);
            d->wifiPrint->setPort(d->lastWifiPor);
        }
    } else {
        d->wifiPrint = new MBluetoothDevice(this);
        d->wifiPrint->setDeviceType(1);
        d->wifiPrint->setIpAddress(d->lastWifiIpAddress);
        d->wifiPrint->setPort(d->lastWifiPor);
    }
    emit wifiDeviceChanged();
}

QString MBluetoothDeviceManager::ipAddress()
{
    Q_D(MBluetoothDeviceManager);

    return d->lastWifiIpAddress.isEmpty() ? QStringLiteral("设置WIFI打印机") : d->lastWifiIpAddress;
}

void MBluetoothDeviceManager::setIpAddress(const QString &ip)
{
    Q_D(MBluetoothDeviceManager);
    if (d->lastWifiIpAddress == ip)
        return;

    d->lastWifiIpAddress = ip;

    emit ipAddressChanged();
    writeSelectDevice();

    if (d->wifiPrint) {
        d->wifiPrint->setIsConnected(false);
        d->wifiPrint->disconnectSocket();
        if (d->selectPrint == d->wifiPrint) {
            d->wifiPrint = new MBluetoothDevice(this);
            d->wifiPrint->setDeviceType(1);
            d->wifiPrint->setIpAddress(d->lastWifiIpAddress);
            d->wifiPrint->setPort(d->lastWifiPor);
            d->selectPrint = d->wifiPrint;
            d->selectPrint->connectSocket();
            d->wifiPrint->setIsCurrentDevice(true);
            emit currentDeviceChanged();
        } else {
            d->wifiPrint = new MBluetoothDevice(this);
            d->wifiPrint->setDeviceType(1);
            d->wifiPrint->setIpAddress(d->lastWifiIpAddress);
            d->wifiPrint->setPort(d->lastWifiPor);
        }
    } else {
        d->wifiPrint = new MBluetoothDevice(this);
        d->wifiPrint->setDeviceType(1);
        d->wifiPrint->setIpAddress(d->lastWifiIpAddress);
        d->wifiPrint->setPort(d->lastWifiPor);
    }
    emit wifiDeviceChanged();
}

QString MBluetoothDeviceManager::bluetoothName()
{
    Q_D(MBluetoothDeviceManager);
    return d->lastBluetoothName.isEmpty() ? QStringLiteral("设置蓝牙打印机") : d->lastBluetoothName;
}

void MBluetoothDeviceManager::setBluetoothName(const QString &bleName)
{

}

MBluetoothDevice *MBluetoothDeviceManager::currentDevice()
{
    Q_D(MBluetoothDeviceManager);
    return d->selectPrint;
}

void MBluetoothDeviceManager::setCurrentDevice(MBluetoothDevice *device)
{
    Q_D(MBluetoothDeviceManager);
    if (device) {
        if (d->selectPrint) {
            d->selectPrint->setIsConnected(false);
            d->selectPrint->setIsCurrentDevice(false);
            d->selectPrint->setSocketConnectState(MBluetoothDevice::Unconnect);
        }

        if (device->deviceType() == 0) {
            d->lastBluetoothName = device->deviceName();
            d->lastBluetoothAddress = device->address();
            d->lastBluetoothUuid = device->uuid();

            emit bluetoothNameChanged();
            if (d->printType == 2) {
                d->printType = 1;
                emit printTypeChanged();
            }
        } else {
            d->lastWifiIpAddress = device->ipAddress();
            d->lastWifiPor = device->port();

            emit ipAddressChanged();
            if (d->printType == 1) {
                d->printType = 2;
                emit printTypeChanged();
            }
        }

        writeSelectDevice();
        d->selectPrint = device;
        d->selectPrint->setIsCurrentDevice(true);
        connectPrinterWithSocket(device);

        emit currentDeviceChanged();
    }
}

MBluetoothDevice *MBluetoothDeviceManager::wifiDevice()
{
    Q_D(MBluetoothDeviceManager);
    return d->wifiPrint;
}

void MBluetoothDeviceManager::setWifiDevice(MBluetoothDevice *device)
{

}

MBluetoothDevice *MBluetoothDeviceManager::bluetoothDevice()
{
    Q_D(MBluetoothDeviceManager);
    return d->bluetoothPrint;
}

void MBluetoothDeviceManager::setBluetoothDevice(MBluetoothDevice *device)
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
    //    qWarning()<<__FILE__<<__FUNCTION__<<__LINE__<<"=================";
    list = d->pairedDevices;

    d->pairedDevices = new MBluetoothDeviceList();
    d->pairedDeviceModel->setDataList(d->pairedDevices);
    d->pairedDeviceModel->resetAll();
    while (!list->isEmpty()) {
        list->takeLast()->deleteLater();
    }
    //    qWarning()<<__FILE__<<__FUNCTION__<<__LINE__<<"=================";
    delete list;
    //    qWarning()<<__FILE__<<__FUNCTION__<<__LINE__<<"=================";

    list = d->discoveredDevices;

    //    qWarning()<<__FILE__<<__FUNCTION__<<__LINE__<<"=================";
    d->discoveredDevices = new MBluetoothDeviceList();
    d->discoveredDeviceModel->setDataList(d->discoveredDevices);
    d->discoveredDeviceModel->resetAll();
    while (!list->isEmpty()) {
        list->takeLast()->deleteLater();
    }
    //    qWarning()<<__FILE__<<__FUNCTION__<<__LINE__<<"=================";
    delete list;
    //qWarning()<<__FILE__<<__FUNCTION__<<__LINE__<<"=================";

    d->discoveryAgent->start();
    emit discoverIsActiveChanged();
}

void MBluetoothDeviceManager::stopDiscoverDevice()
{
    Q_D(MBluetoothDeviceManager);
    d->discoveryAgent->stop();
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
    if (device) {
        device->connectSocket();
    }
}

void MBluetoothDeviceManager::printTest()
{
    Q_D(MBluetoothDeviceManager);
    MOrderPrinter orderPrinter(this);
    if (d->selectPrint) {
        orderPrinter.initPrintCom(d->selectPrint->deviceSocket());
        orderPrinter.printTest();
    }
}

void MBluetoothDeviceManager::printTakeoutOrder(const QJsonObject &jsonObj)
{
    Q_D(MBluetoothDeviceManager);
    MOrderPrinter orderPrinter(this);
    if (d->selectPrint) {
        orderPrinter.initPrintCom(d->selectPrint->deviceSocket());
        QObject obj;
        orderPrinter.printTakeoutOrder(&obj);
    }
}

void MBluetoothDeviceManager::resetBluetoothPrint()
{
    Q_D(MBluetoothDeviceManager);
    if (d->printType == 1) {
        qWarning()<<__FILE__<<__FUNCTION__<<__LINE__<<"=================";
        if (d->bluetoothPrint != d->selectPrint) {
            qWarning()<<__FILE__<<__FUNCTION__<<__LINE__<<"================="<<d->bluetoothPrint<<d->selectPrint;
            if (d->bluetoothPrint)
                d->bluetoothPrint->disconnectSocket();
            qWarning()<<__FILE__<<__FUNCTION__<<__LINE__<<"=================";
            d->selectPrint->disconnectSocket();
            qWarning()<<__FILE__<<__FUNCTION__<<__LINE__<<"=================";
            createPrintDevice();
            qWarning()<<__FILE__<<__FUNCTION__<<__LINE__<<"=================";
            d->selectPrint->connectSocket();
        }
    }
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

void MBluetoothDeviceManager::newDeviceDiscovered(const QBluetoothDeviceInfo &deviceInfo)
{
    Q_D(MBluetoothDeviceManager);
    QString newDevice;
    newDevice = QString("%1(%2)(%3)")
            .arg(deviceInfo.name())
            .arg(deviceInfo.address().toString())
            .arg(deviceInfo.deviceUuid().toString());
    //qWarning()<<__FILE__<<__FUNCTION__<<__LINE__<<"=============>>>>"<<newDevice<<deviceInfo.majorDeviceClass()<<deviceInfo.minorDeviceClass();
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
                    btDevice->setIsCurrentDevice(true);

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

    settings.beginGroup("Wifi");
    d->lastWifiIpAddress = settings.value("IpAddress").toString();
    d->lastWifiPor = settings.value("Por", 1024).toInt();
    settings.endGroup();

    createPrintDevice();

    if (!d->wifiPrint) {
        d->wifiPrint = new MBluetoothDevice(this);
        d->wifiPrint->setDeviceType(1);
        d->wifiPrint->setIpAddress(d->lastWifiIpAddress);
        d->wifiPrint->setPort(d->lastWifiPor);
    }

    if (!d->bluetoothPrint) {
        d->bluetoothPrint = new MBluetoothDevice(this);
        d->bluetoothPrint->setDeviceType(0);
        d->bluetoothPrint->setDeviceName(d->lastBluetoothName);
        d->bluetoothPrint->setAddress(d->lastBluetoothAddress);
        d->bluetoothPrint->setUuid(d->lastBluetoothUuid);
    }
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

void MBluetoothDeviceManager::createPrintDevice()
{
    Q_D(MBluetoothDeviceManager);

    MBluetoothDevice * tmpDevice = d->selectPrint;
    if (d->printType == 1) {
        if (!d->lastBluetoothAddress.isEmpty() || !d->lastBluetoothUuid.isEmpty()) {
            d->selectPrint = new MBluetoothDevice(this);
            d->selectPrint->setDeviceType(0);
            d->selectPrint->setDeviceName(d->lastBluetoothName);
            d->selectPrint->setAddress(d->lastBluetoothAddress);
            d->selectPrint->setUuid(d->lastBluetoothUuid);

            if (d->bluetoothPrint)
                d->bluetoothPrint->deleteLater();
            d->bluetoothPrint = d->selectPrint;

            emit currentDeviceChanged();
        }
    } else if (d->printType == 2){
        if (!d->lastWifiIpAddress.isEmpty()) {
            d->selectPrint = new MBluetoothDevice(this);
            d->selectPrint->setDeviceType(1);
            d->selectPrint->setIpAddress(d->lastWifiIpAddress);
            d->selectPrint->setPort(d->lastWifiPor);

            if (d->wifiPrint)
                d->wifiPrint->deleteLater();
            d->wifiPrint = d->selectPrint;

            emit currentDeviceChanged();
        }
    }
}

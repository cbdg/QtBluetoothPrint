#include "mbluetooth.h"
#include "mbluetooth_p.h"

#include <vector>

#include <QDebug>
#include <QBluetoothAddress>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothTransferManager>
#include <QBluetoothTransferRequest>
#include <QBluetoothTransferReply>
#include <QbluetoothSocket>
#include <QBuffer>
#include <QFile>
#include <QTextCodec>
#include <QImage>
#include <QBitmap>
#include "cbinarization.h"

// bluetooth printer operation reference
// https://www.jianshu.com/p/0fe3a7e06f57


/**
 * 图片灰度的转化
 */
int RGB2Gray(int r, int g, int b) {
//    int gray = (int) (0.29900 * r + 0.58700 * g + 0.11400 * b);  //灰度转化公式
//    return gray;
    return (uchar)((((qint32)((r << 5) + (r << 2) + (r << 1)))+ (qint32)((g << 6) + (g << 3) + (g << 1) + g)
+ (qint32)((b << 4) - b)) >> 7);
}

//QBYTE RGBtoGRAY(QBYTE r, QBYTE g, QBYTE b)
//{
//    return (QBYTE)((((QUINT32)((r << 5) + (r << 2) + (r << 1)))+ (QUINT32)((g << 6) + (g << 3) + (g << 1) + g)
//+ (QUINT32)((b << 4) - b)) >> 7);
//}

/**
 * 灰度图片黑白化，黑色是1，白色是0
 *
 * @param x   横坐标
 * @param y   纵坐标
 * @param bit 位图
 * @return
 */
char px2Byte(int x, int y, QImage bit) {
    if (x < bit.width() && y < bit.height()) {
        char b;
        int pixel = bit.bits()[y * x + x];
        int red = qRed(pixel);//(pixel & 0x00ff0000) >> 16; // 取高两位
        int green = qGreen(pixel);//(pixel & 0x0000ff00) >> 8; // 取中两位
        int blue = qBlue(pixel);//pixel & 0x000000ff; // 取低两位
        int gray = RGB2Gray(red, green, blue);
        if (gray < 128) {
            b = 1;
        } else {
            b = 0;
        }
        //        qWarning()<<__FILE__<<__FUNCTION__<<__LINE__<<x<<y<<pixel<<gray<<int(b);
        return b;
    }
    return 0;
}


QByteArray PosCmd(std::vector<int> v)
{
    QByteArray byte;
    for (int i = 0; v.size(); ++i)
        byte.append(v[i]);
    return byte;
}

//ESC POS 指令打印图片
std::vector<QByteArray> ConvertImageToByteArray(
        const QImage &img) {
    std::vector<QByteArray> cmd_serial;
    // just print image !!
    int w = img.width();
    int rw = (w +7) / 8;
    int rh = img.height();
    int h = ((rh + 23) / 24) * 24;
    int32_t img_seg_height = h;

    qWarning()<<__FILE__<<__FUNCTION__<<__LINE__<<"================="<<img.width()<<img.height()<<rw<<w<<rh<<h;
    // 初始化打印机
    //cmd_serial.push_back(PosCmd({27, 64}));
    QByteArray ba;
    ba.clear();
    ba.append(27);
    ba.append(64);
    cmd_serial.push_back(ba);
    // recv data !!
    QByteArray cmd;
    int32_t img_seg_count = h / img_seg_height;
    for (int si = 0; si < img_seg_count; si++) {
        cmd.clear();
        cmd.append(29);
        cmd.append(118);
        cmd.append(48);
        cmd.append((char)0);
        cmd.append((char)(rw % 256));
        cmd.append((char)(rw / 256));
        cmd.append((char)(img_seg_height % 256));
        cmd.append((char)(img_seg_height / 256));

        for (int ph = 0; ph < rh; ph++) {
            for (int rwi = 0; rwi < rw; rwi++) {
                char t = 0x00;

                for (int j = 0; j < 8; ++j) {
                    int image_x = rwi * 8 + j;
//                    int image_y = ph + si * img_seg_height;
                    int v;
                    if (image_x >= w) {
                        v = 0;
//                    } else if (image_y >= rh) {
//                        qWarning()<<__FILE__<<__FUNCTION__<<__LINE__<<"--------------------------"<<image_y<<rh;
//                        v = 0;
                    } else {
                        v = qBlue(img.pixel(image_x, ph/*image_y*/)) < 0xc0 ? 1 : 0;
//                        v = px2Byte(image_x, image_y, img);
                    }
                    t |= (v << (7 - j));
                }
                cmd.append(t);
            }
        }
        qWarning()<<__FILE__<<__FUNCTION__<<__LINE__<<"================="<<si<<img_seg_count<<img_seg_height<<h;
        cmd_serial.push_back(cmd);
    }
    //cmd_serial.push_back(PosCmd({29, 86, 66, 0}));
//    ba.clear();
//    ba.append(29);
//    ba.append(86);
//    ba.append(66);
//    ba.append((char)0);

//    ba.clear();
//    ba.append(0x1b);
//    ba.append(0x4a);
//    ba.append(0x18);
//    cmd_serial.push_back(ba);

    return cmd_serial;
}


MBluetoothPrivate::MBluetoothPrivate(MBluetooth *parent)
{
    Q_Q(MBluetooth);
    q_ptr = parent;
    discoveryAgent = new QBluetoothDeviceDiscoveryAgent(parent);

    // creater a transfer manager
    transferManager = new QBluetoothTransferManager(parent);

    // create a socket
    bluetoothSocket = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol,parent);
}

//-------------------------------

MBluetooth::MBluetooth(QObject *parent)
    : QObject(parent)
    , d_ptr(new MBluetoothPrivate(this))
{
    Q_D(MBluetooth);
    connect(&d->locaDevice, &QBluetoothLocalDevice::hostModeStateChanged, [=](){emit localDeviceStatusChanged();});
    connect(d->discoveryAgent, SIGNAL(deviceDiscovered(QBluetoothDeviceInfo)),
            this, SLOT(deviceDiscovered(QBluetoothDeviceInfo)));
    connect(d->discoveryAgent, &QBluetoothDeviceDiscoveryAgent::finished, [=](){emit discoverIsActiveChanged();});

    connect(&d->locaDevice, &QBluetoothLocalDevice::deviceConnected, [=](const QBluetoothAddress &address){
        qWarning()<<__FILE__<<__FUNCTION__<<__LINE__<<"===================="<<address;
    });
    connect(&d->locaDevice, &QBluetoothLocalDevice::deviceDisconnected, [=](const QBluetoothAddress &address){
        qWarning()<<__FILE__<<__FUNCTION__<<__LINE__<<"===================="<<address;
    });


    // pairing device
    connect(&d->locaDevice, &QBluetoothLocalDevice::pairingFinished, [=](const QBluetoothAddress &address, QBluetoothLocalDevice::Pairing pairing){
        qWarning()<<__FILE__<<__FUNCTION__<<__LINE__<<"00000000000000000000 pairing finished"<<address<<pairing;
        //        if (d->locaDevice.pairingStatus(device.address()) == QBluetoothLocalDevice::Unpaired) {
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
    });
    connect(&d->locaDevice, &QBluetoothLocalDevice::pairingDisplayConfirmation, [=](const QBluetoothAddress &address, QString pin){
        qWarning()<<__FILE__<<__FUNCTION__<<__LINE__<<"-------------------- pairingDisplayConfirmation"<<address<<pin;
    });
    connect(&d->locaDevice, &QBluetoothLocalDevice::pairingDisplayPinCode, [=](const QBluetoothAddress &address, QString pin){
        qWarning()<<__FILE__<<__FUNCTION__<<__LINE__<<"XXXXXXXXXXXXXXXXXXXX pairingDisplayPinCode"<<address<<pin;
    });

    // socekt connect
    connect(d->bluetoothSocket, &QBluetoothSocket::connected, [=](){
        qWarning()<<__FILE__<<__FUNCTION__<<__LINE__<<"XXXXXXXXXXXXXXXXXXXX QBluetoothSocket::connected";
        if (d->bluetoothSocket->open(QIODevice::ReadWrite)) {
            qWarning()<<__FILE__<<__FUNCTION__<<__LINE__<<"XXXXXXXXXXXXXXXXXXXX QBluetoothSocket::open";
            QByteArray ba;
            ba.append(0x1B);
            ba.append(0x40);
            qint64 s = d->bluetoothSocket->write(ba);

//            // 剧中
//            ba.clear();
//            ba.append(0x1B);
//            ba.append(0x61);
//            ba.append(1);
//            s = d->bluetoothSocket->write(ba);

            //            // 设置字符打印方式
            //            ba.clear();
            //            ba.append(0x1B);
            //            ba.append(0x21);
            //            ba.append(0xa9); //ESC　！　n是综合性的字符打印方式设置命令，用于选择打印字符的大小和下划线
            //            s = d->bluetoothSocket->write(ba);

//            //设置行距为0的指令
//            ba.clear();
//            ba.append(0x1B);
//            ba.append(0x33);
//            ba.append(0x30); // 0 十进制ascii 48 十六进制 0x30
//            s = d->bluetoothSocket->write(ba);
//            //            // 逐行打印

//            qWarning()<<__FILE__<<__FUNCTION__<<__LINE__<<"XXXXXXXXXXXXXXXXXXXX QBluetoothSocket::write"<< s;
//            s = d->bluetoothSocket->write("hello bluetooth");
//            qWarning()<<__FILE__<<__FUNCTION__<<__LINE__<<"XXXXXXXXXXXXXXXXXXXX QBluetoothSocket::write"<< s;
//            //======
//            QString zhstr = QStringLiteral("\t打印测试\n");
//            //打印中文 要加结束换行
//            QTextCodec *codec = QTextCodec::codecForName("gbk");
//            QByteArray encodedString = codec->fromUnicode(zhstr);
//            s = d->bluetoothSocket->write(encodedString);
//            //======

            // 打印图片
            QImage img(":/20180520000759880.png");
//            img = img.convertToFormat(QImage::Format_Grayscale8);
//            bool re = img.save("/sdcard/icon_m_coin.bmp");

            CBinarization* binImg = new CBinarization(":/20180520000759880.png");
            QImage* grayImage = binImg->grayScaleImg();
            img = img.scaledToWidth(380);
            grayImage->save("/sdcard/icon_m_coin_grayimg.png");


            int threshold = binImg->Otsu(grayImage);    // 这就是计算出的阈值
            if (-1 == threshold)
            {
                return;    // error
            }

            binImg->threshold = threshold;

            QImage* binaryImg = binImg->process(grayImage);

            if (NULL == binaryImg)
            {
                return;    // error
            }

            binaryImg->save("/sdcard/icon_m_coin.png");

            ba.clear();

            std::vector<QByteArray> vectorBy = ConvertImageToByteArray(img);
            qWarning()<<__FILE__<<__FUNCTION__<<__LINE__<<"EEEEEEEEEEEEEEEEEEEEEEE std::vector<QByteArray>"<<vectorBy.size();
            for (int i = 0; i < vectorBy.size(); i++) {
                s = d->bluetoothSocket->write(vectorBy[i]);
                }

            ba.clear();
            ba.append(0x1B);
            ba.append(0x4A);
            ba.append(0x01);
            s = d->bluetoothSocket->write(ba);

            //======
            QString zhstr = QStringLiteral("\t打印测试\n");
            //打印中文 要加结束换行
            QTextCodec *codec = QTextCodec::codecForName("gbk");
            QByteArray encodedString = codec->fromUnicode(zhstr);
            s = d->bluetoothSocket->write(encodedString);
            //======

            d->bluetoothSocket->close();
        }
    });
    connect(d->bluetoothSocket, static_cast<void (QBluetoothSocket::*)(QBluetoothSocket::SocketError)>(&QBluetoothSocket::error)
            , [=](QBluetoothSocket::SocketError error){
        qWarning()<<__FILE__<<__FUNCTION__<<__LINE__<<"EEEEEEEEEEEEEEEEEEEEEEE SocketError"<<error;
    });
}

MBluetooth::~MBluetooth()
{

}

QString MBluetooth::localDevice()
{
    Q_D(MBluetooth);
    qWarning()<<__FILE__<<__FUNCTION__<<__LINE__<<"---------------------------"<<d->locaDevice.name()<<d->locaDevice.address();
    return d->locaDevice.name().isEmpty() ? d->locaDevice.address().toString() : d->locaDevice.name();
}

QStringList MBluetooth::connectDevice()
{
    Q_D(MBluetooth);

    // Get connected devices
    QList<QBluetoothAddress> remotes;
    remotes = d->locaDevice.connectedDevices();

    QStringList addressList;
    foreach (QBluetoothAddress addr, remotes) {
        addressList << QString("%1").arg(addr.toString());
    }

    return addressList;
}

int MBluetooth::localDeviceStatus()
{
    Q_D(MBluetooth);
    return d->locaDevice.hostMode();
}

//可用设备
QStringList MBluetooth::remoteDevice()
{
    Q_D(MBluetooth);
    return d->discoveredDevice;
}

//已配对的设备
QStringList MBluetooth::pairedDevice()
{
    Q_D(MBluetooth);
    return d->pairedDevice;
}

bool MBluetooth::discoverIsActive()
{
    Q_D(MBluetooth);
    return d->discoveryAgent->isActive();
}

void MBluetooth::switchLocalDeviceHostmodel()
{
    Q_D(MBluetooth);
    if (d->locaDevice.hostMode() == QBluetoothLocalDevice::HostPoweredOff) {
        d->locaDevice.setHostMode(QBluetoothLocalDevice::HostDiscoverable);
    } else {
        d->locaDevice.setHostMode(QBluetoothLocalDevice::HostPoweredOff);
    }
}

void MBluetooth::startDiscoverDevice()
{
    Q_D(MBluetooth);
    d->discoveredDevice.clear();
    d->discoveryAgent->stop();
    d->discoveryAgent->start();
    emit discoverIsActiveChanged();
}

void MBluetooth::sendFileToRemote(const QString &address)
{
    Q_D(MBluetooth);
    QString remoteAddressStr = address.section("(", 1);
    remoteAddressStr = remoteAddressStr.section(")", 0, 0);
    qWarning()<<__FILE__<<__FUNCTION__<<__LINE__<<address<<remoteAddressStr;

    // Create the transfer request and file to be send
    QBluetoothAddress remoteAddress(remoteAddressStr);
    QBluetoothTransferRequest request(remoteAddress);
    qWarning()<<__FILE__<<__FUNCTION__<<__LINE__<<address<<remoteAddressStr;

    QFile *file = new QFile("bluetoothtest.txt");
    if (file->open(QIODevice::ReadWrite)) {
        file->write("Hello Bluetooth, I'm from Qt&Android");
    }
    file->close();
    if (file->open(QIODevice::ReadOnly)) {
        qWarning()<<__FILE__<<__FUNCTION__<<__LINE__<<"====ffff"<<file->readAll();
    }
    file->close();


    qWarning()<<__FILE__<<__FUNCTION__<<__LINE__<<address<<remoteAddressStr;

    QBluetoothTransferReply *reply = d->transferManager->put(request, file);
    if (reply) { // android ios can not support opp so reply return null
        connect(reply, &QBluetoothTransferReply::finished, [=](QBluetoothTransferReply *reply){
            qWarning()<<__FILE__<<__FUNCTION__<<__LINE__<<reply->errorString();
        });

        // this connect not work without nothing warning, why ??
        // TODO
        connect(reply, static_cast<void (QBluetoothTransferReply::*)(QBluetoothTransferReply::TransferError)>(&QBluetoothTransferReply::error),
                [=](QBluetoothTransferReply::TransferError errorType){
            qWarning()<<__FILE__<<__FUNCTION__<<__LINE__<<errorType;
        });
    }

    qWarning()<<__FILE__<<__FUNCTION__<<__LINE__<<address<<remoteAddressStr<<reply;
    connect(reply, SIGNAL(finished(QBluetoothTransferReply*)), this, SLOT(transferFinished(QBluetoothTransferReply*)));
    connect(reply, SIGNAL(error(QBluetoothTransferReply::TransferError)), this, SLOT(transferError(QBluetoothTransferReply::TransferError)));
    qWarning()<<__FILE__<<__FUNCTION__<<__LINE__<<address<<remoteAddressStr;
}

void MBluetooth::pairingToRemote(const QString &address)
{
    Q_D(MBluetooth);
    QString remoteAddressStr = address.section("(", 1);
    remoteAddressStr = remoteAddressStr.section(")", 0, 0);
    qWarning()<<__FILE__<<__FUNCTION__<<__LINE__<<address<<remoteAddressStr;

    QBluetoothAddress remoteAddress(remoteAddressStr);
    if (d->locaDevice.pairingStatus(remoteAddress) == QBluetoothLocalDevice::Paired) {
        d->locaDevice.requestPairing(remoteAddress, QBluetoothLocalDevice::Unpaired);
    } else {
        d->locaDevice.requestPairing(remoteAddress, QBluetoothLocalDevice::Paired);
    }

    qWarning()<<__FILE__<<__FUNCTION__<<__LINE__<<"=============== pairing status"<<d->locaDevice.pairingStatus(remoteAddress);
}

void MBluetooth::connectPrinter(const QString &address)
{
    Q_D(MBluetooth);
    QString remoteAddressStr = address.section("(", 1);
    remoteAddressStr = remoteAddressStr.section(")", 0, 0);
    qWarning()<<__FILE__<<__FUNCTION__<<__LINE__<<address<<remoteAddressStr;
    QBluetoothUuid uuid(QString("00001101-0000-1000-8000-00805F9B34FB"));
    d->bluetoothSocket->connectToService(QBluetoothAddress(remoteAddressStr), uuid);
}

// In your local slot, read information about the found devices
void MBluetooth::deviceDiscovered(const QBluetoothDeviceInfo &device)
{
    Q_D(MBluetooth);
    QString newDevice = QString("%1(%2)").arg(device.name()).arg(device.address().toString());
    qDebug() << "Found new device:" << device.name() << '(' << device.address().toString() << ')';
    qWarning() << __FILE__<<__FUNCTION__<<__LINE__<< device.coreConfigurations();
    qWarning() << __FILE__<<__FUNCTION__<<__LINE__<< d->locaDevice.pairingStatus(device.address());

    if (d->locaDevice.pairingStatus(device.address()) == QBluetoothLocalDevice::Unpaired) {
        if (!d->discoveredDevice.contains(newDevice)){
            d->discoveredDevice << QString("%1(%2)").arg(device.name()).arg(device.address().toString());
            emit remoteDeviceChanged();
        }
    } else {
        if (!d->pairedDevice.contains(newDevice)){
            d->pairedDevice << QString("%1(%2)").arg(device.name()).arg(device.address().toString());
            emit pairedDeviceChanged();
        }
    }
}

void MBluetooth::transferFinished(QBluetoothTransferReply *reply)
{
    qWarning()<<__FILE__<<__FUNCTION__<<__LINE__<<"================="<<reply->errorString();
    reply->deleteLater();
}

void MBluetooth::transferError(QBluetoothTransferReply::TransferError errorType)
{
    qWarning()<<__FILE__<<__FUNCTION__<<__LINE__<<"====+++++++++++=="<<errorType;
}

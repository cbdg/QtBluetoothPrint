#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "mbluetooth.h"
#include "mbluetoothdevicemanager.h"
#include "mlistmodel.h"
#include "mnetworkprinter.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    qmlRegisterType<MBluetooth>("com.mofing.bluetooth", 1, 0, "MBluetooth");
    qmlRegisterType<MBluetoothDeviceManager>("com.mofing.bluetooth", 1, 0, "MBluetoothDeviceManager");
    qmlRegisterType<MBluetoothDevice>("com.mofing.bluetooth", 1, 0, "MBluetoothDevice");
    qmlRegisterType<MListModel>("com.mofing.bluetooth", 1, 0, "MListModel");
    qmlRegisterType<MNetworkPrinter>("com.mofing.bluetooth", 1, 0, "MNetworkPrinter");

    engine.load(QUrl(QLatin1String("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}

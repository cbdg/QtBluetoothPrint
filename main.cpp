#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "mprintdevicemanager.h"
#include "mlistmodel.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    qmlRegisterType<MPrintDeviceManager>("com.mofing.bluetooth", 1, 0, "MPrintDeviceManager");
    qmlRegisterType<MPrintDevice>("com.mofing.bluetooth", 1, 0, "MPrintDevice");
    qmlRegisterType<MListModel>("com.mofing.bluetooth", 1, 0, "MListModel");

    engine.load(QUrl(QLatin1String("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}

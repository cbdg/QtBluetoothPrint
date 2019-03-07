import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import com.mofing.bluetooth 1.0

ApplicationWindow {
    id: appWindow
    visible: true
    width: 640
    height: 480
    title: qsTr("Hello World")
    color: "#d7d7d7"

    function showDevicePage(deviceObj)
    {
        var cmp = Qt.createComponent("DevicePage.qml");
        var qmlPage = cmp.createObject(appWindow, {"btManager": bt, "btDevice":deviceObj})

        //        appWindow.focus = false;
        qmlPage.focus = true;
    }

    function showBluetoothPage()
    {
        var cmp = Qt.createComponent("MBluetoothList.qml");
        var qmlPage = cmp.createObject(appWindow, {"bt": bt})

        qmlPage.focus = true;
    }

    function showWifiPage()
    {
        var cmp = Qt.createComponent("WifiPage.qml");
        var qmlPage = cmp.createObject(appWindow, {"bt": bt})

        qmlPage.focus = true;
    }

    function showReceiptsPage()
    {
        var cmp = Qt.createComponent("ReceiptsPage.qml");
        var qmlPage = cmp.createObject(appWindow, {"bt": bt})

        qmlPage.focus = true;
    }

    MBluetoothDeviceManager {
        id: bt
    }

    Flickable {
        anchors.fill: parent

        contentWidth: width;
        contentHeight: col.height

        Column {
            id: col
            width: parent.width
            // localdevice
            Column {
                width: parent.width

                Rectangle {
                    width: parent.width
                    height: 50;

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            showBluetoothPage();
                        }
                    }

                    Row {
                        anchors.left: parent.left
                        anchors.leftMargin: 16
                        anchors.verticalCenter: parent.verticalCenter;
                        height: parent.height
                        spacing: 10
                        Rectangle{
                            height: 20
                            width: height
                            anchors.verticalCenter: parent.verticalCenter;
                            radius: height/2;
                            color: "white";
                            border.width: bt.printType == 1 ? height/4 : 1;
                            border.color: bt.printType == 1 ? "#ffad00" : "gray";

                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    bt.printType = 1
                                }
                            }
                        }

                        Text {
                            anchors.verticalCenter: parent.verticalCenter
                            text: qsTr("蓝牙打印机")


                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    bt.printType = 1
                                }
                            }
                        }
                    }

                    Text {
                        anchors.right: parent.right
                        anchors.rightMargin: 16
                        anchors.verticalCenter: parent.verticalCenter
                        text: qsTr("%1  >").arg(bt.bluetoothName)
                    }

                    Rectangle {
                        width: parent.width
                        height: 1
                        color: "#d7d7d7"
                        anchors.bottom: parent.bottom
                    }
                }

                Rectangle {
                    width: parent.width
                    height: 50

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            showWifiPage();
                        }
                    }

                    Row {
                        anchors.left: parent.left
                        anchors.leftMargin: 16
                        anchors.verticalCenter: parent.verticalCenter;
                        height: parent.height
                        spacing: 10
                        Rectangle{
                            height: 20;
                            width: height;
                            anchors.verticalCenter: parent.verticalCenter;
                            radius: height/2;
                            color: "white";
                            border.width: bt.printType == 2 ? height/4 : 1;
                            border.color: bt.printType == 2 ? "#ffad00" : "gray";

                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    bt.printType = 2
                                }
                            }
                        }
                        Text {
                            anchors.verticalCenter: parent.verticalCenter
                            text: qsTr("WIFI 打印机")
                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    bt.printType = 2
                                }
                            }
                        }
                    }



                    Text {
                        anchors.right: parent.right
                        anchors.rightMargin: 16
                        anchors.verticalCenter: parent.verticalCenter
                        text: qsTr("%1  >").arg(bt.ipAddress)
                    }

                    Rectangle {
                        width: parent.width
                        height: 1
                        color: "#d7d7d7"
                        anchors.bottom: parent.bottom
                    }
                }

                // spac
                Item {
                    width: 1
                    height: 20
                }

                Rectangle {
                    width: parent.width
                    height: 50

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            showReceiptsPage();
                        }
                    }

                    Text {
                        anchors.left: parent.left
                        anchors.leftMargin: 16;
                        anchors.verticalCenter: parent.verticalCenter
                        text: qsTr("打印小票设置")
                    }

                    Text {
                        anchors.right: parent.right
                        anchors.rightMargin: 16;
                        anchors.verticalCenter: parent.verticalCenter
                        text: bt.receiptsType == 0 ? (qsTr("32英文/行  >")) : (bt.receiptsType == 1 ? (qsTr("40英文/行  >")) : (qsTr("48英文/行  >")))
                    }

                    Rectangle {
                        width: parent.width
                        height: 1
                        color: "#d7d7d7"
                        anchors.bottom: parent.bottom
                    }
                }

                // spac
                Item {
                    width: 1
                    height: 20
                }

                Rectangle {
                    width: parent.width
                    height: 50

                    Text {
                        anchors.centerIn: parent
                        text: qsTr("打印测试")
                        color: bt.currentDevice ? ( bt.currentDevice.isConnected ? "black" : "#d7d7d7") :  "#d7d7d7"
                    }

                    Rectangle {
                        width: parent.width
                        height: 1
                        color: "#d7d7d7"
                        anchors.bottom: parent.bottom
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            bt.printTest();
                        }
                    }
                }
            }
        }
    }
}

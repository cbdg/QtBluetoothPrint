import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import com.mofing.bluetooth 1.0

Rectangle {
    id: wifiPage
    property MBluetoothDeviceManager bt: null
    property MBluetoothDevice btDevice: null

    function exitPage()
    {
        wifiPage.focus = false
        wifiPage.parent.focus = true
        wifiPage.visible = false
        wifiPage.destroy();
    }    

    function showInputPage(type, value)
    {
        var cmp = Qt.createComponent("InputPage.qml");
        var qmlPage = cmp.createObject(wifiPage, {"bt": bt, "isIP": type == 1, "defaultValue": value})
        qmlPage.inputFinished.connect(onInputFinished)

        qmlPage.focus = true;
    }

    function onInputFinished(isIP, value)
    {
        if (isIP) {
            bt.ipAddress = value
        } else {
            bt.port = value
        }
    }

    width: parent.width
    height: parent.height
    color: "#d7d7d7"

    MouseArea {
        anchors.fill: parent
        onPressed: mouse.accepted = true
        onReleased: mouse.accepted = true
        onClicked: mouse.accepted = true
        onWheel: wheel.accepted = true
    }

    Keys.onReleased: {
        event.accepted = true
        if (event.key == Qt.Key_Escape || event.key == Qt.Key_Back) {
            exitPage();
        }

    }

    Flickable {
        anchors.fill: parent

        contentWidth: width;
        contentHeight: childrenRect.height

        Column {
            width: parent.width

            Rectangle {
                width: parent.width
                height: 50

                MouseArea {
                    anchors.fill: parent
                    anchors.margins: 5
                    onClicked: {
                        showInputPage(1, bt.ipAddress)
                    }
                }

                Rectangle {
                    width: parent.width
                    height: 1
                    color: "#d7d7d7"
                    anchors.bottom: parent.bottom
                }

                Row {
                    anchors.fill: parent
                    anchors.leftMargin: 16;
                    anchors.rightMargin: 16;
                    spacing: 10;

                    Text {
                        anchors.verticalCenter: parent.verticalCenter
                        text: qsTr("IP 地址：")
                    }

                    Text {
                        anchors.verticalCenter: parent.verticalCenter
                        text: bt.ipAddress
                    }
                }
                Text {
                    anchors.right: parent.right
                    anchors.rightMargin: 16;
                    anchors.verticalCenter: parent.verticalCenter
                    text: qsTr(">")
                }
            }

            Rectangle {
                width: parent.width
                height: 50

                MouseArea {
                    anchors.fill: parent
                    anchors.margins: 5
                    onClicked: {
                        showInputPage(2, bt.port)
                    }
                }

                Rectangle {
                    width: parent.width
                    height: 1
                    color: "#d7d7d7"
                    anchors.bottom: parent.bottom
                }

                Row {
                    anchors.fill: parent
                    anchors.leftMargin: 16;
                    anchors.rightMargin: 16;
                    spacing: 10;

                    Text {
                        anchors.verticalCenter: parent.verticalCenter
                        text: qsTr("端     口：")
                    }

                    Text {
                        anchors.verticalCenter: parent.verticalCenter
                        text: bt.port
                    }

                }
                Text {
                    anchors.right: parent.right
                    anchors.rightMargin: 16;
                    anchors.verticalCenter: parent.verticalCenter
                    text: qsTr(">")
                }
            }

            Rectangle {
                width: parent.width
                height: 50

                Rectangle {
                    width: parent.width
                    height: 1
                    color: "#d7d7d7"
                    anchors.bottom: parent.bottom
                }

                Text {
                    anchors.left: parent.left
                    anchors.leftMargin: 16
                    anchors.verticalCenter: parent.verticalCenter
                    text: qsTr("连接打印机")
                    color: bt.wifiDevice ? (bt.wifiDevice.isConnected ? "#d7d7d7" : "black") :  "black"
                }

                BusyIndicator {
                    anchors.right: parent.right
                    anchors.rightMargin: 16
                    anchors.verticalCenter: parent.verticalCenter
                    width: 30
                    height: width
                    visible: running
                    running: bt.wifiDevice ? (bt.wifiDevice.socketConnectState == MBluetoothDevice.Connecting ? true : false) : false
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        console.log("========================>>>>>>>>1", bt.wifiDevice, bt.wifiDevice.isConnected)
                        bt.currentDevice = bt.wifiDevice;
                    }
                }
            }

            Rectangle {
                width: parent.width
                height: 50

                Rectangle {
                    width: parent.width
                    height: 1
                    color: "#d7d7d7"
                    anchors.bottom: parent.bottom
                }

                Text {
                    anchors.left: parent.left
                    anchors.leftMargin: 16
                    anchors.verticalCenter: parent.verticalCenter
                    text: qsTr("打印测试")
                    color: bt.wifiDevice ? ( (bt.wifiDevice.isConnected) ? "black" : "#d7d7d7") :  "#d7d7d7"
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        if (bt.wifiDevice)
                            bt.printTest();
                    }
                }
            }
        }
    }
}

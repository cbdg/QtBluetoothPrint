import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import com.mofing.bluetooth 1.0

Rectangle {
    id: devicePage
    property MBluetoothDeviceManager btManager: null
    property MBluetoothDevice btDevice: null

    function exitPage()
    {
        devicePage.focus = false
        devicePage.visible = false
        devicePage.destroy();
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

                Rectangle {
                    width: parent.width
                    height: 1
                    color: "#d7d7d7"
                    anchors.bottom: parent.bottom
                }

                //                Text {
                //                    anchors.left: parent.left
                //                    anchors.leftMargin: 16
                //                    anchors.verticalCenter: parent.verticalCenter
                //                    text: qsTr("%1(%2)").arg(btDevice.deviceName).arg(Qt.platform.os == "ios" ? btDevice.uuid : btDevice.address)
                //                }
                Row {
                    anchors.fill: parent
                    anchors.leftMargin: 16;
                    anchors.rightMargin: 16;
                    spacing: 10;

                    Image {
                        anchors.verticalCenter: parent.verticalCenter
                        height: 20
                        width: height
                        source: deviceIcon(btDevice.majorDeviceClass, btDevice.minorDeviceClass)
                    }

                    Text {
                        anchors.verticalCenter: parent.verticalCenter
                        text: qsTr("%1(%2)").arg(btDevice.deviceName).arg(Qt.platform.os == "ios" ? btDevice.uuid : btDevice.address)
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
                    text: qsTr("连接打印机")
                    color: btDevice.isConnected ? "#d7d7d7" : "black"
                }

                BusyIndicator {
                    anchors.right: parent.right
                    anchors.rightMargin: 16
                    anchors.verticalCenter: parent.verticalCenter
                    width: 30
                    height: width
                    visible: running
                    running: btDevice.socketConnectState == MBluetoothDevice.Connecting
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        if (!btDevice.isConnected) {
                            btManager.setPrintDevice(btDevice)
                        }
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
                    color: btDevice.isConnected ? "black" : "#d7d7d7"
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        if (btDevice.isConnected)
                            btManager.printTest();
                    }
                }
            }
        }
    }
}

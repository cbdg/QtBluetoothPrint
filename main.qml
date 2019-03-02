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

    MBluetoothDeviceManager {
        id: bt
    }

    Flickable {
        anchors.fill: parent

        contentWidth: width;
        contentHeight: childrenRect.height

        Column {
            width: parent.width
            // localdevice
            Column {
                width: parent.width

                Rectangle {
                    width: parent.width
                    height: 50;

                    Text {
                        anchors.left: parent.left
                        anchors.leftMargin: 16
                        anchors.verticalCenter: parent.verticalCenter
                        text: qsTr("开启蓝牙")
                    }

                    Rectangle {
                        anchors.right: parent.right
                        anchors.rightMargin: 16
                        anchors.verticalCenter: parent.verticalCenter
                        width: 40
                        height: 20
                        radius: height / 2
                        color: bt.localDeviceStatus ? "#3080d9" : "#d7d7d7"

                        Rectangle {
                            id: flagRec
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.margins: 3
                            width: 15
                            height: 15
                            radius: height / 2
                        }

                        states: [
                            State {
                                when: bt.localDeviceStatus
                                AnchorChanges {
                                    target: flagRec
                                    anchors.left: undefined
                                    anchors.right: flagRec.parent.right
                                }
                            },
                            State {
                                when: !bt.localDeviceStatus
                                AnchorChanges {
                                    target: flagRec
                                    anchors.right: undefined
                                    anchors.left: flagRec.parent.left
                                }
                            }
                        ]

                        transitions: [
                            Transition {
                                AnchorAnimation {duration: 250}
                            }
                        ]

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                bt.switchLocalDeviceHostmodel();
                            }
                        }
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

                    Text {
                        anchors.left: parent.left
                        anchors.leftMargin: 16
                        anchors.verticalCenter: parent.verticalCenter
                        text: qsTr("手机名称")
                    }

                    Text {
                        anchors.right: parent.right
                        anchors.rightMargin: 16
                        anchors.verticalCenter: parent.verticalCenter

                        text: qsTr("%1 >").arg(bt.localDevice)
                        color: "#d7d7d7"
                    }

                    Rectangle {
                        width: parent.width
                        height: 1
                        color: "#d7d7d7"
                        anchors.bottom: parent.bottom
                    }
                }
            }


            // spac
             Item {
                 width: 1
                 height: 20
             }

             Rectangle {
                 width: parent.width
                 height: 40
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
                     text: qsTr("已配对设备")
                     color: "#d7d7d7"
                 }
              }

             Column {
                 width: parent.width

                 Repeater {
                     id: pairedRepeater

                     model: bt.pairedDevice()
                     delegate: Rectangle {
                         width: parent.width
                         height: 50
                         MouseArea {
                             anchors.fill: parent
                             onClicked: {
                                 //bt.sendFileToRemote(modelData)
                                 //bt.pairingToRemote(modelData)
                                 //bt.connectPrinter(modelData)
                                 //bt.connectPrinterWithSocket(modelData)
                                 showDevicePage(modelData)
                             }
                         }

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
                             text:qsTr("%1(%2)").arg(modelData.deviceName).arg(modelData.address)
                         }
                     }
                 }
             }

           // spac
            Item {
                width: 1
                height: 20
            }

            Rectangle {
                width: parent.width
                height: 40
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
                    text: qsTr("可用设备")
                    color: "#d7d7d7"
                }

                Row {
                    anchors.right: parent.right
                    anchors.rightMargin: 16
                    anchors.verticalCenter: parent.verticalCenter;

                    BusyIndicator {
                        anchors.verticalCenter: parent.verticalCenter
                        width: 30
                        height: width
                        visible: running
                        running: bt.discoverIsActive
                    }

                    Text {
                        anchors.verticalCenter: parent.verticalCenter
                        text: qsTr("刷新设备")

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                bt.startDiscoverDevice();
                            }
                        }
                    }
                }
            }

            Column {
                width: parent.width

                Repeater {
                    id: connectRepeater

                    model: bt.remoteDevice()
                    delegate: Rectangle {
                        width: parent.width
                        height: 50
                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                //bt.sendFileToRemote(modelData)
                                bt.pairingToRemote(modelData)
                            }
                        }

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
                            text: qsTr("%1(%2)").arg(modelData.deviceName).arg(Qt.platform.os == "ios" ? modelData.uuid : modelData.address)
                        }
                    }
                }
            }
        }
    }

    Component.onCompleted: {
        bt.startDiscoverDevice();
    }
}

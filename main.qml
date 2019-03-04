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

//                         Text {
//                             anchors.left: parent.left
//                             anchors.leftMargin: 16
//                             anchors.verticalCenter: parent.verticalCenter
//                             text:qsTr("%1(%2)").arg(modelData.deviceName).arg(modelData.address)
//                         }
                         Row {
                             anchors.left: parent.left
                             anchors.leftMargin: 16;
                             anchors.verticalCenter: parent.verticalCenter
                             spacing: 10;

                             Image {
                                 anchors.verticalCenter: parent.verticalCenter
                                 height: 20
                                 width: height
                                 source: deviceIcon(modelData.majorDeviceClass, modelData.minorDeviceClass)
                             }

                             Text {
                                 anchors.verticalCenter: parent.verticalCenter
                                 text: qsTr("%1(%2)(Major %3,Minor %4)")
                                 .arg(modelData.deviceName)
                                 .arg(Qt.platform.os == "ios" ? modelData.uuid : modelData.address)
                                 .arg(modelData.majorDeviceClass).arg(modelData.minorDeviceClass)
                             }
                         }

                         Image {
                             anchors.right: parent.right
                             anchors.rightMargin: 16;
                             anchors.verticalCenter: parent.verticalCenter
                             width: 20
                             height: width
                             source: "qrc:/correct.png"
                             visible: modelData.isConnected
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

                        Row {
                            anchors.fill: parent
                            anchors.leftMargin: 16;
                            anchors.rightMargin: 16;
                            spacing: 10;

                            Image {
                                anchors.verticalCenter: parent.verticalCenter
                                height: 20
                                width: height
                                source: deviceIcon(modelData.majorDeviceClass, modelData.minorDeviceClass)
                            }

                            Text {
                                anchors.verticalCenter: parent.verticalCenter
                                text: qsTr("%1(%2)(Major %3,Minor %4)")
                                .arg(modelData.deviceName)
                                .arg(Qt.platform.os == "ios" ? modelData.uuid : modelData.address)
                                .arg(modelData.majorDeviceClass).arg(modelData.minorDeviceClass)
                            }
                        }
                    }
                }
            }
        }
    }

    function deviceIcon(majorDeviceClass, minorDeviceClass)
    {
        var iconStr = "qrc:/bluetooth/buletoth.png";
        switch (majorDeviceClass) {
        case 0://QBluetoothDeviceInfo::MiscellaneousDevice:
            iconStr = "qrc:/bluetooth/buletoth.png";
            break;
        case 1://QBluetoothDeviceInfo::ComputerDevice
            iconStr = "qrc:/bluetooth/computer.png";
            break;
        case 2://QBluetoothDeviceInfo::PhoneDevice
            iconStr = "qrc:/bluetooth/mobilephone.png";
            break;
        case 3://QBluetoothDeviceInfo::LANAccessDevice
            iconStr = "qrc:/bluetooth/buletoth.png";
            break;
        case 4://QBluetoothDeviceInfo::AudioVideoDevice
            iconStr = "qrc:/bluetooth/buletoth.png";
            switch (minorDeviceClass) {
            case 4://QBluetoothDeviceInfo::Microphone
                iconStr = "qrc:/bluetooth/micphone.png";
                break;
            case 5://QBluetoothDeviceInfo::Loudspeaker
                iconStr = "qrc:/bluetooth/speaker.png";
                break;
            case 6://QBluetoothDeviceInfo::Headphones
                iconStr = "qrc:/bluetooth/headmusic.png";
                break;
            case 8://QBluetoothDeviceInfo::CarAudio
                iconStr = "qrc:/bluetooth/car.png";
                break;
            }
            break;
        case 5://QBluetoothDeviceInfo::PeripheralDevice
            iconStr = "qrc:/bluetooth/keyborder.png";
            switch (minorDeviceClass) {
            case 0x10://QBluetoothDeviceInfo::KeyboardPeripheral
            case 0x30://QBluetoothDeviceInfo::KeyboardWithPointingDevicePeripheral
                iconStr = "qrc:/bluetooth/keyborder.png";
                break;
            case 0x20://QBluetoothDeviceInfo::PointingDevicePeripheral
                iconStr = "qrc:/bluetooth/mouse.png";
                break;
            }
            break;
        case 6://QBluetoothDeviceInfo::ImagingDevice
            iconStr = "qrc:/bluetooth/printer.png";
            switch (minorDeviceClass) {
            case 0x20://QBluetoothDeviceInfo::ImagePrinter
                iconStr = "qrc:/bluetooth/printer.png";
                break;
            }
            break;
        case 7://QBluetoothDeviceInfo::WearableDevice
            iconStr = "qrc:/bluetooth/wathch.png";
            break;
        case 8://QBluetoothDeviceInfo::ToyDevice
            iconStr = "qrc:/bluetooth/buletoth.png";
            break;
        case 9://QBluetoothDeviceInfo::HealthDevice
            iconStr = "qrc:/bluetooth/buletoth.png";
            break;
        case 31://QBluetoothDeviceInfo::UncategorizedDevice
            iconStr = "qrc:/bluetooth/buletoth.png";
            break;
        default:
            iconStr = "qrc:/bluetooth/buletoth.png";
            break;

        }

        return iconStr;
    }

    Component.onCompleted: {
        bt.startDiscoverDevice();
    }
}

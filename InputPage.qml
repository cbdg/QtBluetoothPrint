import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import com.mofing.bluetooth 1.0

Rectangle {
    id: inputPage
    property MPrintDeviceManager btManager: null
    property MPrintDevice btDevice: null
    property bool isIP: true
    property string defaultValue: ""

    signal inputFinished(bool type, string value)

    function exitPage()
    {
        inputPage.focus = false
        inputPage.parent.focus = true
        inputPage.visible = false
        inputPage.destroy();
    }

    width: parent.width
    height: parent.height
    color: "#80000000"

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

    Rectangle {
        anchors.centerIn: parent

        width: parent.width * 0.8
        height: childrenRect.height
        radius: 10

        Column {
            width: parent.width
            spacing: 10

            Item {
                width: 1
                height: 1
            }

            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text: isIP ? qsTr("请输入 IP 地址") : qsTr("请输入端口")
            }

            Rectangle{
                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width * 0.8
                height: 40
                border.width: 1
                border.color: "#d7d7d7"
                radius: 5
                TextInput {
                    id: txtInput
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.margins: 5
                    anchors.verticalCenter: parent.verticalCenter
                    inputMethodHints:  Qt.ImhPreferNumbers
                    text: defaultValue
                    validator: RegExpValidator { regExp: isIP ? /((25[0-5]|2[0-4]\d|((1\d{2})|([1-9]?\d)))\.){3}(25[0-5]|2[0-4]\d|((1\d{2})|([1-9]?\d)))/
                    : /[0-9]|[1-9]\d{1,3}|[1-5]\d{4}|6[0-4]\d{4}|65[0-4]\d{2}|655[0-2]\d|6553[0-5]/}
                }
            }

            Row {
                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width * 0.8
                height: 40
                spacing: parent.width * 0.1

                Rectangle {
                    width: parent.width * 0.45
                    height: parent.height
                    radius: 10
                    color: "#ffad00"

                    Text {
                        anchors.centerIn: parent
                        text: qsTr("取消")
                        color: "white"
                    }
                    MouseArea {
                        anchors.fill: parent
                        anchors.margins: 5
                        onClicked: {
                            exitPage()
                        }
                    }
                }

                Rectangle {
                    width: parent.width * 0.45
                    height: parent.height
                    radius: 10
                    color: "#ffad00"

                    Text {
                        anchors.centerIn: parent
                        text: qsTr("确定")
                        color: "white"
                    }


                    MouseArea {
                        anchors.fill: parent
                        anchors.margins: 5
                        onClicked: {
                            console.log("=========================>>>>>txtInput", txtInput.text)
                            inputFinished(isIP, txtInput.text)
                            exitPage()
                        }
                    }
                }
            }
            Item {
                width: 1
                height: 1
            }
        }
    }
}

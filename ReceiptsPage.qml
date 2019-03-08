import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import com.mofing.bluetooth 1.0

Rectangle {
    id: receiptsPage
    property MPrintDeviceManager bt: null
    property MPrintDevice btDevice: null
    property int receiptsCurrentType: 0

    function exitPage()
    {
        receiptsPage.focus = false
        receiptsPage.parent.focus = true
        receiptsPage.visible = false
        receiptsPage.destroy();
    }    

    function initModel()
    {
        receiptsModel.append({"txt":qsTr("32英文(16汉字)/行")})
        receiptsModel.append({"txt":qsTr("40英文(20汉字)/行")})
        receiptsModel.append({"txt":qsTr("48英文(24汉字)/行")})
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
            Repeater {

                model: ListModel{
                   id: receiptsModel
                }
                delegate: Rectangle {
                    width: parent.width
                    height: 50;

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            bt.receiptsType = index
                        }
                    }

                    Row {
                        anchors.left: parent.left
                        anchors.leftMargin: 16
                        anchors.verticalCenter: parent.verticalCenter;
                        spacing: 10
                        Rectangle{
                            height: 20
                            width: height
                            anchors.verticalCenter: parent.verticalCenter;
                            radius: height/2;
                            color: "white";
                            border.width: bt.receiptsType == index ? height/4 : 1;
                            border.color: bt.receiptsType == index ? "#ffad00" : "gray";

                        }

                        Text {
                            anchors.verticalCenter: parent.verticalCenter
                            text: txt
                        }
                    }

                    Rectangle {
                        width: parent.width
                        height: 1
                        color: "#d7d7d7"
                        anchors.bottom: parent.bottom
                    }
                }
            }
        }
    }

    Component.onCompleted: {
        initModel();
    }
}

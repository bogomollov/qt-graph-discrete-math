import QtQuick

Rectangle {
    id: root

    color: "#ffffff"

    ListModel {
        id: vertices
    }

    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton
        cursorShape: Qt.CrossCursor

        onClicked: function(mouse) {
            vertices.append({ "posX": mouse.x, "posY": mouse.y })
        }
    }

    Repeater {
        model: vertices

        delegate: Rectangle {
            width: 64
            height: 64
            radius: width / 2
            color: "#d9ffc2"
            border.color: "#abfb79"
            border.width: 2
            x: model.posX - width / 2
            y: model.posY - height / 2
        }
    }
}

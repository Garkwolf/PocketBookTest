import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12

ApplicationWindow {
    visible: true
    width: 640
    height: 480
    title: qsTr("File Processor")

    ListView {
        id: fileListView
        anchors.fill: parent
        model: fileModel
        delegate: Item {
            width: ListView.view.width
            height: 40
            GridLayout {
                columns: 3
                columnSpacing: 10
                ColumnLayout {
                    Text {
                        text: name
                        Layout.fillWidth: true
                        Layout.alignment: Qt.AlignLeft
                    }
                }
                ColumnLayout {
                    Text {
                        text: size + " bytes"
                        Layout.alignment: Qt.AlignRight
                        Layout.preferredWidth: 100
                    }
                }
                ColumnLayout {
                    Text {
                        text: status
                        Layout.alignment: Qt.AlignRight
                        Layout.preferredWidth: 100
                    }
                }
            }
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    if (type === "bmp" || type === "barch") {
                        fileProcessor.processFile(path)
                    } else {
                        errorDialog.message = "Unknown file type"
                        errorDialog.visible = true
                    }
                }
            }
        }
    }

    Component.onCompleted: {
        fileProcessor.fileProcessed.connect((filePath, success, message) => {
            if (!success) {
                errorDialog.message = message
                errorDialog.visible = true
            }
        });
    }

    Dialog {
        id: errorDialog
        visible: false
        modal: true
        title: "Error"
        standardButtons: Dialog.Ok
        property string message: ""
        onAccepted: {
            errorDialog.visible = false
        }
        contentItem: Column {
            spacing: 10
            Text {
                text: errorDialog.message
                wrapMode: Text.WordWrap
                width: parent.width
            }
        }
    }
}

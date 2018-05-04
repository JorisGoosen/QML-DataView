import QtQuick 2.10
import QtQuick.Window 2.10
import QtQuick.Controls 2.2
import JASP 1.0

Window {
	visible: true
	width: 640
	height: 480
	title: qsTr("Hello World")
	id: theWindow

	Item
	{

		anchors.fill: parent

		JASPDataView
		{


			anchors.top: parent.top
			anchors.left: parent.left
			anchors.right: parent.right
			anchors.bottom: buttons.top

			model: myStupidModel
		}

		Item
		{
			id: buttons
			height: 20
			anchors.left: parent.left
			anchors.right: parent.right
			anchors.bottom: parent.bottom

			Button
			{
				id: randomShit
				onClicked: myStupidModel.randomShit()

				text: "random shit!"
				anchors.top: parent.top
				anchors.left: parent.left
				anchors.right: parent.horizontalCenter
				anchors.bottom: parent.bottom
			}

			Button
			{
				id: resetMe
				onClicked: myStupidModel.resetMe()

				text: "reset!"
				anchors.top: parent.top
				anchors.left: parent.horizontalCenter
				anchors.right: parent.right
				anchors.bottom: parent.bottom
			}
		}

	}
}


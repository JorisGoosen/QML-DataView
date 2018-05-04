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

		Flickable
		{
			id: myFlickable
			Keys.onEscapePressed: theWindow.close()

			anchors.top: parent.top
			anchors.left: parent.left
			anchors.right: parent.right
			anchors.bottom: buttons.top

			contentWidth: myStupidView.width
			contentHeight: myStupidView.height

			clip: true

			DataSetView
			{
				z: -1
				Keys.onEscapePressed: theWindow.close()
				id: myStupidView
				model: myStupidModel

				viewportX: myFlickable.visibleArea.xPosition * width
				viewportY: myFlickable.visibleArea.yPosition * height
				viewportW: myFlickable.visibleArea.widthRatio * width
				viewportH: myFlickable.visibleArea.heightRatio * height

			}


			ScrollBar.vertical: ScrollBar { z:2}
			ScrollBar.horizontal: ScrollBar { z:2}

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


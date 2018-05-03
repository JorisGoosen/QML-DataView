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

	Flickable
	{
		id: myFlickable
		Keys.onEscapePressed: theWindow.close()

		anchors.fill: parent

		contentWidth: myStupidView.width
		contentHeight: myStupidView.height

		clip: true

		DataSetView
		{
			z: -1
			Keys.onEscapePressed: theWindow.close()
			id: myStupidView
			model: myStupidModel

			viewportX: myFlickable.visibleArea.xPosition
			viewportY: myFlickable.visibleArea.yPosition
			viewportW: myFlickable.visibleArea.widthRatio
			viewportH: myFlickable.visibleArea.heightRatio

			//color: "pink"
			//width: 1000
			//height: 1000
			//text: "aaaaaaa"

			//anchors.fill: parent

			rowNumberDelegate:
			Item
			{
				property alias text: tekst.text

				Rectangle
				{
					z: -1
					border.color: "red"
					border.width: 1
					color: "yellow"

					width: parent.width
					height: parent.height + 1
				}

				Text
				{
					id: tekst
					color: "blue"
					anchors.centerIn: parent
				}
			}

		}

		ScrollBar.vertical: ScrollBar { z:2}
		ScrollBar.horizontal: ScrollBar { z:2}
	}
}


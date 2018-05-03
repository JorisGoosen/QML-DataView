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
		}

		ScrollBar.vertical: ScrollBar { z:2}
		ScrollBar.horizontal: ScrollBar { z:2}
	}
}


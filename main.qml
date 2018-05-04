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

	/*Item
	{
		id: backRect
		//color: "purple"

		z: -1*/

		Flickable
		{
			id: myFlickable
			Keys.onEscapePressed: theWindow.close()

			anchors.fill: parent

			contentWidth: myStupidView.width
			contentHeight: myStupidView.height

			clip: true



			/*MouseArea
			{
				anchors.fill: parent
				onClicked: backRect.color = "black"
				z: 5
*/
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



				/*rowNumberDelegate:
				Item
				{
					property alias text: tekst.text

					Button
					{
						id: tekst

						width: parent.width
						height: parent.height + 1

						property color myColor: "yellow"

						onClicked:
						{
							myColor = "blue"

						}

						background: Rectangle
						{
							id: myBackground
							z: 0
							border.color: "red"
							border.width: 1
							color: tekst.myColor
						}

						//preventStealing: true
					}
				}*/

			}
		//	}

			ScrollBar.vertical: ScrollBar { z:2}
			ScrollBar.horizontal: ScrollBar { z:2}

	}
}


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

	ScrollView
	{
		Keys.onEscapePressed: theWindow.close()

		anchors.fill:parent
		contentWidth: myStupidView.width
		contentHeight: myStupidView.height

		DataSetView
		{
			Keys.onEscapePressed: theWindow.close()
			id: myStupidView
			model: myStupidModel
			//color: "pink"
			//width: 1000
			//height: 1000
			//text: "aaaaaaa"

			//anchors.fill: parent
		}
	}
}

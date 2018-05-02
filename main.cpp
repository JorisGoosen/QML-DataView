#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "stupidmodel.h"
#include "datasetview.h"

int main(int argc, char *argv[])
{
	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

	QGuiApplication app(argc, argv);

	StupidModel myStupidModel;
	qmlRegisterType<DataSetView>("JASP", 1, 0, "DataSetView");

	QQmlApplicationEngine engine;
	engine.rootContext()->setContextProperty("myStupidModel", &myStupidModel);
	engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
	if (engine.rootObjects().isEmpty())
		return -1;

	return app.exec();
}

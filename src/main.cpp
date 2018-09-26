#include "mainwindow.h"
#include <QApplication>
#include <QSurfaceFormat>


int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	MainWindow w;

	QSurfaceFormat format;
	format.setDepthBufferSize(24);
	format.setVersion(3, 1);
	format.setProfile(QSurfaceFormat::CoreProfile);
	QSurfaceFormat::setDefaultFormat(format);

	w.show();

	return a.exec();
}

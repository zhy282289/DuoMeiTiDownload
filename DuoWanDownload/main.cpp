#include "stdafx.h"
#include "DuoWanDownload.h"
#include <QtWidgets/QApplication>

#include <QtWebEngineWidgets/QtWebEngineWidgets>

#include "DB.h"
int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	DB db;

	DuoWanDownload w;
	w.show();
	return a.exec();
}

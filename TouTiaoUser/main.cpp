#include "stdafx.h"
#include "TouTiaoUser.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);


	TouTiaoUser w;
	w.show();

	return a.exec();
}

#include "stdafx.h"
#include "TouTiaoDownload.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	if (!MY_DB->Init())
	{
		QMessageBox::information(0, TR("错误"), TR("初始化数据库失败，程序退出！"));
	}
	
	gInitPythonPath();

	TouTiaoDownload w;
	w.show();

	return a.exec();
}

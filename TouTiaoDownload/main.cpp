#include "stdafx.h"
#include "TouTiaoDownload.h"
#include <QtWidgets/QApplication>

#include "VideoInfo.h"


int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	if (!MY_DB->Init())
	{
		QMessageBox::information(0, TR("����"), TR("��ʼ�����ݿ�ʧ�ܣ������˳���"));
	}
	//MOUSEKEYBOARD_FORBID_ENABLED;
	//QThread::sleep(10);
	//MOUSEKEYBOARD_FORBID_DISABLED;
	TouTiaoDownload w;
	w.show();

	return a.exec();
}

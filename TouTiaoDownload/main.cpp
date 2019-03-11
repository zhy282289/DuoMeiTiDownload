#include "stdafx.h"
#include "TouTiaoDownload.h"
#include <QtWidgets/QApplication>

#include "VideoInfo.h"

#include "EmailSend.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	if (!MY_DB->Init())
	{
		QMessageBox::information(0, TR("����"), TR("��ʼ�����ݿ�ʧ�ܣ������˳���"));
	}
	
	EMAIL_NETWORKERROR->SendEmail();
	TouTiaoDownload w;
	w.show();

	return a.exec();
}

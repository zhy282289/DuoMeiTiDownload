#include "stdafx.h"
#include "TouTiaoDownload.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	if (!MY_DB->Init())
	{
		QMessageBox::information(0, TR("����"), TR("��ʼ�����ݿ�ʧ�ܣ������˳���"));
	}
	
	gInitPythonPath();

	TouTiaoDownload w;
	w.show();

	return a.exec();
}

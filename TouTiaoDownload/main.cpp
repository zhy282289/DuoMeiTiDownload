#include "stdafx.h"
#include "TouTiaoDownload.h"

LONG __stdcall MyUnhandledExceptionFilter(PEXCEPTION_POINTERS pExceptionInfo);


int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	if (!MY_DB->Init())
	{
		QMessageBox::information(0, TR("����"), TR("��ʼ�����ݿ�ʧ�ܣ������˳���"));
	}
	
	SetUnhandledExceptionFilter(MyUnhandledExceptionFilter);

	gInitPythonPath();

	TouTiaoDownload w;
	w.show();

	COMMNDLINEMANAGER->Parse();

	return a.exec();
}



LONG __stdcall MyUnhandledExceptionFilter(PEXCEPTION_POINTERS pExceptionInfo)
{
	auto type = COMMNDLINEMANAGER->GetType();
	if (type == CommndLineManager::NONE)
		return EXCEPTION_EXECUTE_HANDLER;

	QString batPath;
	if (type == CommndLineManager::DOWNLOAD)
	{
		batPath = QString(TR("2-��ʼ����.bat"));
	}
	else if (type == CommndLineManager::SCAN)
	{
		batPath = QString(TR("1-��ʼɨ��.bat"));
	}

	batPath = QString(TR("%1\\..\\..\\..\\bat\\%2")).arg(QApplication::applicationDirPath()).arg(batPath);
	batPath = QDir::toNativeSeparators(QFileInfo(batPath).absoluteFilePath());
	//QMessageBox::information(0, "", batPath);
	ShellExecute(NULL, L"open", batPath.toStdWString().c_str(), NULL, NULL, SW_SHOW);


	return EXCEPTION_EXECUTE_HANDLER;
}
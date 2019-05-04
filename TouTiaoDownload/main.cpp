#include "stdafx.h"
#include "TouTiaoDownload.h"

LONG __stdcall MyUnhandledExceptionFilter(PEXCEPTION_POINTERS pExceptionInfo);


int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	if (!MY_DB->Init())
	{
		QMessageBox::information(0, TR("错误"), TR("初始化数据库失败，程序退出！"));
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
		batPath = QString(TR("2-开始下载.bat"));
	}
	else if (type == CommndLineManager::SCAN)
	{
		batPath = QString(TR("1-开始扫描.bat"));
	}

	batPath = QString(TR("%1\\..\\..\\..\\bat\\%2")).arg(QApplication::applicationDirPath()).arg(batPath);
	batPath = QDir::toNativeSeparators(QFileInfo(batPath).absoluteFilePath());
	//QMessageBox::information(0, "", batPath);
	ShellExecute(NULL, L"open", batPath.toStdWString().c_str(), NULL, NULL, SW_SHOW);


	return EXCEPTION_EXECUTE_HANDLER;
}
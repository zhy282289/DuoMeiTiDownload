#include "stdafx.h"
#include "ExternalApi.h"

#include "inc_PyDll/IPython_Exe.h"

QString gGetTouTiaoResource()
{
	QString path = QDir::toNativeSeparators(QApplication::applicationDirPath() + "/toutiao");
	gMakeDirectory(path);
	return path;
}

QString gGetTouTiaoTodayPath()
{
	//QString path = gGetTouTiaoResource() + "\\" + QDateTime::currentDateTime().toString("yy-MM-dd");
	QString path = DownloadConfig::DownloadPath() + "\\" + QDateTime::currentDateTime().toString("yy-MM-dd");
	gMakeDirectory(path);

	return path;
}



QString gGetTouTiaoTodaySourcePath()
{
	QString path = QString("%1\\%2.mp4").arg(gGetTouTiaoTodayPath()).arg(QUuid::createUuid().toString());
	return QDir::toNativeSeparators(path);
}

QString gGetTouTiaoTodayDestePath(QString src)
{
	int pos = src.indexOf(".mp4", 0, Qt::CaseInsensitive);
	if (pos>0)
	{

		int i = 1;
		QString temp;
		do
		{
			temp = src;
			temp.insert(pos, QString("_%1").arg(i));
			if (!QFile::exists(temp))
				break;
		} while (++i);
		src = temp;
	}
	else
	{
		src += "_1.mp4";
	}
	return src;
}



QComboBox* gCreateVideoTypeComboBox(QWidget *parent)
{
	auto cmbVideoType = new QComboBox(parent);
	cmbVideoType->addItem(TR("推荐"), TuiJian);
	cmbVideoType->addItem(TR("影视"), YingShi);
	cmbVideoType->addItem(TR("小品"), YinYue);
	cmbVideoType->addItem(TR("时尚"), ShiShang);
	cmbVideoType->addItem(TR("育儿"), MeiShi);
	cmbVideoType->addItem(TR("文化"), WenHua);

	return cmbVideoType;
}

QString gGetVideoTypeString(int videoType)
{
	if (videoType == TuiJian)
		return TR("推荐");
	else if (videoType == YingShi)
		return TR("影视");
	else if (videoType == YinYue)
		return TR("小品");
	else if (videoType == ShiShang)
		return TR("时尚");
	else if (videoType == MeiShi)
		return TR("育儿");
	else if (videoType == WenHua)
		return TR("文化");

	return TR("未知");
}

void gKeybdEvent(char key)
{
	keybd_event(key, 0, 0, 0);
	keybd_event(key, 0, KEYEVENTF_KEYUP, 0);
}

void gKeybdEvent_CTL(char key)
{
	keybd_event(VK_CONTROL, 0, 0, 0);
	keybd_event(key, 0, 0, 0);
	keybd_event(key, 0, KEYEVENTF_KEYUP, 0);
	keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP, 0);

}

void gMoveCursorAndClick(QPoint point)
{
	QCursor::setPos(point);
	mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
	QThread::msleep(10);
	mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);

}

void gWebViewScrollBottom(QWebEnginePage *page, std::function<void(QVariant)> callback)
{
	page->runJavaScript(
		"window.scrollTo(0,document.body.scrollHeight);"
		, callback);
}

void gWebViewScrollTop(QWebEnginePage *page, std::function<void(QVariant)> callback)
{
	page->runJavaScript(
		"window.scrollTo(0,0);"
		, callback);
}

void gInitPythonPath()
{
	QString pyevnpath = QDir::toNativeSeparators(QApplication::applicationDirPath() + "/python");
	pyevnpath.replace('\\', '/');
	IPython_Exe::GetInstance()->AppendPyEnvPath(pyevnpath.toLocal8Bit().data());
}


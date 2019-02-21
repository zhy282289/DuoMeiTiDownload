#include "stdafx.h"
#include "ExternalApi.h"



QString gGetTouTiaoResource()
{
	QString path = QDir::toNativeSeparators(QApplication::applicationDirPath() + "/toutiao");
	gMakeDirectory(path);
	return path;
}

QString gGetTouTiaoTodayPath()
{
	QString path = gGetTouTiaoResource() + "\\" + QDateTime::currentDateTime().toString("yy-MM-dd");
	gMakeDirectory(path);

	return path;
}



QString gGetTouTiaoTodaySourcePath()
{
	QString path = QString("%1\\%2.mp4").arg(gGetTouTiaoTodayPath()).arg(QUuid::createUuid().toString());
	return path;
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
	cmbVideoType->addItem(TR("音乐"), YinYue);
	cmbVideoType->addItem(TR("时尚"), ShiShang);
	cmbVideoType->addItem(TR("美食"), MeiShi);
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
		return TR("音乐");
	else if (videoType == ShiShang)
		return TR("时尚");
	else if (videoType == MeiShi)
		return TR("美食");
	else if (videoType == WenHua)
		return TR("文化");

	return TR("未知");
}


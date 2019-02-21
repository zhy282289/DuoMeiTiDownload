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
	cmbVideoType->addItem(TR("�Ƽ�"), TuiJian);
	cmbVideoType->addItem(TR("Ӱ��"), YingShi);
	cmbVideoType->addItem(TR("����"), YinYue);
	cmbVideoType->addItem(TR("ʱ��"), ShiShang);
	cmbVideoType->addItem(TR("��ʳ"), MeiShi);
	cmbVideoType->addItem(TR("�Ļ�"), WenHua);

	return cmbVideoType;
}

QString gGetVideoTypeString(int videoType)
{
	if (videoType == TuiJian)
		return TR("�Ƽ�");
	else if (videoType == YingShi)
		return TR("Ӱ��");
	else if (videoType == YinYue)
		return TR("����");
	else if (videoType == ShiShang)
		return TR("ʱ��");
	else if (videoType == MeiShi)
		return TR("��ʳ");
	else if (videoType == WenHua)
		return TR("�Ļ�");

	return TR("δ֪");
}


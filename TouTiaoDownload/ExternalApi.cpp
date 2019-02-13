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
		src.insert(pos, "_dest");
	}
	else
	{
		src += "_dest.mp4";
	}
	return src;
}



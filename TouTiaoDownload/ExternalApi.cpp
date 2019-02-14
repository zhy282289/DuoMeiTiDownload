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



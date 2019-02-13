#include "stdafx.h"
#include "ExternalApi.h"


QString gGetGifPath()
{
	QString path = QDir::toNativeSeparators(QApplication::applicationDirPath() + "/gif");
	gMakeDirectory(path);
	return path;
}

QString gTodayPath()
{
	QString path = gGetGifPath() + "\\" + QDateTime::currentDateTime().toString("yy-MM-dd");
	gMakeDirectory(path);

	return path;
}

QString gTodayUrlsPath()
{
	QString path = gTodayPath() + "\\urls.json";
	return path;
}

QString gTodaySourceGifPath()
{
	QString path = gTodayPath() + "\\src";
	gMakeDirectory(path);
	return path;
}

QString gTodayDestGifPath()
{
	QString path = gTodayPath() + "\\dest";
	gMakeDirectory(path);
	return path;
}

#include "stdafx.h"
#include "TouTiaoDownload.h"
#include <QtWidgets/QApplication>

#include "VideoInfo.h"


void test();
int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	if (!MY_DB->Init())
	{
		QMessageBox::information(0, TR("错误"), TR("初始化数据库失败，程序退出！"));
	}

	TouTiaoDownload w;
	w.show();


	return a.exec();
}

#include "DownloadManager.h"
void test()
{
	//VideoInfo::GetMediaInfo("");
	QString src("E:\\zhy\\works\\qt\\DuoMeiTiDownload\\bin\\x64\\Debug\\ffmpeg\\11.mp4");
	QString dst("E:\\zhy\\works\\qt\\DuoMeiTiDownload\\bin\\x64\\Debug\\ffmpeg\\333.mp4");
	ConvertManager convert;
	convert.Convert(src, gGetTouTiaoTodayDestePath(src));
}
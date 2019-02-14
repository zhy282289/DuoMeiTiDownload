#include "stdafx.h"
#include "DownloadManager.h"

#include <thread>

#include "inc_PyDll/IPython_Exe.h"
#include "VideoInfo.h"

DownloadManager::DownloadManager(QObject *parent)
	: QObject(parent)
{
}

DownloadManager::~DownloadManager()
{

}

bool DownloadManager::Download(TaskInfoPtr info)
{

	auto DownloadFun = [](QString url, QString dstpath) ->bool
	{
		IPython_Exe *pyExe = IPython_Exe::GetInstance();
		bool ret = pyExe->Simple_Call("toutiao", "downloadFile", "(ss)", url.toUtf8().data(), dstpath.toUtf8().data());
		ret |= (bool)pyExe->ReturnInt();

		ret = QFile::exists(dstpath);

		return ret;
	};

	m_info = info;
	std::thread t([=]()
	{
		QString url = m_info->videoUrl;
		QString localPath = m_info->localPath;
		bool ret = false;
		if (!QFile::exists(localPath))
		{

			localPath = gGetTouTiaoTodaySourcePath();
			LOG(TR("��ʼ������Ƶ"));
			ret = DownloadFun(url, localPath);

			if (!ret)
			{
				LOG(TR("����ʧ�ܣ����»�ȡ���ص�ַ"));
				emit sigUpdateVideoUrl(info);
				if (url != info->videoUrl)
				{
					LOG(TR("��ȡ���ص�ַ�ɹ�����ʼ����"));
					ret = DownloadFun(info->videoUrl, localPath);
				}

			}
		}

		
		if (ret)
		{
			LOG(TR("���سɹ�"));
			Convert(localPath);
		}
		else
		{
			LOG(TR("����ʧ��"));
			Finish(ret);

		}
	});
	t.detach();

	return true;
}

bool DownloadManager::Convert(QString dstpath)
{
	LOG(TR("��ʼת��"));

	m_info->localPath = dstpath;
	QString dstpathTemp = gGetTouTiaoTodayDestePath(dstpath);
	ConvertManager convert;
	bool ret = convert.Convert(dstpath, dstpathTemp);
	if (ret)
		m_info->localPath = dstpathTemp;

	LOG(TR("ת�����"));

	Finish(true);

	return true;
}

void DownloadManager::Finish(bool code)
{
	emit sigFinish(true, m_info);
	deleteLater();
}

//////////////////////////////////////////////////////////////////////////
ConvertManager::ConvertManager(QObject *parent /*= nullptr*/)
	:QObject(parent)
{

}

bool ConvertManager::Convert(QString src, QString dst)
{
	auto vinfo = VideoInfo::GetMediaInfo(src);

	if (vinfo.vinfo.width <= 0 | vinfo.vinfo.height <= 0)
	{
		return false;
	}
	

	int width = vinfo.vinfo.width;
	int height = vinfo.vinfo.height;
	float ratio = 1.0f * width / height;
	int x = width - 200;
	if (x <= 0)
	{
		return false;

	}

	if (width > 600)
		width = 800;
	else
		width = 600;

	height = 1.0f * width / ratio;
	if (height % 2)
		height += 1;

	QString startTime = VideoInfo::SecondToQString(5);
	QString ffmpegPath = QApplication::applicationDirPath() + "/ffmpeg/ffmpeg.exe";
	QString cmd = QString("%1 -ss %7 -i %2 -vf delogo=x=%6:y=20:w=190:h=56,scale=%4:%5 -b:v %8 %3").
		arg(ffmpegPath).arg(src).arg(dst).arg(width).arg(height).
		arg(x).arg(startTime).arg(vinfo.bit_rate);

	bool ret = QProcess::execute(cmd) == 0;

	return ret;
}

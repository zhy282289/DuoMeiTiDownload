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

	auto DownloadFun = [](QString url, QString dstpath) ->int
	{
		IPython_Exe *pyExe = IPython_Exe::GetInstance();
		bool ret = pyExe->Simple_Call("toutiao", "downloadFile", "(ss)", url.toUtf8().data(), dstpath.toUtf8().data());
		ret |= (bool)pyExe->ReturnInt();

		ret = QFile::exists(dstpath);

		return ret ? ERROR_CODE_OK : ERROR_CODE_NETWORK_ERROR;
	};

	m_info = info;
	std::thread t([=]()
	{
		QString url = m_info->videoUrl;
		QString localPath = m_info->localPath;
		int ret = ERROR_CODE_NETWORK_ERROR;

		// 存在文件删除
		if (QFile::exists(localPath))
			QFile::remove(localPath);
		

		localPath = gGetTouTiaoTodaySourcePath();
		LOG(TR("开始下载视频"));
		ret = DownloadFun(url, localPath);

		if (ret == ERROR_CODE_NETWORK_ERROR)
		{
			LOG(TR("下载失败，重新获取下载地址"));
			emit sigUpdateVideoUrl(info);
			ret = info->errorCode;
			if (ret == ERROR_CODE_OK && url != info->videoUrl)
			{
				LOG(TR("获取下载地址成功，开始下载"));
				ret = DownloadFun(info->videoUrl, localPath);
			}

		}
		

		
		if (ret == ERROR_CODE_OK)
		{
			LOG(TR("下载成功"));
			QThread::sleep(1);
			Convert(localPath);
		}
		else
		{
			LOG(TR("下载失败"));
			Finish(ret);

		}
	});
	t.detach();

	return true;
}

bool DownloadManager::Convert(QString dstpath)
{
	LOG(TR("开始转码"));

	m_info->localPath = dstpath;
	QString dstpathTemp = gGetTouTiaoTodayDestePath(dstpath);
	ConvertManager convert;
	bool ret = convert.Convert(dstpath, dstpathTemp);
	if (ret)
	{
		LOG(TR("转码成功"));
		m_info->localPath = dstpathTemp;
		if (!QFile::remove(dstpath))
		{
			LOG(TR("删除原文件失败"));
		}
		Finish(ERROR_CODE_OK);
	}
	else
	{
		LOG(TR("转码失败"));
		Finish(ERROR_CODE_CONVERT_ERROR);

	}



	return true;
}

void DownloadManager::Finish(int code)
{
	emit sigFinish(code, m_info);
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
	QString duration = VideoInfo::SecondToQString(vinfo.duration * 0.95);

	QString ffmpegPath = QApplication::applicationDirPath() + "/ffmpeg/ffmpeg.exe";
	QString cmd = QString("%1 -ss %7 -t %8 -i %2 -vf delogo=x=%6:y=20:w=190:h=56,scale=%4:%5 -b:v %9 %3").
		arg(ffmpegPath).arg(src).arg(dst).arg(width).arg(height).
		arg(x).arg(startTime).arg(duration).arg(vinfo.bit_rate);

	bool ret = QProcess::execute(cmd) == 0;

	return ret;
}

#include "stdafx.h"
#include "DownloadManager.h"

#include <thread>

#include "inc_PyDll/IPython_Exe.h"

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
		QString dstpath = gGetTouTiaoTodaySourcePath();
		
		LOG(TR("开始下载视频"));
		bool ret = DownloadFun(url, dstpath);

		if (!ret)
		{
			LOG(TR("下载失败，重新获取下载地址"));
			emit sigUpdateVideoUrl(info);
			if (url != info->videoUrl)
			{
				LOG(TR("获取下载地址成功，开始下载"));
				ret = DownloadFun(info->videoUrl, dstpath);
			}
				
		}
		ret = true;
		if (ret)
		{
			LOG(TR("下载成功"));
			Convert(dstpath);
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

	LOG(TR("转码结束"));

	Finish(true);

	return true;
}

void DownloadManager::Finish(bool code)
{
	emit sigFinish(true, m_info);
	deleteLater();
}

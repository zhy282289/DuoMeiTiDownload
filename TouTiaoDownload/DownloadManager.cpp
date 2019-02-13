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
		
		LOG(TR("��ʼ������Ƶ"));
		bool ret = DownloadFun(url, dstpath);

		if (!ret)
		{
			LOG(TR("����ʧ�ܣ����»�ȡ���ص�ַ"));
			emit sigUpdateVideoUrl(info);
			if (url != info->videoUrl)
			{
				LOG(TR("��ȡ���ص�ַ�ɹ�����ʼ����"));
				ret = DownloadFun(info->videoUrl, dstpath);
			}
				
		}
		ret = true;
		if (ret)
		{
			LOG(TR("���سɹ�"));
			Convert(dstpath);
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

	LOG(TR("ת�����"));

	Finish(true);

	return true;
}

void DownloadManager::Finish(bool code)
{
	emit sigFinish(true, m_info);
	deleteLater();
}

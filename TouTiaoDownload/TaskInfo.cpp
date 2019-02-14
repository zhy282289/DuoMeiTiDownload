#include "stdafx.h"
#include "TaskInfo.h"

#include "inc_PyDll/IPython_Exe.h"

#include <QtWebEngineWidgets/QtWebEngineWidgets>
#include <QtWebEngineWidgets/QtWebEngineWidgets>
#include <thread>

#define TRY_COUNT 5
#define GET_MORE_COUNT 10

TaskObtainManager::TaskObtainManager(QObject *parent /*= nullptr*/)
	:QObject(parent)
{
	m_scaning = false;
	m_stoping = false;
	m_count = 1;
	m_curCount = 1;
	m_view = nullptr;
	m_detailView = nullptr;
	m_redownloadView = nullptr;
	m_getMoreCount = 0;

	QString pyevnpath = QDir::toNativeSeparators(QApplication::applicationDirPath() + "/python");
	pyevnpath.replace('\\', '/');
	IPython_Exe::GetInstance()->AppendPyEnvPath(pyevnpath.toLocal8Bit().data());
	 
}

TaskObtainManager::~TaskObtainManager()
{

}

bool TaskObtainManager::StartScan()
{
	if (m_scaning)
	{
		LOG(TR("����ɨ���У�"));
		return true;
	}

	LOG(TR("��ʼɨ������"));

	m_scaning = true;
	m_stoping = false;

	m_url = ScanConfig::Url();
	m_count = ScanConfig::Number();
	m_curCount = 1;
	m_getMoreCount = 0;

	if (m_view == nullptr)
	{
		m_view = GET_UNIQUEN_WEBVIEW();

		connect(m_view, &QWebEngineView::loadFinished, this, &TaskObtainManager::ParseMainPage);

		m_view->load(m_url);
		m_view->show();
	}
	else
	{
		ParseMainPage();
	}

	return false;
}

bool TaskObtainManager::StopScan()
{
	m_stoping = true;
	LOG(TR("����ֹͣɨ������"));
	return false;
}

TaskInfos TaskObtainManager::GetInfos()
{
	return m_infos;
}



bool TaskObtainManager::UpdateInfo(TaskInfoPtr info)
{
	static QString videoUrl;
	static QEventLoop eventloop;

	videoUrl = info->videoUrl;
	if (m_redownloadView == nullptr)
	{
		m_redownloadView = GET_UNIQUEN_WEBVIEW();

		connect(m_redownloadView, &QWebEngineView::loadFinished, [=]()
		{
			m_redownloadView->page()->toHtml([=](const QString &text)
			{
				m_redownloadView->page()->runJavaScript(
					"var es = document.getElementsByClassName('bui-left index-content');"
					"if (es.length > 0)"
					"{es[0].innerHTML;}"
					, [=](QVariant v)
				{

					QString html = v.toString();
					if (!html.isEmpty())
					{
						IPython_Exe *pyExe = IPython_Exe::GetInstance();
						bool ret = pyExe->Simple_Call("toutiao", "parseTouTiaoDetail", "(s)", html.toUtf8().data());
						string retString = pyExe->ReturnString();
						if (!retString.empty())
						{
							QString retStringUtf8 = QString::fromUtf8(retString.c_str());
							QJsonDocument json = QJsonDocument::fromJson(retString.c_str());
							QJsonObject detailObject = json.object();
							videoUrl = detailObject["videourl"].toString();
							eventloop.exit(1);
							return;
						}
					}
					eventloop.exit(0);
				});
			});
		});


	}

	m_redownloadView->load(info->url);
	m_redownloadView->show();


	if (eventloop.exec())
	{
		info->videoUrl = videoUrl;
		return true;
	}

	return false;
}

void TaskObtainManager::ParseMainPage()
{
	m_view->page()->toHtml([=](const QString &text)
	{

		m_view->page()->runJavaScript(
			"var es = document.getElementsByClassName('feed-infinite-wrapper');"
			"if (es.length > 0)"
			"{es[0].innerHTML;}"
			, [=](QVariant v)
		{
			bool bData = false;
			QString html = v.toString();
			if (!html.isEmpty())
			{
				IPython_Exe *pyExe = IPython_Exe::GetInstance();
				bool ret = pyExe->Simple_Call("toutiao", "parseTouTiaoFromMain", "(s)", html.toUtf8().data());
				string retString = pyExe->ReturnString();
				if (!retString.empty())
				{
					bData = true;
					QString retStringUtf8 = QString::fromUtf8(retString.c_str());
					m_mainNewUrlist = retStringUtf8.split(";", QString::SkipEmptyParts);
					LOG(QString(TR("��ȡ��ҳ��������%2")).arg(m_mainNewUrlist.size()));
					NextUrl();

				}

			}
			if (!bData)
			{
				LOG("error: document.getElementsByClassName('feed-infinite-wrapper');");
				//QThread::sleep(1);
				ParseMainPage();
			}

		});

	});

}

void TaskObtainManager::NextDownload()
{
	if (m_stoping)
	{
		m_scaning = false;
		LOG(TR("ֹͣɨ������"));
		emit sigStopScan();

	}
	else
	{
		if (m_curCount <= m_count)
		{
			NextUrl();
		}
		else
		{
			m_scaning = false;
			LOG(TR("ɨ��������ɣ�"));
			emit sigScanFinish();
		}
	}

}

void TaskObtainManager::NextUrl()
{
	QString taskUrl;
	while (!m_mainNewUrlist.isEmpty())
	{
		QString url = m_mainNewUrlist.takeFirst();
		if (!m_mainOldUrlist.contains(url))
		{
			m_mainOldUrlist.push_back(url);
			if (!IsUrlExistInDB(url))
			{
				taskUrl = url;
				break;
			}
			else
			{
				LOG("warning: db had exist this url");
			}
		}
	}
	if (!taskUrl.isEmpty())
	{
		ParseUrlDetailInfo(taskUrl);
	}
	else
	{
		// ˢ����ҳ
		GetMore();
	}
}

void TaskObtainManager::GetMore()
{
	if (++m_getMoreCount < GET_MORE_COUNT)
	{
		LOG("get more");
		m_view->page()->runJavaScript(
			"window.scrollTo(0,document.body.scrollHeight);"
			, [=](QVariant)
		{
			QTimer::singleShot(2000, [=]()
			{
				ParseMainPage();
			});
		});
	}
	else
	{
		LOG(TR("ˢ����ҳ"));
		m_getMoreCount = 0;
		m_view->load(m_url);

	}

}

bool TaskObtainManager::IsUrlExistInDB(const QString &url)
{
	//https://www.ixigua.com/group/6642977069707821581/
	QString id = url.mid(29, 19);
	bool ret = MY_DB->TaskContain(id);
	return ret;
}





void TaskObtainManager::ParseUrlDetailInfo(const QString &url)
{
	if (m_detailView == nullptr)
	{
		m_detailView = GET_UNIQUEN_WEBVIEW();

		connect(m_detailView, &QWebEngineView::loadFinished, this, [=]()
		{
			m_detailView->page()->toHtml([=](const QString &text)
			{
				m_detailView->page()->runJavaScript(
					"var es = document.getElementsByClassName('bui-left index-content');"
					"if (es.length > 0)"
					"{es[0].innerHTML;}"
					, [=](QVariant v)
				{

					QString html = v.toString();
					if (!html.isEmpty())
					{
						IPython_Exe *pyExe = IPython_Exe::GetInstance();
						bool ret = pyExe->Simple_Call("toutiao", "parseTouTiaoDetail", "(s)", html.toUtf8().data());
						string retString = pyExe->ReturnString();
						if (!retString.empty())
						{
							_ParseUrlDetailInfo(retString);
						}
						else
						{ 
							LOG("error: parseTouTiaoDetail return empty");
							m_stoping = true;
							NextDownload();
						}

					}
					else
					{
						LOG(QString("error: document.getElementsByClassName('bui-left index-content')"));
						NextDownload();
					}
				});
			});
		});


	}

	m_detailView->load(url);
	m_detailView->show();

}

void TaskObtainManager::_ParseUrlDetailInfo(const string& retString)
{
	QString retStringUtf8 = QString::fromUtf8(retString.c_str());
	QJsonDocument json = QJsonDocument::fromJson(retString.c_str());
	QJsonObject detailObject = json.object();

	QString origin = detailObject["origin"].toString();
	bool bOrigin = origin.indexOf(TR("ԭ��")) == -1;

	LOG(QString("%1 %2 %3 %4").
		arg(bOrigin ? TR("ԭ��") : TR("��ԭ��")).
		arg(detailObject["title"].toString()).
		arg(m_detailView->url().toString()).
		arg(detailObject["playcount"].toString()));


	if (!ScanConfig::Origin() && bOrigin)
	{
		//ֻɨ���ԭ��
		NextDownload();
		return;
	}

	// ���Ŵ���
	QString playCountStr = detailObject["playcount"].toString();
	if (ScanConfig::PlayTimes() > 0)
	{
		if (playCountStr.isEmpty() ||
			(!playCountStr.isEmpty() && QString(playCountStr[playCountStr.size() - 1]) != TR("��")))
		{
			NextDownload();
			return;
		}
	}

	float playCount = playCountStr.left(playCountStr.size() - 1).toFloat();
	if (playCount < ScanConfig::PlayTimes())
	{
		NextDownload();
		return;
	}


	TaskInfoPtr info = new TaskInfo;

	info->title = detailObject["title"].toString();
	info->url = m_detailView->url().toString();
	info->origin = bOrigin;

	info->id = info->url.mid(24, 19);

	info->videoUrl = detailObject["videourl"].toString();
	info->playCount = playCountStr;
	info->userName = detailObject["username"].toString();
	info->userUrl = detailObject["userurl"].toString();

	// ���浽���ݿ�
	if (MY_DB->TaskInsert(info))
	{
		m_infos.push_back(info);
		sigNewInfo(info);
		++m_curCount;
	}
	else
	{
		LOG("error: insert db fail");
	}

	NextDownload();
}
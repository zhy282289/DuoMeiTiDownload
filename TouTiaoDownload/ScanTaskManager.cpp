#include "stdafx.h"
#include "scantaskmanager.h"

#include "inc_PyDll/IPython_Exe.h"

#include <QtWebEngineWidgets/QtWebEngineWidgets>
#include <thread>

#define TRY_COUNT 5
#define GET_MORE_COUNT 10

ScanTaskManager::ScanTaskManager(QObject *parent /*= nullptr*/)
	:QObject(parent)
{
	m_tryGetMoreCount = 0;
	m_scaning = false;
	m_stoping = false;
	m_count = 1;
	m_curCount = 1;
	m_view = nullptr;
	m_detailView = nullptr;
	m_redownloadView = nullptr;

	QString pyevnpath = QDir::toNativeSeparators(QApplication::applicationDirPath() + "/python");
	pyevnpath.replace('\\', '/');
	IPython_Exe::GetInstance()->AppendPyEnvPath(pyevnpath.toLocal8Bit().data());
	 
}

ScanTaskManager::~ScanTaskManager()
{

}

bool ScanTaskManager::StartScan()
{
	if (m_scaning)
	{
		LOG(TR("����ɨ���У�"));
		return true;
	}

	LOG(TR("��ʼɨ������"));

	m_scaning = true;
	m_stoping = false;
	m_tryGetMoreCount = 0;

	m_url = ScanConfig::Url();
	m_count = ScanConfig::Number();
	m_curCount = 1;

	if (m_view == nullptr)
	{
		if (ScanConfig::ScanType() == 0)
		{
			m_view = GET_UNIQUEN_WEBVIEW()
		}
		else
		{
			m_view = GET_TEST_WEBVIEW()
		}

		connect(m_view, &QWebEngineView::loadFinished, this, &ScanTaskManager::_ParseMainPage);

		m_view->show();
	}
	m_view->load(m_url);


	return false;
}

bool ScanTaskManager::StopScan()
{
	m_stoping = true;
	LOG(TR("����ֹͣɨ������"));
	return false;
}

TaskInfos ScanTaskManager::GetInfos()
{
	return m_infos;
}



bool ScanTaskManager::UpdateInfo(TaskInfoPtr info)
{

	static QString videoUrl;
	static QEventLoop eventloop;

	videoUrl = info->videoUrl;
	if (m_redownloadView == nullptr)
	{
		m_redownloadView = GET_UNIQUEN_WEBVIEW();

		connect(m_redownloadView, &QWebEngineView::loadFinished, [=]()
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
						eventloop.exit(ERROR_CODE_OK);
						return;
					}
					eventloop.exit(ERROR_CODE_CONVERT_ERROR);
				}
				else
				{
					// �����Ƶ�Ƿ��Ѿ���������
					m_redownloadView->page()->runJavaScript(
						"document.body.innerHTML;"
						, [=](QVariant v)
					{
						QString html = v.toString();
						if (html.contains(TR("�Ҳ�������Ҫ��ҳ��")))
						{
							LOG(TR("��ȡ��Ƶ��Ϣʧ�ܣ��Ҳ�������Ҫ��ҳ��"));
							eventloop.exit(ERROR_CODE_REMOTEPATH_NOT_EXIST_ERROR);

						}
						else
							eventloop.exit(ERROR_CODE_NETWORK_ERROR);
					});
				}
					
			});
	
		});


	}

	m_redownloadView->load(info->url);
	m_redownloadView->show();

	int code = eventloop.exec();
	info->errorCode = code;
	if (code == ERROR_CODE_OK)
	{
		info->videoUrl = videoUrl;
		return true;
	}

	return false;
}

void ScanTaskManager::ParseMainPage()
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
			if (m_stoping)
				_StopScan();
			else
				ParseMainPage();

		}

	});

}

void ScanTaskManager::_ParseMainPage()
{
	// �л�����
	const int delayTime = 5000;


	LOG(QString(TR("�л�����:%1")).arg(ScanConfig::ScanType()));
	QTimer::singleShot(delayTime, [=]() {
		//m_view->page()->toHtml([=](const QString &text)
		//{
			m_view->page()->runJavaScript(QString(
				"var es = document.getElementsByClassName('channel-item');"
				"if (es.length > 0)"
				"{es[%1].click();}"
			).arg(ScanConfig::ScanType())
				, [=](QVariant v)
			{
				LOG(TR("��ʼ������ҳ���ݣ�"));
				QTimer::singleShot(delayTime, [=]() {
					ParseMainPage();

				});


			});



		//});
	});

}

void ScanTaskManager::NextDownload()
{
	if (m_stoping)
	{
		_StopScan();
	}
	else
	{
		if (m_curCount <= m_count)
		{
			NextUrl();
		}
		else
		{
			LOG(TR("ɨ��������ɣ�"));
			m_scaning = false;
			emit sigScanFinish();
		}
	}

}

void ScanTaskManager::NextUrl()
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
		}
	}
	if (!taskUrl.isEmpty())
	{
		ParseVideoPage(taskUrl);
	}
	else
	{
		if (++m_tryGetMoreCount < 10)
		{
			if (m_mainOldUrlist.size() < 200)
			{
				m_tryGetMoreCount = 0;
				// ˢ����ҳ
				GetMore();
			}
			else
			{
				m_mainOldUrlist.clear();
				// ���¼�����ҳ
				LOG(TR("���¼�����ҳ��"));
				m_view->load(m_url);
			}
		}
		else
		{
			m_tryGetMoreCount = 0;
			// ���¼�����ҳ
			LOG(TR("���¼�����ҳ��"));
			m_view->load(m_url);
		}


	}
}

void ScanTaskManager::GetMore()
{
	if (m_stoping)
	{
		_StopScan();

	}
	else
	{
		LOG("get more");
		gWebViewScrollBottom(m_view->page(), [=](QVariant)
		{
			QTimer::singleShot(2000, [=]()
			{
				ParseMainPage();
			});
		});
	}

}



bool ScanTaskManager::IsUrlExistInDB(const QString &url)
{
	LOG(TR("���ڲ�ѯ���ݿ⣬�Ƿ����ظ����ݣ�"));

	bool bexist = false;
	do 
	{
		QString id = url.mid(29, 19);
		if (MY_DB->TaskContain(id))
		{
			bexist = true;
			break;
		}
		if (MY_DB->DownladContain(id))
		{
			bexist = true;
			break;
		}
		if (MY_DB->HistoryContain(id))
		{
			bexist = true;
			break;
		}
	} while (false);
	if (bexist)
		LOG(TR("��ѯ��ϣ��ظ�����"));
	else
		LOG(TR("��ѯ��ϣ����ظ�����"));


	return bexist;
}





void ScanTaskManager::_StopScan()
{
	LOG(TR("ֹͣɨ������"));
	m_scaning = false;
	m_detailView->deleteLater();
	m_detailView = nullptr;
	emit sigStopScan();
}

void ScanTaskManager::ParseVideoPage(const QString &url)
{
	if (m_detailView == nullptr)
	{
		m_detailView = GET_UNIQUEN_WEBVIEW();
		//m_detailView = GET_TEST_WEBVIEW();

		connect(m_detailView, &QWebEngineView::loadFinished, this, [=]()
		{
			//m_detailView->page()->toHtml([=](const QString &text)
			//{
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
							_ParseVideoPage(retString);
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
						LOG(TR("��ȡ��Ƶ��ϸ��Ϣʧ�ܣ�����������ʱ��ޣ�"));

						if (ScanConfig::Loop())
						{
							LOG(TR("����������ɨ�裬60������ɨ������"));
							QTimer::singleShot(1000 * 60, this, &ScanTaskManager::NextDownload);
						}
						else
						{
							_StopScan();
						}


					}
				});
			//});
		});


	}

	m_detailView->load(url);
	m_detailView->show();

}

void ScanTaskManager::_ParseVideoPage(const string& retString)
{
	QString retStringUtf8 = QString::fromUtf8(retString.c_str());
	QJsonDocument json = QJsonDocument::fromJson(retString.c_str());
	QJsonObject detailObject = json.object();

	QString origin = detailObject["origin"].toString();
	bool bOrigin = origin.indexOf(TR("ԭ��")) == -1;

	LOG(QString("[%1] %2 %3 %4").
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
	info->videoType = ScanConfig::VideoType();
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

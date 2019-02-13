#include "stdafx.h"
#include "TodayTaskWnd.h"

#include <thread>
#include <QtWebEngineWidgets/QtWebEngineWidgets>

#include "inc_PyDll/IPython_Exe.h"

#define TRY_COUNT 5

TodayTaskWnd::TodayTaskWnd(QWidget *parent)
	: QWidget(parent)
{
	m_toolBar = new TodayTaskToolBar(this);
	m_listWnd = new TodayTaskListWnd(this);

}

TodayTaskWnd::~TodayTaskWnd()
{
}

void TodayTaskWnd::resizeEvent(QResizeEvent *event)
{
	int h = 100;
	m_toolBar->setGeometry(0, 0, width(), h);
	int top = m_toolBar->geometry().bottom() + 4;
	m_listWnd->setGeometry(0, top, width(), height()-top);

}

TodayTaskListWnd::TodayTaskListWnd(QWidget *parent)
	:QListWidget(parent)
	, m_curDownloadItem(nullptr)
{
	auto manager = TodayTaskScanUrlsManger::GetInstance();

	connect(manager, &TodayTaskScanUrlsManger::sigScan, this, [=]()
	{
		ClearItem();

	});

	connect(manager, &TodayTaskScanUrlsManger::sigScanFinish, this, [=](int code)
	{
		if (code != 0)
		{
			TodayTaskInfos infos = manager->GetInfos();
			for (auto info : infos)
			{
				AddItem(info);
			}
		}

	});


	connect(manager, &TodayTaskScanUrlsManger::sigDownload, this, [=]()
	{
		Download();
	});
	connect(manager, &TodayTaskScanUrlsManger::sigStopDownload, this, [=]()
	{
		StopDownload();
	});
	connect(manager, &TodayTaskScanUrlsManger::sigDownloadFinish, this, [=]()
	{
		Download();
	});
}

void TodayTaskListWnd::AddItem(TodayTaskInfo *info)
{
	QListWidgetItem *item = new QListWidgetItem(this);
	item->setSizeHint(QSize(100, 100));
	TodayTaskListItem *w = new TodayTaskListItem(info);
	this->setItemWidget(item, w);
}

void TodayTaskListWnd::ClearItem()
{
	this->clear();
}

void TodayTaskListWnd::Download()
{
	m_curDownloadItem = nullptr;
	for (int i = 0; i < count(); ++i)
	{
		auto item = qobject_cast<TodayTaskListItem*>(itemWidget(this->item(i)));
		if (!item->GetInfo()->download)
		{
			m_curDownloadItem = item;
			item->Download();
			break;
		}
	}
	if (!m_curDownloadItem)
	{
		TodayTaskScanUrlsManger::GetInstance()->sigDownloadFinishAll();
	}

}

void TodayTaskListWnd::StopDownload()
{
	if (m_curDownloadItem)
	{
		m_curDownloadItem->StopDownload();
		TodayTaskScanUrlsManger::GetInstance()->sigDownloadFinishAll();
	}
}

void TodayTaskListWnd::mouseReleaseEvent(QMouseEvent *e)
{
	QListWidget::mouseReleaseEvent(e);

}

TodayTaskToolBar::TodayTaskToolBar(QWidget *parent)
	:QWidget(parent)
{
	m_mainView = new QWebEngineView(this);
	m_mainView->setGeometry(0, 0, 1, 1);
	TodayTaskScanUrlsManger::GetInstance()->SetMainVeiw(m_mainView);

	m_lbUrl = new QLabel(TR("Gif主页:"), this);
	m_leUrl = new QLineEdit(this);

	m_lbUrlNum = new QLabel(TR("条数:"), this);
	m_leUrlNum = new QLineEdit(this);
	m_leUrlNum->setText("50");

	m_btnStart = new QPushButton(TR("开始扫描"), this);
	m_btnStop = new QPushButton(TR("结束扫描"), this);
	m_btnStop->setEnabled(false);
	m_btnStop->setVisible(false);
	m_btnDownlad = new QPushButton(TR("开始下载"), this);
	m_btnStopDownlad = new QPushButton(TR("停止下载"), this);


	m_leUrl->setText("http://tu.duowan.com/m/bxgif");

	auto manager = TodayTaskScanUrlsManger::GetInstance();
	connect(m_btnStart, &QPushButton::clicked, this, [=]() 
	{
		bool ok = false;
		int count = m_leUrlNum->text().toInt(&ok);
		bool ret = manager->StartScan(m_leUrl->text(), ok ? count : 50);
		ToolBarVisible(!ret);
	});
	connect(m_btnStop, &QPushButton::clicked, this, [=]()
	{
		manager->Stop();
	});

	connect(m_btnDownlad, &QPushButton::clicked, this, [=]()
	{
		ToolBarVisible(false);
		manager->StartDownload();

	});
	connect(m_btnStopDownlad, &QPushButton::clicked, this, [=]()
	{
		//ToolBarVisible(false);
		manager->StartDownload();

	});

	connect(manager, &TodayTaskScanUrlsManger::sigScanFinish, this, [=]()
	{
		ToolBarVisible(true);

	});

	connect(manager, &TodayTaskScanUrlsManger::sigDownloadFinishAll, this, [=]()
	{
		ToolBarVisible(true);

	});



}

void TodayTaskToolBar::ToolBarVisible(bool visible)
{
	m_lbUrl->setEnabled(visible);
	m_leUrl->setEnabled(visible);
	m_lbUrlNum->setEnabled(visible);
	m_leUrlNum->setEnabled(visible);
	m_btnStart->setEnabled(visible);
	m_btnStop->setEnabled(!visible);
	m_btnDownlad->setEnabled(visible);
	m_btnStopDownlad->setEnabled(visible);
}

void TodayTaskToolBar::resizeEvent(QResizeEvent *event)
{
	const int margins = 6;
	int left = margins;
	int top = margins;
	int lbw = 40;
	int leh = 26;
	const int btnw = 100;
	m_lbUrl->setGeometry(left, top, lbw, leh);
	left = m_lbUrl->geometry().right() + margins;
	int urlleft = width() - left - btnw - 20;
	m_leUrl->setGeometry(left, top, urlleft, leh);
	left = m_leUrl->geometry().right() + margins;
	m_btnStart->setGeometry(left, top, btnw, leh);

	top = m_btnStart->geometry().bottom() + margins;
	left = margins;
	m_lbUrlNum->setGeometry(left, top, lbw, leh);
	left = m_lbUrlNum->geometry().right() + margins;
	m_leUrlNum->setGeometry(left, top, 100, leh);

	left = m_btnStart->geometry().left();
	m_btnStop->setGeometry(left, top, btnw, leh);
	top = m_btnStop->geometry().bottom() + margins;
	left = width() - 2 * btnw - 3 * margins;
	m_btnDownlad->setGeometry(left, top, btnw, leh);
	left = m_btnDownlad->geometry().right() + margins;
	m_btnStopDownlad->setGeometry(left, top, btnw, leh);

}

TodayTaskScanUrlsManger* TodayTaskScanUrlsManger::GetInstance()
{
	static TodayTaskScanUrlsManger instance;
	return &instance;
}

TodayTaskScanUrlsManger::TodayTaskScanUrlsManger(QObject *parent /*= nullptr*/)
	:QObject(parent)
	, m_totalCount(0)
	, m_startDownload(false)
{
	QString pyevnpath = QDir::toNativeSeparators(QApplication::applicationDirPath() + "/python");
	pyevnpath.replace('\\', '/');
	IPython_Exe::GetInstance()->AppendPyEnvPath(pyevnpath.toLocal8Bit().data());


	connect(this, &TodayTaskScanUrlsManger::sigDownloadFinish, this, [=]()
	{
		SaveInfo();
	});

}

TodayTaskScanUrlsManger::~TodayTaskScanUrlsManger()
{
	qDeleteAll(m_infos);
	m_infos.clear();
}

bool TodayTaskScanUrlsManger::StartScan(QString url, int count)
{
	if (m_bRuning)
		return true;

	LOG(TR("开始扫描"));

	m_bRuning = true;
	m_url = url;
	m_totalCount = count;
	m_tryCount = 0;

	emit sigScan();
	//_StartScan(url, count);

	if (m_view == nullptr)
	{
		m_view = new QWebEngineView(0);
		m_view->setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint);
		m_view->resize(720, 576);
		m_view->show();
	}
	m_view->load(QUrl(url));


	connect(m_view, &QWebEngineView::loadFinished, this, [=]()
	{
		ParseURLAndSave();
	});

	return true;
}

bool TodayTaskScanUrlsManger::Stop()
{
	//if (m_bRuning)
	//{
	//	IPython_Exe *pyExe = IPython_Exe::GetInstance();
	//	bool ret = pyExe->Simple_Call("duowan_gif", "stop");
	//	ret |= (bool)pyExe->ReturnInt();

	//}

	return true;
}


bool TodayTaskScanUrlsManger::StartDownload()
{
	emit sigDownload();
	return true;
}

bool TodayTaskScanUrlsManger::StopDownload()
{
	emit sigStopDownload();
	return true;
}

TodayTaskInfos TodayTaskScanUrlsManger::GetInfos()
{
	return m_infos;
}


//void TodayTaskScanUrlsManger::_StartScan(QString url, int count)
//{
//	if (m_view == nullptr)
//	{
//		m_view = new QWebEngineView(0);
//		m_view->setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint);
//		m_view->resize(720,576);
//		m_view->show();
//	}
//	m_view->load(QUrl(url));
//
//	m_tryCount = 0;
//
//	connect(m_view, &QWebEngineView::loadFinished, this, [=]() 
//	{
//		ParseURLAndSave();
//	});
//}

void TodayTaskScanUrlsManger::LoadInfoFromJson()
{
	qDeleteAll(m_infos);
	m_infos.clear();


	QString path = gTodayUrlsPath();
	QFile file(path);
	if (file.open(QIODevice::ReadOnly))
	{
		QByteArray data = file.readAll();
		auto json = QJsonDocument::fromJson(data);
		QJsonObject object = json.object();
		QString time = object["time"].toString();
		int count = object["count"].toInt();
		auto array = object["body"].toArray();
		int id = 1;
		for (auto t : array)
		{
			TodayTaskInfo *info = new TodayTaskInfo;
			auto t2 = t.toObject();
			info->id = id++;
			info->title = t2["title"].toString();
			info->url = t2["href"].toString();
			info->download = t2["download"].toBool(false);
			auto mp4Urls = t2["mp4Urls"].toArray();
			for (auto t : mp4Urls)
			{
				info->mp4Urls.push_back(t.toObject()["url"].toString());
			}
			m_infos.push_back(info);
		}
	}
}

void TodayTaskScanUrlsManger::CheckUnique()
{
	auto infos = m_infos;
	for (auto info : infos)
	{
		if (DB_GIF->Contain(info->url))
		{
			m_infos.removeOne(info);
			delete info;
		}
	}
}

void TodayTaskScanUrlsManger::CheckUrlsCount()
{
	int count = m_totalCount - m_infos.size();
	
	if (count > 0)
	{
		if (IsTryTimesOut(count))
		{
			emit sigScanFinish(1);
			return;
		}
	

		LOG(TR("继续扫描")+ QString("total:%1 cur:%2").arg(m_totalCount).arg(m_infos.size()));

		m_view->page()->runJavaScript(
			"window.scrollTo(0,document.body.scrollHeight);"
			, [=](QVariant)
		{
			QTimer::singleShot(2000, [=]() 
			{
				ParseURLAndSave();
			});
		});
	}
	else
	{
		LOG(TR("扫描结束"));
		emit sigScanFinish(1);
	}
}

void TodayTaskScanUrlsManger::ParseURLAndSave()
{
	m_view->page()->toHtml([=](const QString &text)
	{
		IPython_Exe *pyExe = IPython_Exe::GetInstance();
		QString path = gTodayUrlsPath();
		QByteArray array = text.toUtf8();
		bool ret = pyExe->Simple_Call("duowan_gif", "getGifUrlsFromMainPage", "(ssi)", path.toUtf8().data(), array.data(), m_totalCount);
		QMetaObject::invokeMethod(this, "ScanOneTimeFinish", Qt::QueuedConnection, Q_ARG(bool, ret));
	});
}

bool TodayTaskScanUrlsManger::IsTryTimesOut(int count)
{
	static int tryCount = 0;
	if (tryCount != count)
	{
		tryCount = count;
	}
	else
	{
		if (++m_tryCount > TRY_COUNT)
		{
			LOG(TR("扫描超时，停止继续扫描"));
			return true;
		}
	}
	return false;
}

void TodayTaskScanUrlsManger::SaveInfo()
{
	if (m_infos.isEmpty())
		return;

	QJsonDocument doc;
	QJsonObject object;
	object["time"] = QDateTime::currentDateTime().toString("yy-MM-dd hh:mm:ss");
	object["count"] = m_infos.size();
	QJsonArray array;
	for (TodayTaskInfo *info : m_infos)
	{
		QJsonObject t;
		t["title"] = info->title;
		t["href"] = info->url;
		t["download"] = info->download;
		QJsonArray mp4Urls;
		for (auto p : info->mp4Urls)
		{
			QJsonObject t2;
			t2["url"] = p;
			mp4Urls.push_back(t2);
		}
		t["mp4Urls"] = mp4Urls;
		array.push_back(t);
	}
	object["body"] = array;
	doc.setObject(object);

	QString path = gTodayUrlsPath();
	QFile file(path);
	if (file.open(QIODevice::WriteOnly))
	{
		auto data = doc.toJson();
		file.write(data);
	}
}

void TodayTaskScanUrlsManger::SetMainVeiw(QWebEngineView *view)
{
	m_view = view;
}

void TodayTaskScanUrlsManger::ScanOneTimeFinish(bool code)
{
	if (code == 1)
	{
		LoadInfoFromJson();
		CheckUnique();
		CheckUrlsCount();
	}
	else
	{
		LOG(TR("错误： start scan return 0"));
		emit sigScanFinish(code);

	}
}


TodayTaskListItem::TodayTaskListItem(TodayTaskInfo *info, QWidget *parent /*= nullptr*/)
	:QWidget(parent)
	,m_info(info)
{
	m_lbNumber = new QLabel(this);
	//m_lbNumber->setAlignment(Qt::AlignTop);

	m_lbTitle = new QLabel(this);
	m_lbUrl = new QLabel(this);
	m_lbLoading = new QLabel(this);
	m_lbDesc = new QLabel(this);

	m_lbNumber->setText(QString::number(info->id));
	m_lbTitle->setText(info->title);
	m_lbUrl->setText(info->url);

	if (info->download)
	{
		m_lbNumber->setStyleSheet("background-color:rgb(137,245,156)");
		m_lbDesc->setText(QString("[%1/%2]").arg(m_info->mp4Urls.size()).arg(m_info->mp4Urls.size()));
		m_lbLoading->setText(TR("已下载"));
	}

	m_timerDownload = new QTimer(this);
	m_timerDownload->setInterval(500);
	connect(m_timerDownload, &QTimer::timeout, this, [=]()
	{
		static QString a[4] = { TR("下载中"),TR("下载中."),TR("下载中.."),TR("下载中...") };
		static int count = 0;
		if (m_bDonwloadLoading)
		{
			m_lbLoading->setText(a[count++%4]);
		}
	});

	m_totalPages = 1;
	m_curPages = 1;
	m_view = nullptr;
	m_tryCount = 0;
	m_bDonwloadLoading = false;
}

TodayTaskInfo* TodayTaskListItem::GetInfo()
{
	return m_info;
}

void TodayTaskListItem::Download()
{
	if (!m_info->download && !m_bDonwloadLoading)
	{
		_StartDownload();
		_Download(m_curPages);
	}

}

void TodayTaskListItem::StopDownload()
{
	if (m_bDonwloadLoading)
	{
		m_lbLoading->setText(TR("取消下载"));
		_FinishDownload();
	}
	
}

void TodayTaskListItem::_Download(int page)
{ 
	if (m_view == nullptr)
	{
		m_view = new QWebEngineView(this);
		int h = height() - 20;
		m_view->setGeometry(width()-2, 1, 1,1);
	}
	QString urlstr = QString("%1#p%2").arg(m_info->url).arg(page);
	m_view->load(QUrl(urlstr));
	m_view->show();

	m_lbLoading->setText(TR("下载中"));
	m_lbDesc->setText(QString("[%1/%2]").arg(page).arg(m_totalPages));
	m_lbNumber->setStyleSheet("background-color:rgb(255,119,122)");
	m_timerDownload->start();
	m_tryCount = 0;

	GetViewPage();
}

void TodayTaskListItem::NextDownload()
{
	if (++m_curPages <= m_totalPages)
	{
		_Download(m_curPages);
	}
	else
	{
		LOG(TR("下载完成：") + m_info->title);
		LOG("");
		_FinishDownload();
		m_lbLoading->setText(TR("下载完成"));
		m_lbNumber->setStyleSheet("background-color:rgb(137,245,156)");
		m_info->download = true;
		TodayTaskScanUrlsManger::GetInstance()->sigDownloadFinish();
	}
}

void TodayTaskListItem::GetViewPage()
{
	if (!m_bDonwloadLoading)
		return;

	if (m_tryCount++ < TRY_COUNT)
	{
		m_view->page()->toHtml([=](const QString &text)
		{
			IPython_Exe *pyExe = IPython_Exe::GetInstance();

			QByteArray array = text.toUtf8();
			string arrayString = array.data();
			bool ret = pyExe->Simple_Call("duowan_gif", "getMp4Path", "(s)", arrayString.c_str());
			QString result = QString::fromUtf8(pyExe->ReturnString().c_str());
			QStringList strList = result.split(";", QString::SkipEmptyParts);
			if (strList.size() >= 2)
			{
				m_totalPages = qMax(1, strList[0].toInt());
				//test
				//if (m_totalPages > 5) m_totalPages = 5;

				QString mp4Url = strList[1];
				m_info->mp4Urls << mp4Url;

				auto destPath = GetDownlaodMp4Directory();
				gMakeDirectory(destPath);
				LOG(TR("正在下载：")+mp4Url);
				std::thread th([=]()
				{
					TodayTaskDownloadManger::GetInstance()->DownloadFile(mp4Url, destPath);
					LOG(TR("成功下载："));

					QMetaObject::invokeMethod(this, "RunJavaScript", Qt::QueuedConnection);

				});
				th.detach();

			}
			else
			{
				QTimer::singleShot(2000, [=]() 
				{
					GetViewPage();
				});

			}


		});
	}
	else
	{
		NextDownload();
	}
}

QString TodayTaskListItem::GetDownlaodMp4Directory()
{
	return QString("%1\\%2").arg(gTodaySourceGifPath()).arg(m_info->id);
}



void TodayTaskListItem::_StartDownload()
{
	m_bDonwloadLoading = true;
	m_totalPages = 1;
	m_curPages = 1;
	m_tryCount = 0;
}

void TodayTaskListItem::_FinishDownload()
{
	m_bDonwloadLoading = false;
	m_timerDownload->stop();
	m_view->deleteLater();
	m_view = nullptr;
}

Q_INVOKABLE void TodayTaskListItem::RunJavaScript()
{
	m_view->page()->runJavaScript(
		"var el = document.getElementsByClassName('next');"
		"el[0].click();"
		, [=](QVariant)
	{
		NextDownload();
	});
}

void TodayTaskListItem::resizeEvent(QResizeEvent *event)
{
	const int margins = 10;
	int left = margins;
	int top = margins;
	int lbh = 22;
	const int btnw = 100;

	m_lbNumber->setGeometry(left, top, 20, lbh);
	left = m_lbNumber->geometry().right() + margins;
	m_lbTitle->setGeometry(left, top, width() - 200, lbh);
	top = m_lbTitle->geometry().bottom();
	m_lbUrl->setGeometry(left, top, width() - 200, lbh);
	top = m_lbUrl->geometry().bottom();
	m_lbDesc->setGeometry(left, top, 40, lbh);
	left = m_lbDesc->geometry().right();
	m_lbLoading->setGeometry(left, top, width() - 200, lbh);

}

void TodayTaskListItem::mouseReleaseEvent(QMouseEvent *e)
{
	QWidget::mouseReleaseEvent(e);

	if (e->button() == Qt::RightButton)
	{
		QMenu menu;
		auto actViewUrl = menu.addAction(TR("查看网页"));
		auto actViewUrlDir = menu.addAction(TR("查看下载目录"));
		actViewUrlDir->setEnabled(m_info->download);
		auto act = menu.exec(e->globalPos());
		if (act == actViewUrl)
		{
			QDesktopServices::openUrl(QUrl(m_info->url));
		}
		else if (act == actViewUrlDir)
		{
			QDesktopServices::openUrl(QUrl::fromLocalFile(GetDownlaodMp4Directory()));
		}
	}
}

//void TodayTaskListItem::paintEvent(QPaintEvent *event)
//{
//	QPainter painter(this);
//	if (m_info->download)
//	{
//		painter.fillRect(rect(), QColor(137, 245, 156));
//	}
//	else
//	{
//		if (m_bDonwloadLoading)
//		{
//			painter.fillRect(rect(), QColor(255, 119, 122));
//		}
//	}
//}

TodayTaskDownloadManger* TodayTaskDownloadManger::GetInstance()
{
	static TodayTaskDownloadManger instance;
	return &instance;
}

TodayTaskDownloadManger::TodayTaskDownloadManger(QObject *parent /*= nullptr*/)
	:QObject(parent)
{

}

TodayTaskDownloadManger::~TodayTaskDownloadManger()
{

}

bool TodayTaskDownloadManger::Download(QString urlPath)
{

	QWebEngineView *view = new QWebEngineView(0);
	QUrl url(urlPath);
	view->load(url);
	view->resize(50,50);
	view->show();
	QTimer::singleShot(5000, [=]() 
	{
		view->page()->toHtml([](const QString &text) {
			qDebug() << text;
		});
	});

	return false;
}

bool TodayTaskDownloadManger::DownloadFile(QString url, QString dir)
{
	IPython_Exe *pyExe = IPython_Exe::GetInstance();
	bool ret = pyExe->Simple_Call("download_file", "download", "(ss)", url.toUtf8().data(), dir.toUtf8().data());
	ret |= (bool)pyExe->ReturnInt();
	return ret;
}

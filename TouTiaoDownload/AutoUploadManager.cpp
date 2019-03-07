#include "stdafx.h"
#include "AutoUploadManager.h"

#include <thread>
#include "NetworkCookie.h"

AutoUploadManager::AutoUploadManager(QObject *parent)
	: QObject(parent)
	, m_view(nullptr)
{
	m_url = "https://mp.toutiao.com/profile_v3/xigua/upload-video";
	m_tryLoadUpload = 0;
	m_tryLoadFinish = 0;
	m_bUploading = false;
	m_bInDialog = false;
}

AutoUploadManager::~AutoUploadManager()
{
}

bool AutoUploadManager::Login(int index)
{
	CreateWebView(index);
	m_view->load(m_url);
	return true;
}

bool AutoUploadManager::StartUpload(TaskInfoPtr info, int index)
{
	m_bUploading = true;

	m_info = info;

	LOG((TR("开始上传任务")));
	CreateWebView(index);
	connect(m_view, &QWebEngineView::loadFinished, this, &AutoUploadManager::LoadFinished, Qt::UniqueConnection);

	m_view->load(m_url);

	std::thread t([=]() {

		while (true)
		{
			QThread::sleep(10);

			if (!m_bInDialog)
			{
				auto dlg = m_view->findChild<QDialog*>();
				if (dlg)
				{
					LOG((TR("离开本页面需要回车确认")));

					auto dlgHwnd = ::FindWindow(L"#32770", L"打开");
					auto button = ::FindWindowEx(dlgHwnd, 0, L"Button", nullptr);//  # 确定按钮Button
					auto cancelButton = ::FindWindowEx(dlgHwnd, button, L"Button", nullptr);//  # 确定按钮Button
					::SendMessage(dlgHwnd, WM_COMMAND, 2, (LPARAM)cancelButton); // # 按button
				}
			}

			if (!m_bUploading)
			{
				break;
			}


		}


	});
	t.detach();

	return true;
}

bool AutoUploadManager::StopUpload()
{
	m_bUploading = false;
	return true;
}

void AutoUploadManager::LoadFinished()
{
	if (++m_tryLoadUpload <= 30)
	{
		m_view->page()->runJavaScript(
			"var es = document.getElementsByTagName('input');"
			"if (es.length > 0)"
			"{"
			"1+0;"
			"}"
			"else{0+0;}"
			, [=](QVariant v)
		{
			int ret = v.toInt();
			if (ret == 0)
			{
				LOG((TR("取不到视频输入框，继续获取")));
				QTimer::singleShot(2 * 1000, [=]()
				{
					LoadFinished();
				});
			}
			else
			{
				LOG((TR("取到视频输入框，上传文件")));

				m_tryLoadUpload = 0;
				m_view->activateWindow();
			
				gMoveCursorAndClick(m_view->mapToGlobal(QPoint(1000, 365)));
				UploadFile();
			}
		});
	}
	else
	{
		// 重新刷新网页
		LOG((TR("获取不到上传文件按钮，重新刷新网页")));
		m_tryLoadUpload = 0;
		m_view->load(m_url);
	}

}



void AutoUploadManager::CreateWebView(int index)
{
	if (m_view == nullptr)
	{
		m_view = GET_TEST_WEBVIEW()
		m_view->showMaximized();

		auto cookie = new NetworkCookie(m_view, index, "https://mp.toutiao.com");
	}

}

void AutoUploadManager::UploadFile()
{
	m_bInDialog = true;
	QTimer::singleShot(3000, [=]()
	{
		auto dlg = m_view->findChild<QDialog*>();
		if (dlg)
		{
			auto dlgHwnd = ::FindWindow(L"#32770", L"打开");
			auto ComboBoxEx32 = ::FindWindowEx(dlgHwnd, 0, L"ComboBoxEx32", nullptr);
			auto ComboBox = ::FindWindowEx(ComboBoxEx32, 0, L"ComboBox", nullptr);
			auto Edit = ::FindWindowEx(ComboBox, 0, L"Edit", nullptr);  //# 上面三句依次寻找对象，直到找到输入框Edit对象的句柄
			auto button = ::FindWindowEx(dlgHwnd, 0, L"Button", nullptr);//  # 确定按钮Button

			auto pathw = m_info->localPath.toStdWString();
			::SendMessage(Edit, WM_SETTEXT, 0, (LPARAM)pathw.c_str()); // # 往输入框输入绝对地址

			int len = SendMessage(Edit, WM_GETTEXTLENGTH, 0, 0);
			if (len==0)
			{
				UploadFile();
			}
			else
			{
				QTimer::singleShot(1000, [=]() {
					::SendMessage(dlgHwnd, WM_COMMAND, 1, (LPARAM)button); // # 按button
					LOG((TR("开始上传文件")));
					MonitorUploadFileFinish();
					m_bInDialog = false;
				});
			}
			



		}
		else
		{
			m_bInDialog = false;
			UploadFile();
			// 重新刷新网页
			//LOG((TR("获取不到上传文件对话框，重新刷新网页")));
			//m_view->load(m_url);
		}

	});
}

void AutoUploadManager::MonitorUploadFileFinish()
{
	if (++m_tryLoadFinish < 30 * 5)
	{

		QTimer::singleShot(2 * 1000, [=]()
		{
			m_view->page()->runJavaScript(
				"var es = document.getElementsByClassName('tips');"
				"if (es.length > 0)"
				"{"
				"var s;"
				"for(var i=0;i<es.length;i++){s=s+es[i].innerHTML;}"
				"s+'1';"
				"}"
				"else{''+'1';}"
				, [=](QVariant v)
			{
				QString html = v.toString();
				qDebug() << html;
				if (html.contains(TR("上传完毕")))
				{
					m_tryLoadFinish = 0;
					LOG((TR("获取到上传完毕状态")));
					UploadFileFinishEx();
				}
				else if (html.contains(TR("重复")))
				{
					LOG((TR("视频重复上传,下个任务")));
					m_tryLoadFinish = 0;
					m_view->load(m_url);

					QTimer::singleShot(1000, [=]() {
						gKeybdEvent_CTL(VK_RETURN);
						emit sigFinish(true, m_info);
					});

				}
				else
				{
					LOG((TR("未获取到上传完毕状态，继续监听")));
					MonitorUploadFileFinish();
				}
			});
		});

	}
	else
	{
		m_tryLoadFinish = 0;
		// 重新刷新网页
		LOG((TR("等不到上传文件完成，重新刷新网页")));
		m_view->load(m_url);

	}

}


void AutoUploadManager::UploadFileFinishEx()
{
	m_view->activateWindow();

	QString title = REPLACEWORDS_MANAGER->Replace(m_info->title);

	if (title == m_info->title)
		LOG((TR("未进行标题替换")));
	else
		LOG((QString(TR("替换标题\n原:%1\n目:%2")).arg(m_info->title).arg(title)));

	QApplication::clipboard()->setText(title);
	gMoveCursorAndClick(m_view->mapToGlobal(QPoint(870, 477)));
	gKeybdEvent_CTL('A');
	gKeybdEvent_CTL('V');

	QTimer::singleShot(1000, [=]() {

		gKeybdEvent(VK_TAB);
		gKeybdEvent_CTL('A');
		gKeybdEvent_CTL('V');

		gKeybdEvent(VK_TAB);
		gKeybdEvent(VK_TAB);
		gKeybdEvent(VK_TAB);

		QTimer::singleShot(1000, [=]() {
			QApplication::clipboard()->setText(TR("搞笑"));
			gKeybdEvent_CTL('V');
			gKeybdEvent_CTL(VK_RETURN);

			QTimer::singleShot(1000, [=]() {
				QApplication::clipboard()->setText(TR("影视"));
				gKeybdEvent_CTL('V');
				gKeybdEvent_CTL(VK_RETURN);

				QTimer::singleShot(1000, [=]() {
					gKeybdEvent(VK_TAB);
					gKeybdEvent(VK_TAB);
					gKeybdEvent(VK_TAB);
					gKeybdEvent(VK_TAB);
					gKeybdEvent(VK_TAB);
					gKeybdEvent(VK_RETURN);

					QTimer::singleShot(1000, [=]() {
						for (int i = 0; i < 12; ++i)
							gKeybdEvent(VK_DOWN);

						gKeybdEvent(VK_RETURN);

						QTimer::singleShot(2000, [=]() {
							Submit();

						});
					});

				});


			});


		});
	});


}

void AutoUploadManager::Submit()
{
	m_view->page()->runJavaScript(""
		"var submitbtn = document.getElementsByClassName('submit btn ');"
		"if(submitbtn.length>0){"
		"submitbtn[0].click();0+1;}else{0+0;}"
		, [=](QVariant v)
	{

		auto ret = v.toInt();
		qDebug() << ret;
		QTimer::singleShot(2000, [=]() {

			if (ret == 1)
			{
				LOG((TR("任务成功提交")));
			}
			else
			{
				LOG((TR("任务成功失败 找不到提交按钮")));
			}
			if (m_view)
				disconnect(m_view, &QWebEngineView::loadFinished, this, &AutoUploadManager::LoadFinished);
			emit sigFinish(ret, m_info);
		});


	});
}

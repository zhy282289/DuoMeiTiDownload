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

	LOG((TR("��ʼ�ϴ�����")));
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
					LOG((TR("�뿪��ҳ����Ҫ�س�ȷ��")));

					auto dlgHwnd = ::FindWindow(L"#32770", L"��");
					auto button = ::FindWindowEx(dlgHwnd, 0, L"Button", nullptr);//  # ȷ����ťButton
					auto cancelButton = ::FindWindowEx(dlgHwnd, button, L"Button", nullptr);//  # ȷ����ťButton
					::SendMessage(dlgHwnd, WM_COMMAND, 2, (LPARAM)cancelButton); // # ��button
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
				LOG((TR("ȡ������Ƶ����򣬼�����ȡ")));
				QTimer::singleShot(2 * 1000, [=]()
				{
					LoadFinished();
				});
			}
			else
			{
				LOG((TR("ȡ����Ƶ������ϴ��ļ�")));

				m_tryLoadUpload = 0;
				m_view->activateWindow();
			
				gMoveCursorAndClick(m_view->mapToGlobal(QPoint(1000, 365)));
				UploadFile();
			}
		});
	}
	else
	{
		// ����ˢ����ҳ
		LOG((TR("��ȡ�����ϴ��ļ���ť������ˢ����ҳ")));
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
			auto dlgHwnd = ::FindWindow(L"#32770", L"��");
			auto ComboBoxEx32 = ::FindWindowEx(dlgHwnd, 0, L"ComboBoxEx32", nullptr);
			auto ComboBox = ::FindWindowEx(ComboBoxEx32, 0, L"ComboBox", nullptr);
			auto Edit = ::FindWindowEx(ComboBox, 0, L"Edit", nullptr);  //# ������������Ѱ�Ҷ���ֱ���ҵ������Edit����ľ��
			auto button = ::FindWindowEx(dlgHwnd, 0, L"Button", nullptr);//  # ȷ����ťButton

			auto pathw = m_info->localPath.toStdWString();
			::SendMessage(Edit, WM_SETTEXT, 0, (LPARAM)pathw.c_str()); // # �������������Ե�ַ

			int len = SendMessage(Edit, WM_GETTEXTLENGTH, 0, 0);
			if (len==0)
			{
				UploadFile();
			}
			else
			{
				QTimer::singleShot(1000, [=]() {
					::SendMessage(dlgHwnd, WM_COMMAND, 1, (LPARAM)button); // # ��button
					LOG((TR("��ʼ�ϴ��ļ�")));
					MonitorUploadFileFinish();
					m_bInDialog = false;
				});
			}
			



		}
		else
		{
			m_bInDialog = false;
			UploadFile();
			// ����ˢ����ҳ
			//LOG((TR("��ȡ�����ϴ��ļ��Ի�������ˢ����ҳ")));
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
				if (html.contains(TR("�ϴ����")))
				{
					m_tryLoadFinish = 0;
					LOG((TR("��ȡ���ϴ����״̬")));
					UploadFileFinishEx();
				}
				else if (html.contains(TR("�ظ�")))
				{
					LOG((TR("��Ƶ�ظ��ϴ�,�¸�����")));
					m_tryLoadFinish = 0;
					m_view->load(m_url);

					QTimer::singleShot(1000, [=]() {
						gKeybdEvent_CTL(VK_RETURN);
						emit sigFinish(true, m_info);
					});

				}
				else
				{
					LOG((TR("δ��ȡ���ϴ����״̬����������")));
					MonitorUploadFileFinish();
				}
			});
		});

	}
	else
	{
		m_tryLoadFinish = 0;
		// ����ˢ����ҳ
		LOG((TR("�Ȳ����ϴ��ļ���ɣ�����ˢ����ҳ")));
		m_view->load(m_url);

	}

}


void AutoUploadManager::UploadFileFinishEx()
{
	m_view->activateWindow();

	QString title = REPLACEWORDS_MANAGER->Replace(m_info->title);

	if (title == m_info->title)
		LOG((TR("δ���б����滻")));
	else
		LOG((QString(TR("�滻����\nԭ:%1\nĿ:%2")).arg(m_info->title).arg(title)));

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
			QApplication::clipboard()->setText(TR("��Ц"));
			gKeybdEvent_CTL('V');
			gKeybdEvent_CTL(VK_RETURN);

			QTimer::singleShot(1000, [=]() {
				QApplication::clipboard()->setText(TR("Ӱ��"));
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
				LOG((TR("����ɹ��ύ")));
			}
			else
			{
				LOG((TR("����ɹ�ʧ�� �Ҳ����ύ��ť")));
			}
			if (m_view)
				disconnect(m_view, &QWebEngineView::loadFinished, this, &AutoUploadManager::LoadFinished);
			emit sigFinish(ret, m_info);
		});


	});
}

#include "stdafx.h"
#include "AutoUploadManager.h"

#include <thread>
#include "NetworkCookie.h"

#define AUTOUPLOAD "autoupload"
#define INTERNAL_TIME 1000

#define MOUSEKEYBOARD_FORBID_DISABLED	BlockInput(false);
#define MOUSEKEYBOARD_FORBID_ENABLED	BlockInput(true);

AutoUploadManager::AutoUploadManager(QObject *parent)
	: QObject(parent)
	, m_view(nullptr)
	, m_cookie(nullptr)
{
	m_url = "https://mp.toutiao.com/profile_v3/xigua/upload-video";
	m_tryLoadUpload = 0;
	m_tryLoadFinish = 0;
	m_bUploading = false;
	m_bInDialog = false;
	m_index = 1;

	/*
	QObject::connect: Cannot queue arguments of type 'QClipboard::Mode'
(Make sure 'QClipboard::Mode' is registered using qRegisterMetaType().)
	*/
	//qRegisterMetaType<QClipboard::Mode>("QClipboard::Mode");
}

AutoUploadManager::~AutoUploadManager()
{
	if (m_view)
	{
		delete m_view;
	}
}

bool AutoUploadManager::Login(int index)
{
	m_index = index;
	CreateWebView(index);
	m_view->load(m_url);

	return true;
}

bool AutoUploadManager::StartUpload(TaskInfoPtr info, int index)
{
	m_index = index;

	m_bUploading = true;
	m_tryLoadFinish = 0;
	m_tryLoadUpload = 0;

	m_info = info;

	LOG((TR("��ʼ�ϴ�����")));
	CreateWebView(index);
	LoadURL();


	std::thread t([=]() {

		while (true)
		{
			QThread::sleep(10);

			if (!m_bInDialog)
			{
				auto dlg = m_view->findChild<QDialog*>();
				if (dlg)
				{
					m_view->activateWindow();

					LOG((TR("�뿪��ҳ����Ҫ�س�ȷ��")));

					auto dlgHwnd = ::FindWindow(L"#32770", L"��");
					auto button = ::FindWindowEx(dlgHwnd, 0, L"Button", nullptr);//  # ȷ����ťButton
					auto cancelButton = ::FindWindowEx(dlgHwnd, button, L"Button", nullptr);//  # ȷ����ťButton
					::SendMessage(dlgHwnd, WM_COMMAND, 2, (LPARAM)cancelButton); // # ��button

					gKeybdEvent_CTL(VK_RETURN);

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
	if (m_view)
	{
		m_view->deleteLater();
		m_view = nullptr;
	}

	return true;
}



void AutoUploadManager::LoadFinished()
{
	DisConnect();
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

				PROCESS_LOCK->Lock(AUTOUPLOAD);
				MOUSEKEYBOARD_FORBID_ENABLED;

				m_tryLoadUpload = 0;
				m_view->activateWindow();
				
				QTimer::singleShot(1000, [=]() {

					gMoveCursorAndClick(m_view->mapToGlobal(QPoint(1000, 365)));
					UploadFile();
				});

			}
		});
	}
	else
	{
		// ����ˢ����ҳ
		LOG((TR("��ȡ�����ϴ��ļ���ť������ˢ����ҳ")));
		m_tryLoadUpload = 0;
		LoadURL();
	}

}



void AutoUploadManager::CreateWebView(int index)
{
	if (m_view)
	{
		m_view->deleteLater();
		m_view = nullptr;
	}
	if (m_view == nullptr)
	{
		m_view = GET_TEST_WEBVIEW()
		m_view->setWindowTitle(QString(TR("�˺�%1")).arg(index));
		m_cookie = new NetworkCookie(m_view, index, "https://mp.toutiao.com");
	}
	m_view->showMaximized();

}

void AutoUploadManager::UploadFile()
{
	m_bInDialog = true;
	QTimer::singleShot(1000, [=]()
	{
		auto dlg = m_view->findChild<QDialog*>();
		if (dlg)
		{
			auto dlgHwnd = ::FindWindow(L"#32770", L"��");
			auto ComboBoxEx32 = ::FindWindowEx(dlgHwnd, 0, L"ComboBoxEx32", nullptr);
			auto ComboBox = ::FindWindowEx(ComboBoxEx32, 0, L"ComboBox", nullptr);
			auto Edit = ::FindWindowEx(ComboBox, 0, L"Edit", nullptr);  //# ������������Ѱ�Ҷ���ֱ���ҵ������Edit����ľ��
			auto button = ::FindWindowEx(dlgHwnd, 0, L"Button", nullptr);//  # ȷ����ťButton

			auto pathw = QDir::toNativeSeparators(m_info->localPath).toStdWString();
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
					PROCESS_LOCK->UnLock(AUTOUPLOAD);
					MOUSEKEYBOARD_FORBID_DISABLED;

					MonitorUploadFileFinish();
					m_bInDialog = false;
				});
			}

		}
		else
		{
			PROCESS_LOCK->UnLock(AUTOUPLOAD);
			MOUSEKEYBOARD_FORBID_DISABLED;

			LOG((TR("��ȡ�����ϴ��ļ��Ի����ϴ�ʧ�ܣ���������")));
			//gMoveCursorAndClick(m_view->mapToGlobal(QPoint(1000, 365)));
			m_bInDialog = false;
			//UploadFile();
			emit sigFinish(false, m_info);
		}

	});
}

void AutoUploadManager::MonitorUploadFileFinish()
{
	if (++m_tryLoadFinish < 30 * 10)
	{
		QTimer::singleShot(2 * 1000, [=]()
		{
			if (m_view)
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

					ReLoadURL(true);

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
		//LoadURL();
		ReLoadURL(false);

		
	}

}


void AutoUploadManager::UploadFileFinishEx()
{
	PROCESS_LOCK->Lock(AUTOUPLOAD);
	MOUSEKEYBOARD_FORBID_ENABLED;


	m_view->activateWindow();

	QString title = REPLACEWORDS_MANAGER->Replace(m_info->title);

	if (title == m_info->title)
		LOG((TR("δ���б����滻")));
	else
		LOG((QString(TR("�滻����\nԭ:%1\nĿ:%2")).arg(m_info->title).arg(title)));


	QApplication::clipboard()->setText(title);


	gWebViewScrollTop(m_view->page(), [=](QVariant html) {

		m_view->activateWindow();

		gMoveCursorAndClick(m_view->mapToGlobal(QPoint(870, 477)));
		gKeybdEvent_CTL('A');
		gKeybdEvent_CTL('V');
		 
		QTimer::singleShot(6000, [=]() {

			gKeybdEvent(VK_TAB);
			gKeybdEvent_CTL('A');
			gKeybdEvent_CTL('V');

			gKeybdEvent(VK_TAB);
			gKeybdEvent(VK_TAB);
			gKeybdEvent(VK_TAB);

			QStringList keyWords = DownloadFinishConfig::KeyWords(m_index).split(" ");
			if (keyWords.isEmpty())
				keyWords.push_back(TR("��Ц Ӱ��"));
			SetKeyWorks(keyWords);

		});

	});



}

void AutoUploadManager::Submit()
{

	m_view->page()->runJavaScript(
		"var es = document.getElementsByTagName('input');"
		"if (es.length > 0)"
		"{"
			"var ret='';"
			"for(var i=0;i<es.length;i++){"
				"if (es[i].getAttribute('type')=='text')"
				"{ret = es[i].value;break;}"
			"}"
			"ret + ''"
		"}"
		"else{''+'';}"
		, [=](QVariant v)
	{
		QString html = v.toString();
		int count = 0;
		for (int i = 0; i < html.size(); ++i)
		{
			if (html[i].isNumber())
				++count;
		}
		if (count < 5)
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
						emit sigFinish(ret, m_info);
					}
					else
					{
						LOG((TR("����ɹ�ʧ�� �Ҳ����ύ��ť����������")));
						emit sigFinish(ret, m_info);
					}
				});


			});
		}
		else
		{
			LOG((TR("�ϴ��������ô��󣬼�������")));
			emit sigFinish(false, m_info);
		}
		
	});


}

void AutoUploadManager::LoadURL()
{
	Connect();
	m_view->load(m_url);
}

void AutoUploadManager::Connect()
{
	connect(m_view, &QWebEngineView::loadFinished, this, &AutoUploadManager::LoadFinished, Qt::UniqueConnection);
}

void AutoUploadManager::DisConnect()
{
	disconnect(m_view, &QWebEngineView::loadFinished, this, &AutoUploadManager::LoadFinished);

}

void AutoUploadManager::ReLoadURL(bool ret)
{
	m_view->activateWindow();
	m_view->load(m_url);
	QTimer::singleShot(3000, [=]() {
		gKeybdEvent_CTL(VK_RETURN);
		emit sigFinish(ret, m_info);
	});
}

void AutoUploadManager::SetKeyWorks(QStringList keyWords)
{
	QTimer::singleShot(INTERNAL_TIME, [=]() {
		if (keyWords.size() > 0)
		{
			auto key = keyWords[0];
			QStringList templist;
			for (int i = 1; i < keyWords.size(); ++i)
				templist.push_back(keyWords[i]);
			QApplication::clipboard()->setText(key);
			gKeybdEvent_CTL('V');
			gKeybdEvent_CTL(VK_RETURN);
			SetKeyWorks(templist);
		}
		else
		{
			for (int i = 0; i < DownloadFinishConfig::TabNum(m_index); ++i)
				gKeybdEvent(VK_TAB);

			gKeybdEvent(VK_RETURN);

			QTimer::singleShot(INTERNAL_TIME, [=]() {
				for (int i = 0; i < DownloadFinishConfig::MajorKeyWord(m_index); ++i)
					gKeybdEvent(VK_DOWN);

				gKeybdEvent(VK_RETURN);

				QTimer::singleShot(1000, [=]() {

					PROCESS_LOCK->UnLock(AUTOUPLOAD);
					MOUSEKEYBOARD_FORBID_DISABLED;
					Submit();

				});
			});
		}
	
	});

}


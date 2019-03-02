#include "stdafx.h"
#include "AutoUploadManager.h"

AutoUploadManager::AutoUploadManager(QObject *parent)
	: QObject(parent)
	, m_view(nullptr)
{
	m_url = "https://mp.toutiao.com/profile_v3/xigua/upload-video";
	m_tryLoadUpload = 0;
}

AutoUploadManager::~AutoUploadManager()
{
}

bool AutoUploadManager::StartUpload(TaskInfoPtr info)
{
	m_info = info;

	if (QFile::exists(m_info->localPath))
	{
		LOG((TR("文件路径不存在，下个任务")));
		sigFinish(false, m_info);
		return false;
	}

	LOG((TR("开始上传任务")));
	if (m_view == nullptr)
	{

		m_view = GET_TEST_WEBVIEW()
		connect(m_view, &QWebEngineView::loadFinished, this, &AutoUploadManager::LoadFinished);

		m_view->show();
	}
	m_view->load(m_url);

	return true;
}

void AutoUploadManager::LoadFinished()
{
	//return;
	if (++m_tryLoadUpload <= 20)
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
				//QThread::msleep(40);
				gMoveCursorAndClick(m_view->mapToGlobal(QPoint(500, 365)));
				UploadFile();
			}
		});
	}
	else
	{
		// 重新刷新网页
		LOG((TR("获取不到上传文件按钮，重新刷新网页")));
		m_view->load(m_url);
	}

}



void AutoUploadManager::UploadFile()
{

	QTimer::singleShot(2000, [=]()
	{
		auto dlg = m_view->findChild<QDialog*>();
		if (dlg)
		{
			auto dlgHwnd = ::FindWindow(L"#32770", L"打开");
			auto ComboBoxEx32 = ::FindWindowEx(dlgHwnd, 0, L"ComboBoxEx32", nullptr);
			auto ComboBox = ::FindWindowEx(ComboBoxEx32, 0, L"ComboBox", nullptr);
			auto Edit = ::FindWindowEx(ComboBox, 0, L"Edit", nullptr);  //# 上面三句依次寻找对象，直到找到输入框Edit对象的句柄
			auto button = ::FindWindowEx(dlgHwnd, 0, L"Button", nullptr);//  # 确定按钮Button

			//wchar_t fff[] = L"E:\\zhy\\works\\qt\\DuoMeiTiDownload\\bin\\x64\\Debug\\toutiao\\19-02-24\\{a6828027-75a0-428e-ab4b-a02cfca98b2a}_1.mp4";
			auto pathw = m_info->localPath.toStdWString();
			::SendMessage(Edit, WM_SETTEXT, 0, (LPARAM)pathw.c_str()); // # 往输入框输入绝对地址
			::SendMessage(dlgHwnd, WM_COMMAND, 1, (LPARAM)button); // # 按button

			LOG((TR("开始上传文件")));
			MonitorUploadFileFinish();

		}
		else
		{
			// 重新刷新网页
			LOG((TR("获取不到上传文件对话框，重新刷新网页")));
			m_view->load(m_url);
		}
		
	});
}

void AutoUploadManager::MonitorUploadFileFinish()
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
				LOG((TR("获取到上传完毕状态")));
				UploadFileFinishEx();
			}
			else
			{
				LOG((TR("未获取到上传完毕状态，继续监听")));
				MonitorUploadFileFinish();
			}
		});
	});
}

void AutoUploadManager::UploadFileFinish()
{
	auto CreateTitle = [](QString title) {
		return QString(
			"var es = document.getElementsByClassName('article-title-wrap-new');"
			"if (es.length > 0){"
			"es[0].children[0].setAttribute('value','%1');"
			"0+1;"
			"}"
			"else{0+0;}"
		).arg(title);
	};
	auto CreateContent = [](QString content) {
		return QString(
			"var es2 = document.getElementsByClassName('tui-input-wrapper');"
			"if (es2.length > 0){"
			"es2[0].innerHTML='%1';"
			"0+1;"
			"}"
			"else{0+0;}"
		).arg(content);
	};
	auto CreateVideoLabel = [](QString tag,int valueIndex) {
		return QString(
			"var Selectmultivaluewrapper = document.getElementsByClassName('Select-multi-value-wrapper');"
			"if (Selectmultivaluewrapper.length > 0){"
			//"Selectmultivaluewrapper[0].removeChild(Selectmultivaluewrapper[0].firstChild);"

			"var selectvalue = document.createElement('div');"
			"selectvalue.setAttribute('class','Select-value');"

			"var firstspan = document.createElement('span');"
			"firstspan.setAttribute('class','Select-value-icon');"
			"firstspan.setAttribute('aria-hidden','true');"
			"firstspan.innerText='x';"
			//"Selectmultivaluewrapper[0].appendChild(firstspan);"

			"var selectvaluespan = document.createElement('span');"
			"selectvaluespan.setAttribute('class','Select-value-label');"
			"selectvaluespan.setAttribute('role','option');"
			"selectvaluespan.setAttribute('aria-selected','true');"
			"selectvaluespan.setAttribute('id','react-select-2--value-%2');"
			"selectvaluespan.innerText='%1';"

			"var selectariaonlyspan = document.createElement('span');"
			"selectariaonlyspan.setAttribute('class','Select-aria-only');"
			"selectariaonlyspan.innerText='&nbsp;';"
			"selectvaluespan.appendChild(selectariaonlyspan);"

			"selectvalue.appendChild(selectvaluespan);"
			"Selectmultivaluewrapper[0].appendChild(selectvalue);"
			"0+1;"
			"}else{0+0;}"
		).arg(tag).arg(valueIndex);
	};

	auto CreateVideoType = [](QString tag) {
		return QString(
			"var SelecttuiselectSelectsingle = document.getElementsByClassName('Select tui-select Select--single');"
			"if(SelecttuiselectSelectsingle.length>0){"
			"SelecttuiselectSelectsingle[0].setAttribute('class','Select tui-select has-value Select--single');"
			"}"

			"var Selectmultivaluewrapper = document.getElementsByClassName('Select-multi-value-wrapper');"
			"if (Selectmultivaluewrapper.length > 1){"
			//"Selectmultivaluewrapper[1].removeChild(Selectmultivaluewrapper[1].firstChild);"

			"var selectvalue = document.createElement('div');"
			"selectvalue.setAttribute('class','Select-value');"
			"var selectvaluespan = document.createElement('span');"
			"selectvaluespan.setAttribute('class','Select-value-label');"
			"selectvaluespan.setAttribute('role','option');"
			"selectvaluespan.setAttribute('aria-selected','true');"
			"selectvaluespan.setAttribute('id','react-select-3--value-item');"
			"selectvaluespan.innerText='%1';"
			"selectvalue.appendChild(selectvaluespan);"
			"Selectmultivaluewrapper[1].appendChild(selectvalue);"
			"0+1;"
			"}else{0+0;}"

			"var submitbtn = document.getElementsByClassName('submit btn ');"
			//	"if(submitbtn.length==1){"
			//	"submitbtn[0].click();0+1;}else{0+0;}"
			"var temppp = document.getElementsByClassName('video-content-body');"
			"temppp[0].innerHTML+'0';"
		).arg(tag);
	};


	QString title = m_info->title;

	LOG((TR("修改上传参数")));

	m_view->page()->runJavaScript(QString(
		"%1%2%3%4%5"	
	).arg(CreateTitle(title)).arg(CreateContent(title))
		.arg(CreateVideoLabel(TR("搞笑"),0)).arg(CreateVideoLabel(TR("视频"),1))
		.arg(CreateVideoType(TR("搞笑")))
		, [=](QVariant v)
	{
		auto html = v.toString();
		qDebug() << html;
		//if (v.toInt())
		//{
		//	//提交成功
		//	sigFinish(true, m_info);
		// }
		//else
		//{
		//	// 提交失败
		//	sigFinish(false, m_info);
		//}

		
	});
}

void AutoUploadManager::UploadFileFinishEx()
{
	m_view->activateWindow();


	QApplication::clipboard()->setText(m_info->title);
	gMoveCursorAndClick(m_view->mapToGlobal(QPoint(500, 477)));
	gKeybdEvent_CTL('A');
	gKeybdEvent_CTL('V');

	QTimer::singleShot(1000, [=]() {
		m_view->activateWindow();
		gMoveCursorAndClick(m_view->mapToGlobal(QPoint(500, 535)));
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
					gKeybdEvent(VK_RETURN);

					QTimer::singleShot(1000, [=]() {
						for (int i = 0; i < 12; ++i)
							gKeybdEvent(VK_DOWN);

						gKeybdEvent(VK_RETURN);

						QTimer::singleShot(1000, [=]() {
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
		if (ret == 1)
		{
			LOG((TR("任务成功提交")));

		}
		else
		{
			LOG((TR("任务成功失败")));
		}

	});
}

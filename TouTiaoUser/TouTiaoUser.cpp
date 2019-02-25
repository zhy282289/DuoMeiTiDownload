#include "stdafx.h"
#include "TouTiaoUser.h"

TouTiaoUser::TouTiaoUser(QWidget *parent)
	: QWidget(parent)
{
	resize(800, 400);

	m_unFollowView = nullptr;
	m_btnUnFollow = new QPushButton(TR("����ע"), this);
	m_btnStartUnFollow = new QPushButton(TR("��ʼ����ע"), this);

	m_browser = new QTextBrowser(this);

	connect(m_btnUnFollow, &QPushButton::clicked, this, [=]()
	{
		if (m_unFollowView)
			delete m_unFollowView;
		{
			m_unFollowView = new HtmlView(0);
			m_unFollowView->resize(800,800);
			m_unFollowView->show();
			m_url = "https://www.toutiao.com/c/user/relation/51620417289/?tab=following#mid=1568287264229378";
			m_unFollowView->load(QUrl(m_url));
			//m_unFollowView->load(QUrl("https://www.baidu.com"));
			connect(m_unFollowView, &QWebEngineView::loadFinished, [=]()
			{
				//m_moreCount = 0;
				//GetMoreFollow();

			});
		}
	});

	connect(m_btnStartUnFollow, &QPushButton::clicked, this, [=]()
	{
		m_moreCount = 0;
		GetMoreFollow();

		connect(GetView(), &QWebEngineView::loadFinished, this, [=]()
		{
			m_moreCount = 0;
			GetMoreFollow();

		}, Qt::UniqueConnection);

	});

	connect(LOG_INFO, &LogInfo::sigShowLog, this, [=](QString msg) {
		m_browser->append(msg);
	});

}

void TouTiaoUser::GetMoreFollow()
{
	if (++m_moreCount <= 10)
	{
		LOG(QString("GetMoreFollow:%1").arg(m_moreCount));
		GetView()->page()->runJavaScript(
			"window.scrollTo(0,document.body.scrollHeight);"
			, [=](QVariant)
		{
			QTimer::singleShot(500, [=]()
			{
				GetMoreFollow();
			});
		});
	}
	else
	{
		UnFollow();
	}

}

QWebEngineView* TouTiaoUser::GetView()
{
	auto view = m_unFollowView->GetView();
	if (view == nullptr)
		view = m_unFollowView;

	return view;
}


void TouTiaoUser::UnFollow()
{
	LOG(QString("��ʼ����ע"));
	auto view = GetView();
	view->page()->runJavaScript(
		"var es = document.getElementsByClassName('relation');"
		"for(var i = 0;i<es.length;++i)"
		"{"
		"if (es[i].children.length>0)"
		"{es[i].children[0].children[0].click();}else{alert('2');}"
		"es[0].children[0].getAttribute('media_id')"
		"}"
		, [=](QVariant v)
	{
		if (m_firstId != v.toString())
		{
			LOG(QString("����ע���"));
			m_firstId = v.toString();
			GetView()->load(m_url);
		}
		else
		{
			LOG(QString("�޷��ٷ���ע���������"));
		}


	});
}

void TouTiaoUser::ConnectView()
{


}

void TouTiaoUser::resizeEvent(QResizeEvent *event)
{
	int left = 20;
	int top = 20;

	const int btnw = 70;
	const int btnh = 22;
	m_btnUnFollow->setGeometry(left, top, btnw, btnh);
	left = m_btnUnFollow->geometry().right() + 10;
	m_btnStartUnFollow->setGeometry(left, top, btnw, btnh);

	m_browser->setGeometry(0, 100, width(), height() - 100);
}

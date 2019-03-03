#include "stdafx.h"
#include "WebViewWnd.h"

#include <QtWebEngineWidgets/QtWebEngineWidgets>


static WebViewWnd* s_WebViewWnd = nullptr;
WebViewWnd* WebViewWnd::GetInstance(QWidget *parent /*= nullptr*/)
{
	if (s_WebViewWnd == nullptr)
	{
		s_WebViewWnd = new WebViewWnd(parent);
	}
	return s_WebViewWnd;
}

HtmlView* WebViewWnd::GetView(int id)
{
	if (!m_views.contains(id))
	{
		HtmlView *view = new HtmlView(this);
		const int w = 8;
		view->setGeometry(w*m_views.size(), 0, w, height());
		m_views[id] = view;
	}

	return m_views[id];
}

HtmlView* WebViewWnd::GetExistView(int id)
{
	if (m_views.contains(id))
	{
		return m_views[id];

	}

	return nullptr;
}

HtmlView* WebViewWnd::GetUniquenView()
{
	int id = 0;
	do 
	{
		if (!m_views.contains(id))
			break;
	} while (++id);

	return GetView(id);
}

HtmlView* WebViewWnd::CreateTestView()
{
	auto view = new HtmlView(0);
	view->resize(800, 800);
	return view;
}

WebViewWnd::WebViewWnd(QWidget *parent)
	: QWidget(parent)
{

}

WebViewWnd::~WebViewWnd()
{
	s_WebViewWnd = nullptr;
}

//////////////////////////////////////////////////////////////////////////

HtmlView::HtmlView(QWidget *parent)
	:QWebEngineView(parent)
{
	m_view = nullptr;
}

HtmlView* HtmlView::GetView()
{
	return m_view;
}
QWebEngineView *HtmlView::createWindow(QWebEnginePage::WebWindowType type)
{

	m_view = new HtmlView(0);
	m_view->resize(800, 800);
	m_view->show();
	return m_view;  

}
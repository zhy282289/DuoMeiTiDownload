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

QWebEngineView* WebViewWnd::GetView(int id)
{
	if (!m_views.contains(id))
	{
		QWebEngineView *view = new QWebEngineView(this);
		const int w = 8;
		view->setGeometry(w*m_views.size(), 0, w, height());
		m_views[id] = view;
	}

	return m_views[id];
}

QWebEngineView* WebViewWnd::GetExistView(int id)
{
	if (m_views.contains(id))
	{
		return m_views[id];

	}

	return nullptr;
}

QWebEngineView* WebViewWnd::GetUniquenView()
{
	int id = 0;
	do 
	{
		if (!m_views.contains(id))
			break;
	} while (++id);

	return GetView(id);
}

WebViewWnd::WebViewWnd(QWidget *parent)
	: QWidget(parent)
{

}

WebViewWnd::~WebViewWnd()
{
	s_WebViewWnd = nullptr;
}

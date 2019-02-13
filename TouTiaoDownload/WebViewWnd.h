#pragma once

#include <QWidget>

class WebViewWnd : public QWidget
{
	Q_OBJECT

public:
	static WebViewWnd* GetInstance(QWidget *parent = nullptr);


	QWebEngineView* GetView(int id);
	QWebEngineView* GetExistView(int id);
	QWebEngineView* GetUniquenView();

	WebViewWnd(QWidget *parent);
	~WebViewWnd();

private:
	QMap<int, QWebEngineView*> m_views;
};

#define GET_WEBVIEW(id) WebViewWnd::GetInstance()->GetView(id);
#define GET_EXIST_WEBVIEW(id) WebViewWnd::GetInstance()->GetExistView(id);
#define GET_UNIQUEN_WEBVIEW() WebViewWnd::GetInstance()->GetUniquenView();
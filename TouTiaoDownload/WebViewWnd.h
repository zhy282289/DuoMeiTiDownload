#pragma once

#include <QWidget>


enum ViewType
{
	ViewType_Scan,

};
class HtmlView;
class WebViewWnd : public QWidget
{
	Q_OBJECT

public:
	static WebViewWnd* GetInstance(QWidget *parent = nullptr);


	HtmlView* GetView(int id);
	HtmlView* GetExistView(int id);
	HtmlView* GetUniquenView();

	HtmlView* CreateTestView();

	HtmlView* GetOtherView(int id);


	WebViewWnd(QWidget *parent);
	~WebViewWnd();

private:
	QMap<int, HtmlView*> m_views;
	QMap<int, HtmlView*> m_otherViews;
};


class HtmlView : public QWebEngineView
{
	Q_OBJECT
public:
	HtmlView(QWidget *parent);
	HtmlView* GetView();
	void SetCookieIndex(int index);

protected:
	QWebEngineView *HtmlView::createWindow(QWebEnginePage::WebWindowType type);
	

private:
	HtmlView *m_view;
	int m_index;
};

#define GET_WEBVIEW(id) WebViewWnd::GetInstance()->GetView(id)
#define GET_EXIST_WEBVIEW(id) WebViewWnd::GetInstance()->GetExistView(id)
#define GET_UNIQUEN_WEBVIEW() WebViewWnd::GetInstance()->GetUniquenView()
#define GET_TEST_WEBVIEW() WebViewWnd::GetInstance()->CreateTestView()
#define GET_OTHER_WEBVIEW(id) WebViewWnd::GetInstance()->GetOtherView(id)


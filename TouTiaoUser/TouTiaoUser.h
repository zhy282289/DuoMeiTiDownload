#pragma once

#include <QtWebEngineWidgets/QtWebEngineWidgets>

class HtmlView;
class TouTiaoUser : public QWidget
{
	Q_OBJECT

public:
	TouTiaoUser(QWidget *parent = Q_NULLPTR);

private:

protected:
	void resizeEvent(QResizeEvent *event);

private:

	QPushButton *m_btnUnFollow;
	QPushButton *m_btnStartUnFollow;
	HtmlView *m_unFollowView;

};


class HtmlView : public QWebEngineView
{
	Q_OBJECT
public:
	HtmlView(QWidget *parent) :QWebEngineView(parent) 
	{
		m_view = nullptr;
	}

	QWebEngineView* GetView()
	{
		return m_view;
	}
protected:
	QWebEngineView *HtmlView::createWindow(QWebEnginePage::WebWindowType type)
	{

		m_view = new HtmlView(0);
		m_view->resize(800, 800);
		m_view->show();
		return m_view;   //返回之后就会对当前页面对象进行刷新，之前的页面就变成跳转的页面

	}

private:
	QWebEngineView *m_view;
};

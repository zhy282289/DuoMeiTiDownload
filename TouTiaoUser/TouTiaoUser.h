#pragma once

#include <QtWebEngineWidgets/QtWebEngineWidgets>

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
	QWebEngineView *m_unFollowView;

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
	QWebEngineView *HtmlView::createWindow(QWebEnginePage::WebWindowType type) {
		auto ff = url();
		auto fff = ff.url();

		if (m_view)
		{
			auto view = m_view;
			m_view = nullptr;
			view->deleteLater();

		}
		m_view = new HtmlView(0);
		m_view->resize(800, 800);
		m_view->show();
		return m_view;   //����֮��ͻ�Ե�ǰҳ��������ˢ�£�֮ǰ��ҳ��ͱ����ת��ҳ��

	}

private:
	QWebEngineView *m_view;
};

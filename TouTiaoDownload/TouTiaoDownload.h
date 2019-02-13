#pragma once

class WebViewWnd;
class TouTiaoDownload : public QWidget
{
	Q_OBJECT

public:
	TouTiaoDownload(QWidget *parent = Q_NULLPTR);

protected:
	void resizeEvent(QResizeEvent *event);
private:

	QTabWidget *m_tabWidget;
	QPlainTextEdit *m_debugInfo;
	QSplitter *m_splitter;

	WebViewWnd *m_webWnd;
};

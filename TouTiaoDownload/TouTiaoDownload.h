#pragma once

class WebViewWnd;
class TouTiaoDownload : public QWidget
{
	Q_OBJECT

public:
	TouTiaoDownload(QWidget *parent = Q_NULLPTR);
	~TouTiaoDownload();

protected:
	void resizeEvent(QResizeEvent *event);
	void closeEvent(QCloseEvent *event);

private:

	QTabWidget *m_tabWidget;
	QPlainTextEdit *m_debugInfo;
	QSplitter *m_splitter;

	WebViewWnd *m_webWnd;
};

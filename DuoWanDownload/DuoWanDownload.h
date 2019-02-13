#pragma once


class DuoWanDownload : public QWidget
{
	Q_OBJECT

public:
	DuoWanDownload(QWidget *parent = Q_NULLPTR);

protected:
	void resizeEvent(QResizeEvent *event);
private:

	QTabWidget *m_tabWidget;
	QTextBrowser *m_debugInfo;
	QSplitter *m_splitter;
};

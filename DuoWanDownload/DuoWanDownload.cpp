#include "stdafx.h"
#include "DuoWanDownload.h"

#include "TodayTaskWnd.h"


DuoWanDownload::DuoWanDownload(QWidget *parent)
	: QWidget(parent)
{
	resize(1024, 720);
	m_debugInfo = new QTextBrowser(this);

	m_tabWidget = new QTabWidget(this);

	m_tabWidget->addTab(new TodayTaskWnd, TR("今日任务"));


	m_splitter = new QSplitter(Qt::Vertical, this);
	m_splitter->setChildrenCollapsible(false);
	m_splitter->addWidget(m_tabWidget);
	m_splitter->addWidget(m_debugInfo);
	m_splitter->setSizes(QList<int>() << 700 << 200);


	connect(LOG_INFO, &LogInfo::sigShowLog, this, [=](QString log) 
	{
		m_debugInfo->append(log);
	});

}

void DuoWanDownload::resizeEvent(QResizeEvent *event)
{
	m_splitter->setGeometry(rect());
}

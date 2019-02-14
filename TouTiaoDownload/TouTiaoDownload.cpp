#include "stdafx.h"
#include "TouTiaoDownload.h"

#include "ScanWnd.h"
#include "TaskWnd.h"
#include "DownloadTaskWnd.h"
#include "HistoryWnd.h"
#include "webviewwnd.h"


TouTiaoDownload::TouTiaoDownload(QWidget *parent)
	: QWidget(parent)
{

	resize(1024, 720);
	m_debugInfo = new QPlainTextEdit(this);
	m_webWnd = WebViewWnd::GetInstance(this);

	m_tabWidget = new QTabWidget(this);

	ScanWnd *scanWnd = new ScanWnd;
	TaskWnd *taskWnd = new TaskWnd;
	DownloadTaskWnd *downloadWnd = new DownloadTaskWnd;
	HistoryWnd *historyWnd = new HistoryWnd;
	m_tabWidget->addTab(scanWnd, TR("ɨ������"));
	m_tabWidget->addTab(taskWnd, TR("����������"));
	m_tabWidget->addTab(downloadWnd, TR("����������"));
	m_tabWidget->addTab(historyWnd, TR("��ʷ����"));


	m_splitter = new QSplitter(Qt::Vertical, this);
	m_splitter->setChildrenCollapsible(false);
	m_splitter->addWidget(m_tabWidget);
	m_splitter->addWidget(m_debugInfo);
	m_splitter->setSizes(QList<int>() << 700 << 200);


	connect(LOG_INFO, &LogInfo::sigShowLog, this, [=](QString log)
	{
		m_debugInfo->appendPlainText(log);
	});

	// ScanWnd
	connect(scanWnd, &ScanWnd::sigStartScan, taskWnd, [=]() 
	{
		m_tabWidget->setCurrentIndex(1);
		taskWnd->StartScan();
	});
	connect(scanWnd, &ScanWnd::sigStopScan, taskWnd, [=]() 
	{
		taskWnd->StopScan();

	});
	connect(scanWnd, &ScanWnd::sigNewInfo, taskWnd, [=](TaskInfoPtr info)
	{
		taskWnd->AddItem(info);
	});
	connect(scanWnd, &ScanWnd::sigScanFinish, taskWnd, [=]()
	{
		taskWnd->FinishScan();
	});

	// TaskWnd
	connect(taskWnd, &TaskWnd::sigDownloadStart, this, [=]()
	{
		scanWnd->SetEnabled(false);
	});
	connect(taskWnd, &TaskWnd::sigDownloadStop, this, [=]()
	{
		scanWnd->ResetUI();
	});
	connect(taskWnd, &TaskWnd::sigDownloadFinish, this, [=]()
	{
		scanWnd->ResetUI();
	});

	connect(taskWnd, &TaskWnd::sigDownloadOneFinish, this, [=](TaskInfoPtr info)
	{
		downloadWnd->AddItem(info);
	});
	connect(taskWnd, &TaskWnd::sigConvert2History, this, [=](TaskInfoPtr info)
	{
		historyWnd->AddItem(info);
	});
	// DownloadTaskWnd
	connect(downloadWnd, &DownloadTaskWnd::sigConvert2Task, this, [=](TaskInfoPtr info)
	{
		taskWnd->AddItem(info);
	});
	connect(downloadWnd, &DownloadTaskWnd::sigConvert2History, this, [=](TaskInfoPtr info)
	{
		historyWnd->AddItem(info);
	});

	// HistoryWnd
	connect(historyWnd, &HistoryWnd::sigConvert2Task, this, [=](TaskInfoPtr info)
	{
		taskWnd->AddItem(info);
	});
	connect(historyWnd, &HistoryWnd::sigConvert2Download, this, [=](TaskInfoPtr info)
	{
		downloadWnd->AddItem(info);
	});

}

void TouTiaoDownload::resizeEvent(QResizeEvent *event)
{
	m_splitter->setGeometry(rect().adjusted(0,0,0,-8));
	m_webWnd->setGeometry(0, height() - 8, width(), 8);
}
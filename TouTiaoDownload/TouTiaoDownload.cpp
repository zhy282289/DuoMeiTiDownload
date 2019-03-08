#include "stdafx.h"
#include "TouTiaoDownload.h"

#include "ScanWnd.h"
#include "TaskWnd.h"
#include "DownloadTaskWnd.h"
#include "HistoryWnd.h"
#include "webviewwnd.h"
#include "ReplaceWnd.h"

TouTiaoDownload::TouTiaoDownload(QWidget *parent)
	: QWidget(parent)
{
	
	resize(1024, 720);
	m_debugInfo = new QPlainTextEdit(this);
	m_webWnd = WebViewWnd::GetInstance(this);

	m_tabWidget = new QTabWidget(this);

	ScanWnd *scanWnd = new ScanWnd(this);
	TaskWnd *taskWnd = new TaskWnd(this);
	DownloadTaskWnd *downloadWnd = new DownloadTaskWnd(this);
	HistoryWnd *historyWnd = new HistoryWnd(this);
	ReplaceWnd *replaceWnd = new ReplaceWnd(this);

	m_tabWidget->addTab(scanWnd, TR("扫描设置"));
	m_tabWidget->addTab(taskWnd, TR("待下载任务"));
	m_tabWidget->addTab(downloadWnd, TR("已下载任务"));
	m_tabWidget->addTab(historyWnd, TR("历史任务"));
	m_tabWidget->addTab(replaceWnd, TR("替换词"));


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
		//m_tabWidget->setCurrentIndex(1);
		taskWnd->StartScan();
		downloadWnd->SetEnabled(false);
	});
	connect(scanWnd, &ScanWnd::sigStopScan, taskWnd, [=]() 
	{
		taskWnd->StopScan();
		downloadWnd->SetEnabled(true);


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
		downloadWnd->SetEnabled(false);

	});
	connect(taskWnd, &TaskWnd::sigDownloadStop, this, [=]()
	{
		scanWnd->ResetUI();
		downloadWnd->SetEnabled(true);
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
	connect(taskWnd, &TaskWnd::sigDownloadStart, this, [=]()
	{
		scanWnd->SetEnabled(false);
		downloadWnd->SetEnabled(false);
	});
	connect(taskWnd, &TaskWnd::sigDownloadStop, this, [=]()
	{
		scanWnd->ResetUI();
		taskWnd->SetEnabled(true);
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

TouTiaoDownload::~TouTiaoDownload()
{
}

void TouTiaoDownload::resizeEvent(QResizeEvent *event)
{
	m_splitter->setGeometry(rect().adjusted(0,0,0,-8));
	m_webWnd->setGeometry(0, height() - 8, width(), 8);
}

void TouTiaoDownload::closeEvent(QCloseEvent *event)
{
	if (QMessageBox::Yes == QMessageBox::question(this, TR("问题"),TR("是否退出程序？")))
	{
		event->accept();
		qApp->quit();
	}
	else
	{
		event->ignore();
	}
}

#include "stdafx.h"
#include "ScanWnd.h"

ScanWnd::ScanWnd(QWidget *parent)
	: QWidget(parent)
{
	m_leTouTiao = new QLineEdit(this);
	m_leTouTiao->setEnabled(false);

	m_btnScan = new QPushButton(TR("开始扫描"), this);
	m_btnStopScan = new QPushButton(TR("停止扫描"), this);
	m_btnStopScan->setEnabled(false);

	m_lbScanNumber = new QLabel(TR("扫描条数"), this);
	m_leScanNumber = new QLineEdit(this);


	m_chkNotOriginal = new QCheckBox(TR("非原创"), this);

	m_lbPlayCount = new QLabel(TR("播放次数(万）"), this);
	m_lePlayCount = new QLineEdit(this);

	m_ckbLoop = new QCheckBox(TR("无限扫描"), this);

	m_lbScanType = new QLabel(TR("扫描类型："), this);
	m_leScanType = new QLineEdit(this);

	m_cmbVideoType = gCreateVideoTypeComboBox(this);



	m_task = new ScanTaskManager(this);
	connect(m_task, &ScanTaskManager::sigNewInfo, this, &ScanWnd::sigNewInfo);
	connect(m_task, &ScanTaskManager::sigStopScan, this, [=]() {
		sigStopScan();
		ResetUI();

	});
	connect(m_task, &ScanTaskManager::sigScanFinish, this, [=]()
	{
		emit sigScanFinish();
		ResetUI();

	});


	InitUI();

	connect(m_btnScan, &QPushButton::clicked, this, [=]()
	{
		if (CheckUI())
		{
			SaveUI();
			SetEnabled(false);
			m_btnStopScan->setEnabled(true);

			m_task->StartScan();
			sigStartScan();
		}
		else
		{
			QMessageBox::warning(this, TR("参数设置错误"), TR("参数设置错误"));
		}
	});


	connect(m_btnStopScan, &QPushButton::clicked, this, [=]()
	{
		//SetEnabled(true);
		m_btnStopScan->setEnabled(false);

		m_task->StopScan();

	});

}

ScanWnd::~ScanWnd()
{
}

void ScanWnd::ResetUI()
{
	SetEnabled(true);
	m_btnStopScan->setEnabled(false);
}

void ScanWnd::InitUI()
{
	m_leTouTiao->setText(ScanConfig::Url());
	m_leScanNumber->setText(QString("%1").arg(ScanConfig::Number()));
	m_chkNotOriginal->setChecked(!ScanConfig::Origin());
	m_lePlayCount->setText(QString("%1").arg(ScanConfig::PlayTimes()));
	m_ckbLoop->setChecked(ScanConfig::Loop());
	m_leScanType->setText(QString("%1").arg(ScanConfig::ScanType()));
	m_cmbVideoType->setCurrentIndex(m_cmbVideoType->findData(ScanConfig::VideoType()));
}


bool ScanWnd::CheckUI()
{
	bool bok = false;
	m_leScanNumber->text().toInt(&bok);
	if (!bok) return false;

	m_lePlayCount->text().toInt(&bok);
	if (!bok) return false;


	return true;
}
void ScanWnd::SaveUI()
{
	ScanConfig::SetNumber(m_leScanNumber->text().toInt());
	ScanConfig::SetOrigin(!m_chkNotOriginal->isChecked());
	ScanConfig::SetPlayTimes(m_lePlayCount->text().toFloat());
	ScanConfig::SetLoop(m_ckbLoop->isChecked());
	ScanConfig::SetScanType(m_leScanType->text().toInt());
	ScanConfig::SetVideoType(m_cmbVideoType->currentData().toInt());

}


void ScanWnd::SetEnabled(bool enabled)
{
	m_btnStopScan->setEnabled(enabled);
	m_btnScan->setEnabled(enabled);
	m_leScanNumber->setEnabled(enabled);
	m_chkNotOriginal->setEnabled(enabled); 
	m_lePlayCount->setEnabled(enabled);
	m_ckbLoop->setEnabled(enabled);
	m_leScanType->setEnabled(enabled);


}

void ScanWnd::resizeEvent(QResizeEvent *event)
{
	const int margins = 10;
	const int margins2 = 4;
	const int btnw = 70;
	const int btnh = 24;

	int left = margins;
	int top = margins;
	int tempw = width() - 2 * btnw - 4 * margins;
	m_leTouTiao->setGeometry(left, top, tempw, btnh);
	left = width() - 2 * btnw - 2 * margins;
	m_btnScan->setGeometry(left, top, btnw, btnh);
	left = m_btnScan->geometry().right() + margins2;
	m_btnStopScan->setGeometry(left, top, btnw, btnh);

	left = margins;
	top = m_leTouTiao->geometry().bottom() + margins;
	m_lbScanNumber->setGeometry(left, top, 60, btnh);
	left = m_lbScanNumber->geometry().right() + margins2;
	m_leScanNumber->setGeometry(left, top, btnw, btnh);

	left = m_leScanNumber->geometry().right() + margins;

	//left = margins;
	//top = m_leScanNumber->geometry().bottom() + margins;
	m_chkNotOriginal->setGeometry(left, top, btnw, btnh);
	left = m_chkNotOriginal->geometry().right() + margins2;

	m_lbPlayCount->setGeometry(left, top, 80, btnh);
	left = m_lbPlayCount->geometry().right() + margins2;
	m_lePlayCount->setGeometry(left, top, btnw, btnh);

	left = m_lePlayCount->geometry().right() + margins2;
	m_ckbLoop->setGeometry(left, top, btnw, btnh);

	left = m_ckbLoop->geometry().right() + margins;
	m_lbScanType->setGeometry(left, top, 60, btnh);
	left = m_lbScanType->geometry().right() + margins2;
	m_leScanType->setGeometry(left, top, btnw, btnh);
	left = m_leScanType->geometry().right() + margins2;
	m_cmbVideoType->setGeometry(left, top, 100, btnh);


}

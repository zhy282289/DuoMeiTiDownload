#include "stdafx.h"
#include "ScanWnd.h"

ScanWnd::ScanWnd(QWidget *parent)
	: QWidget(parent)
{
	m_leTouTiao = new QLineEdit(this);
	m_leTouTiao->setEnabled(false);

	m_btnScan = new QPushButton(TR("��ʼɨ��"), this);
	m_btnStopScan = new QPushButton(TR("ֹͣɨ��"), this);
	m_btnStopScan->setEnabled(false);

	m_lbScanNumber = new QLabel(TR("ɨ������"), this);
	m_leScanNumber = new QLineEdit(this);


	m_chkNotOriginal = new QCheckBox(TR("��ԭ��"), this);

	m_lbPlayCount = new QLabel(TR("���Ŵ���(��"), this);
	m_lePlayCount = new QLineEdit(this);

	m_task = new TaskObtainManager(this);
	connect(m_task, &TaskObtainManager::sigNewInfo, this, &ScanWnd::sigNewInfo);
	connect(m_task, &TaskObtainManager::sigStopScan, this, [=]() {
		sigStopScan();
		ResetUI();

	});
	connect(m_task, &TaskObtainManager::sigScanFinish, this, [=]()
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
			QMessageBox::warning(this, TR("�������ô���"), TR("�������ô���"));
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
}


void ScanWnd::SetEnabled(bool enabled)
{
	m_btnStopScan->setEnabled(enabled);
	m_btnScan->setEnabled(enabled);
	m_leScanNumber->setEnabled(enabled);
	m_chkNotOriginal->setEnabled(enabled); 
	m_lePlayCount->setEnabled(enabled);
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
}
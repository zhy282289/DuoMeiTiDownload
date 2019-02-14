#include "stdafx.h"
#include "TaskWnd.h"

#include <thread>

#include "TaskInfo.h"
#include "DownloadManager.h"




TaskWnd::TaskWnd(QWidget *parent)
	: QWidget(parent)
	, m_bDownloading(false)
{
	m_btnDownload = new QPushButton(TR("��ʼ����"), this);
	m_btnStopDownload = new QPushButton(TR("ֹͣ����"), this);

	m_listWnd = new QListWidget(this);
	m_listWnd->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

	m_ckbTop = new QCheckBox(TR("˳��"), this);

	m_leTaskNum = new QLineEdit(this);
	m_btnTaskNum = new QPushButton(TR("��������"), this);

	m_ckbBigIcon = new QCheckBox(TR("��ͼ��"), this);
	m_btnAllTaskNum = new QPushButton(TR("��������"), this);
	m_ckbDownloadFromDB = new QCheckBox(TR("ֻ�����б�����"), this);

	InitUI();

	connect(m_btnDownload, &QPushButton::clicked, this, &TaskWnd::slotStartDownload);
	connect(m_btnStopDownload, &QPushButton::clicked, this, &TaskWnd::slotStopDownload);

	connect(m_btnTaskNum, &QPushButton::clicked, this, &TaskWnd::slotSearchTaskNumber);
	connect(m_ckbBigIcon, &QCheckBox::stateChanged, this, &TaskWnd::slotBigIconChanged);
	connect(m_btnAllTaskNum, &QPushButton::clicked, this, []() 
	{
		std::thread t([]() {
			int count = MY_DB->TaskCount();
			LOG(QString("%1:%2").arg(TR("��������������")).arg(count));
		});
		t.detach();
	});


	connect(this, &TaskWnd::sigDownloadStop, this, [=]() 
	{
		SetEnabled(true);
	});
	connect(this, &TaskWnd::sigDownloadFinish, this, [=]()
	{
		SetEnabled(true);
	});


}

TaskWnd::~TaskWnd()
{
}

void TaskWnd::StartScan()
{
	//m_task->StartScan();
	SetEnabled(false);
}

void TaskWnd::StopScan()
{
	//m_task->StopScan();
	SetEnabled(true);

}

void TaskWnd::FinishScan()
{
	SetEnabled(true);

}

void TaskWnd::AddItem(TaskInfoPtr info)
{
	QListWidgetItem *item = new QListWidgetItem(m_listWnd);
	item->setSizeHint(QSize(100, ITEM_BIG_HEIGHT));
	TaskWndListItem *w = new TaskWndListItem(item);
	w->SetInfo(info);
	w->SetNum(m_listWnd->count());
	m_listWnd->setItemWidget(item, w);

	connect(w, &TaskWndListItem::sigDelete, this, [=]() 
	{
		TaskWndListItem *item = qobject_cast<TaskWndListItem*>(sender());
		RemoveItemByWidget(item);
	});
	connect(w, &TaskWndListItem::sigConvert2Hsitory, this, [=]()
	{
		auto info = qobject_cast<TaskWndListItem*>(sender())->GetInfo();
		MY_DB->TaskRemove(info->id);
		MY_DB->HistoryInsert(info);
		RemoveItemByInfo(info);
		emit sigConvert2History(info);
	});

}



void TaskWnd::InitUI()
{
	m_ckbTop->setChecked(DownloadConfig::Order());
	m_leTaskNum->setText(QString("%1").arg(DownloadConfig::Number()));
	m_ckbDownloadFromDB->setChecked(DownloadConfig::OnlyDownloadList());

}

bool TaskWnd::CheckUI()
{
	bool bok = false;
	m_leTaskNum->text().toInt(&bok);
	if (!bok) return false;


	return true;
}
void TaskWnd::SaveUI()
{
	DownloadConfig::SetOrder(m_ckbTop->isChecked());
	DownloadConfig::SetNumber(m_leTaskNum->text().toInt());
	DownloadConfig::SetOnlyDownloadList(m_ckbDownloadFromDB->isChecked());

}


void TaskWnd::RemoveItemByWidget(QWidget *widget)
{
	for (int i = 0; i < m_listWnd->count(); ++i)
	{
		QWidget *temp = m_listWnd->itemWidget(m_listWnd->item(i));
		if (temp == widget)
		{
			delete m_listWnd->takeItem(i);
			break;
		}
	}
}

void TaskWnd::RemoveItemByInfo(TaskInfoPtr info)
{
	for (int i = 0; i < m_listWnd->count(); ++i)
	{
		TaskWndListItem *taksItem = qobject_cast<TaskWndListItem*>(m_listWnd->itemWidget(m_listWnd->item(i)));
		if (taksItem->GetInfo() == info)
		{
			delete m_listWnd->takeItem(i);
			break;
		}
	}
}

void TaskWnd::SetEnabled(bool enabled)
{
	m_btnDownload->setEnabled(enabled);
	m_btnStopDownload->setEnabled(enabled);
	m_ckbTop->setEnabled(enabled);
	m_leTaskNum->setEnabled(enabled);
	m_btnTaskNum->setEnabled(enabled);
	//m_ckbBigIcon->setEnabled(enabled);
}

void TaskWnd::slotNewInfo(TaskInfoPtr info)
{
	AddItem(info);
	
}

void TaskWnd::slotStartDownload()
{
	SetEnabled(false);
	m_btnStopDownload->setEnabled(true);

	emit sigDownloadStart();
	StartDownload();

}
void TaskWnd::StartDownload()
{
	m_bDownloading = true;

	if (m_listWnd->count() > 0)
	{
		
		TaskWndListItem *taksItem = qobject_cast<TaskWndListItem*>(m_listWnd->itemWidget(m_listWnd->item(0)));
		DownloadManager *download = new DownloadManager;
		download->Download(taksItem->GetInfo());

		connect(download, &DownloadManager::sigFinish, this, &TaskWnd::FinishDownload);

		connect(download, &DownloadManager::sigUpdateVideoUrl, this, [=](TaskInfoPtr info) {
			TaskObtainManager manager(this);
			manager.UpdateInfo(info);
		}, Qt::BlockingQueuedConnection);
	}
	else
	{
		if (!m_ckbDownloadFromDB->isChecked())
		{
			LOG(TR("��ת�����񣬻�ȡ���ݿ�����"));
			slotSearchTaskNumber();
			if (m_listWnd->count() > 0)
			{
				NextDownload();
			}
			else
			{
				StopDownload();
			}
		}
		else
		{
			LOG(TR("��ת������"));
			StopDownload();
		}


	}
}


void TaskWnd::FinishDownload(bool code, TaskInfoPtr info)
{
	bool bdb = false;
	if (code)
	{
		bdb = MY_DB->DownladInsert(info);
		if (bdb)
		{
			LOG(TR("ת��ɹ�����һ������"));
			MY_DB->TaskRemove(info->id);
			RemoveItemByInfo(info);
			sigDownloadOneFinish(info);
			NextDownload();

		}
		else
		{
			LOG(TR("���뵽�������ݿ�ʧ�ܣ� ֹͣת��"));
			emit sigDownloadStop();
		}
	}
	else
	{
		LOG(TR("ת��ʧ�ܣ���һ������"));
		MY_DB->TaskRemove(info->id);
		RemoveItemByInfo(info);
		NextDownload();
	}
}

void TaskWnd::StopDownload()
{
	LOG(TR("ֹͣ����"));
	emit sigDownloadStop();
}

void TaskWnd::slotStopDownload()
{
	m_btnStopDownload->setEnabled(false);
	if (!m_bDownloading)
		SetEnabled(true);
	
	m_bDownloading = false;
	LOG(TR("����ֹͣ����..."));

}

void TaskWnd::slotSearchTaskNumber()
{
	if (CheckUI())
	{
		SaveUI();
		m_listWnd->clear();

		int count = m_leTaskNum->text().toInt();
		bool order = m_ckbTop->isChecked();
		TaskInfos infos =  MY_DB->TaskGetUrls(0, count, order);
		for (auto info : infos)
		{
			AddItem(info);
		}
	}
	else
	{
		QMessageBox::warning(this, TR("�������ô���"), TR("�������ô���"));
	}
}


void TaskWnd::slotBigIconChanged(int state)
{
	int height = m_ckbBigIcon->isChecked() ? ITEM_BIG_HEIGHT : ITEM_SMALL_HEIGHT;
	for (int i = 0; i < m_listWnd->count(); ++i)
	{
		m_listWnd->item(i)->setSizeHint(QSize(100, height));
	}
}

void TaskWnd::NextDownload()
{
	if (m_bDownloading)
	{
		StartDownload();
	}
	else
	{
		StopDownload();

	}
}

void TaskWnd::resizeEvent(QResizeEvent *event)
{
	const int toolbarHeight = 60;
	const int margins = 10;
	const int btnw = 70;
	const int btnh = 24;


	int left = width() - 2 * btnw - 3 * margins;
	int top = margins;
	m_btnDownload->setGeometry(left, top, btnw, btnh);
	left = m_btnDownload->geometry().right() + 4;
	m_btnStopDownload->setGeometry(left, top, btnw, btnh);

	// 1
	left = margins;
	m_ckbTop->setGeometry(left, top, 50, btnh);
	left = m_ckbTop->geometry().right() + margins;
	m_leTaskNum->setGeometry(left, top, 50, btnh);
	left = m_leTaskNum->geometry().right() + margins;
	m_btnTaskNum->setGeometry(left, top, btnw, btnh);
	left = m_btnTaskNum->geometry().right() + margins;
	m_ckbBigIcon->setGeometry(left, top, 60, btnh);
	left = m_ckbBigIcon->geometry().right() + margins;
	m_btnAllTaskNum->setGeometry(left, top, btnw, btnh);
	left = m_btnAllTaskNum->geometry().right() + margins;
	m_ckbDownloadFromDB->setGeometry(left, top, 100, btnh);




	// list
	left = margins;
	top = m_ckbTop->geometry().bottom() + margins;
	m_listWnd->setGeometry(0, toolbarHeight, width(), height() - toolbarHeight);
}



//////////////////////////////////////////////////////////////////////////
TaskWndListItem::TaskWndListItem(QListWidgetItem *item)
	:QWidget(nullptr)
	, m_item(item)
{
	m_lbNum = new QLabel(this);
	m_lbNum->setAlignment(Qt::AlignCenter);

	m_desc = new QTextBrowser(this);
	m_desc->setReadOnly(true);
	//m_desc->setFrameStyle(QFrame::NoFrame);

	m_ckbSelect = new QCheckBox(this);
	m_ckbSelect->setChecked(true);
}

void TaskWndListItem::SetInfo(TaskInfoPtr info)
{
	m_info = info;
	GenerateInfo();
}

TaskInfoPtr TaskWndListItem::GetInfo()
{
	return m_info;
}

void TaskWndListItem::SetNum(int num)
{
	m_lbNum->setText(QString::number(num));
}

int TaskWndListItem::GetNum()
{
	return m_lbNum->text().toInt();
}

void TaskWndListItem::GenerateInfo()
{
	m_desc->append(m_info->title);
	m_desc->append(m_info->url);
	//m_desc->append(m_info->videoUrl);
	m_desc->append(QString("%1%2").arg(m_info->playCount).arg(TR("��")));
	m_desc->append(m_info->origin ? TR("ԭ��") : TR("��ԭ��"));
	m_desc->append(m_info->userName);
	m_desc->append(m_info->userUrl);

	if (!m_info->origin)
	{
		m_lbNum->setStyleSheet("background:rgb(60,200,30)");
	}

}

void TaskWndListItem::MenuPopup(QMouseEvent *event)
{
	if (event->button() == Qt::RightButton)
	{
		QMenu menu;
		auto actViewUrl = menu.addAction(TR("�鿴��ҳ"));
		auto actDelete = menu.addAction(TR("ɾ��"));
		actDelete->setVisible(false);
		auto actExpa = menu.addAction(TR("չ��"));
		menu.addSeparator();
		auto actConvert2History = menu.addAction(TR("��ǳ�����ʷ"));

		auto act = menu.exec(event->globalPos());
		if (act == actViewUrl)
		{
			OpenURL();
		}
		else if (act == actDelete)
		{
			if (QMessageBox::Yes == QMessageBox::question(this, TR("ɾ��"), TR("ȷ���Ƿ�ɾ��")))
			{
				if (MY_DB->TaskRemove(m_info->id))
				{
					emit sigDelete();
				}
				else
				{
					QMessageBox::warning(this, TR("����"), TR("ɾ��ʧ��"));
				}
			}

		}
		else if (actExpa == act)
		{
			ExpanItem();

		}
		else if (actConvert2History == act)
		{
			emit sigConvert2Hsitory();
		}
	}
}

void TaskWndListItem::OpenURL()
{
	QDesktopServices::openUrl(QUrl(m_info->url));

}

void TaskWndListItem::ExpanItem()
{
	int height = m_item->sizeHint().height() == ITEM_BIG_HEIGHT ? ITEM_SMALL_HEIGHT : ITEM_BIG_HEIGHT;
	m_item->setSizeHint(QSize(80, height));
}

void TaskWndListItem::LocalFile(QString filePath)
{
	QString cmd = QString("explorer.exe /select,%1").arg(filePath);
	QProcess::startDetached(cmd);
}

void TaskWndListItem::resizeEvent(QResizeEvent *event)
{
	const int margin = 2;
	int left = 0;
	m_ckbSelect->setGeometry(left, 0, 30, 30);
	left = m_ckbSelect->geometry().right();
	m_lbNum->setGeometry(left, 0, 30, height());


	left = m_lbNum->geometry().right();
	m_desc->setGeometry(left, margin, width() - margin - left, height() - margin);
}
void TaskWndListItem::mouseReleaseEvent(QMouseEvent *event)
{
	MenuPopup(event);
	QWidget::mouseReleaseEvent(event);
}
#include "stdafx.h"
#include "DownloadTaskWnd.h"
#include <thread>
#include <AutoUploadManager.h>

DownloadTaskWnd::DownloadTaskWnd(QWidget *parent)
	: QWidget(parent)
{
	m_autoUpload = nullptr;
	m_bUploading = false;

	m_listWnd = new QListWidget(this);
	m_listWnd->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

	m_ckbTop = new QCheckBox(TR("˳��"), this);

	m_leTaskNum = new QLineEdit(this);
	m_btnTaskNum = new QPushButton(TR("��������"), this);

	m_ckbBigIcon = new QCheckBox(TR("��ͼ��"), this);
	m_btnAllTaskNum = new QPushButton(TR("��������"), this);

	m_cmbVideoType = gCreateVideoTypeComboBox(this);

	m_btnAutoUpload = new QPushButton(TR("�Զ��ϴ�"), this);
	m_btnAutoUploadStop = new QPushButton(TR("ֹͣ�Զ��ϴ�"), this);

	m_autoUpload = new AutoUploadManager(this);

	InitUI();

	connect(m_btnTaskNum, &QPushButton::clicked, this, &DownloadTaskWnd::slotSearchTaskNumber);

	connect(m_ckbBigIcon, &QCheckBox::stateChanged, this, &DownloadTaskWnd::slotBigIconChanged);
	connect(m_btnAllTaskNum, &QPushButton::clicked, this, []()
	{
		std::thread t([]() {
			int count = MY_DB->DownladCount();
			LOG(QString("%1:%2").arg(TR("��������������")).arg(count));
		});
		t.detach();
	});

	connect(m_btnAutoUpload, &QPushButton::clicked, this, &DownloadTaskWnd::slotStartUpload);
	connect(m_btnAutoUploadStop, &QPushButton::clicked, this, &DownloadTaskWnd::slotStopUpload);

	connect(this, &DownloadTaskWnd::sigUploadStart, this, [=]()
	{
		SetEnabled(false);
	});
	connect(this, &DownloadTaskWnd::sigUploadStop, this, [=]()
	{
		SetEnabled(true);
	});
}

DownloadTaskWnd::~DownloadTaskWnd()
{
}


void DownloadTaskWnd::AddItem(TaskInfoPtr info)
{
	QListWidgetItem *item = new QListWidgetItem(m_listWnd);
	item->setSizeHint(QSize(100, ITEM_BIG_HEIGHT));
	auto w = new DownloadWndListItem(item);
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
		Convert2Hsitory(info);
	});

	connect(w, &TaskWndListItem::sigConvert2Task, this, [=]()
	{
		auto info = qobject_cast<TaskWndListItem*>(sender())->GetInfo();
		Convert2Task(info);
	});

	connect(w, &TaskWndListItem::sigConvert2TaskAll, this, [=]()
	{
		for (int i = 0; i < m_listWnd->count(); ++i)
		{
			auto info = qobject_cast<TaskWndListItem*>(m_listWnd->itemWidget(m_listWnd->item(i)))->GetInfo();
			MY_DB->DownladRemove(info->id);
			MY_DB->TaskInsert(info);
			emit sigConvert2Task(info);
		}
		//RemoveItemByInfo(info);
		m_listWnd->clear();

	});
}

void DownloadTaskWnd::SetEnabled(bool enabled)
{
	m_ckbTop->setEnabled(enabled);
	m_leTaskNum->setEnabled(enabled);
	m_btnTaskNum->setEnabled(enabled);
	m_btnAutoUpload->setEnabled(enabled);
}

void DownloadTaskWnd::slotSearchTaskNumber()
{
	if (CheckUI())
	{
		SaveUI();

		m_listWnd->clear();

		int count = m_leTaskNum->text().toInt();
		bool order = m_ckbTop->isChecked();
		TaskInfos infos = MY_DB->DownladGet(count, DownloadFinishConfig::VideoType(), order);
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

void DownloadTaskWnd::slotBigIconChanged(int state)
{
	int height = m_ckbBigIcon->isChecked() ? ITEM_BIG_HEIGHT : ITEM_SMALL_HEIGHT;
	for (int i = 0; i < m_listWnd->count(); ++i)
	{
		m_listWnd->item(i)->setSizeHint(QSize(100, height));
	}
}

void DownloadTaskWnd::slotStartUpload()
{
	m_bUploading = true;
	emit sigUploadStart();

	StartAutoUpload();
}

void DownloadTaskWnd::slotStopUpload()
{
	m_bUploading = false;
}

void DownloadTaskWnd::InitUI()
{
	m_ckbTop->setChecked(DownloadFinishConfig::Order());
	m_leTaskNum->setText(QString("%1").arg(DownloadFinishConfig::Number()));
	m_cmbVideoType->setCurrentIndex(m_cmbVideoType->findData(DownloadFinishConfig::VideoType()));
}

bool DownloadTaskWnd::CheckUI()
{
	bool bok = false;
	m_leTaskNum->text().toInt(&bok);
	if (!bok) return false;


	return true;
}

void DownloadTaskWnd::SaveUI()
{
	DownloadFinishConfig::SetOrder(m_ckbTop->isChecked());
	DownloadFinishConfig::SetNumber(m_leTaskNum->text().toInt());
	DownloadFinishConfig::SetVideoType(m_cmbVideoType->currentData().toInt());
}


void DownloadTaskWnd::RemoveItemByWidget(QWidget *widget)
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

void DownloadTaskWnd::RemoveItemByInfo(TaskInfoPtr info)
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

void DownloadTaskWnd::Convert2Task(TaskInfoPtr info)
{
	MY_DB->DownladRemove(info->id);
	MY_DB->TaskInsert(info);
	RemoveItemByInfo(info);
	emit sigConvert2Task(info);
}

void DownloadTaskWnd::Convert2Hsitory(TaskInfoPtr info)
{
	MY_DB->DownladRemove(info->id);
	MY_DB->HistoryInsert(info);
	RemoveItemByInfo(info);
	emit sigConvert2History(info);
}

void DownloadTaskWnd::FinishUpload(bool ret, TaskInfoPtr info)
{
	if (ret)
	{
		LOG(TR("�ϴ��ɹ�����һ���ϴ�����"));
		//Convert2Hsitory(info);
		NextUploadTask();
		

	}
	else
	{
		LOG(TR("�ϴ�ʧ�ܣ������ϴ�����"));
		NextUploadTask();

	}


}

void DownloadTaskWnd::StartAutoUpload()
{

	if (m_listWnd->count() > 0)
	{

		TaskWndListItem *taksItem = qobject_cast<TaskWndListItem*>(m_listWnd->itemWidget(m_listWnd->item(0)));
		auto info = taksItem->GetInfo();
		if (QFile::exists(info->localPath))
		{
			connect(m_autoUpload, &AutoUploadManager::sigFinish, this, &DownloadTaskWnd::FinishUpload);
			m_autoUpload->StartUpload(info);
		}
		else
		{
			Convert2Task(info);
			QTimer::singleShot(1000, [=]() {
				StartAutoUpload();
			});
		}

	}
	else
	{
		{
			LOG(TR("��ת�����񣬻�ȡ���ݿ�����"));
			slotSearchTaskNumber();
			if (m_listWnd->count() > 0)
			{
				NextUploadTask();
			}
			else
			{
				LOG(TR("��ת������ 1���Ӻ��ٻ�ȡ����"));
				QTimer::singleShot(60 * 1000, [=]() {
					StartAutoUpload();
				});
			}
		}
	}

}

void DownloadTaskWnd::NextUploadTask()
{
	if (m_bUploading)
	{
		StartAutoUpload();
	}
	else
	{
		StopUploadTask();
	}
}

void DownloadTaskWnd::StopUploadTask()
{
	emit sigUploadStop();
}

void DownloadTaskWnd::resizeEvent(QResizeEvent *event)
{
	const int toolbarHeight = 60;
	const int margins = 10;
	const int btnw = 70;
	const int btnh = 24;


	int left = width() - 2 * btnw - 3 * margins;
	int top = margins;
	//m_btnDownload->setGeometry(left, top, btnw, btnh);
	//left = m_btnDownload->geometry().right() + 4;
	//m_btnStopDownload->setGeometry(left, top, btnw, btnh);

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
	m_cmbVideoType->setGeometry(left, top, 100, btnh);
	left = m_btnAllTaskNum->geometry().right() + margins;
	m_btnAutoUpload->setGeometry(left, top, 100, btnh);
	left = m_btnAutoUpload->geometry().right() + margins;
	m_btnAutoUploadStop->setGeometry(left, top, 100, btnh);

	

	// list
	left = margins;
	top = m_ckbTop->geometry().bottom() + margins;
	m_listWnd->setGeometry(0, toolbarHeight, width(), height() - toolbarHeight);
}


////
DownloadWndListItem::DownloadWndListItem(QListWidgetItem *item)
	:TaskWndListItem(item)
{
	m_btnCopyName = new QPushButton("T", this);
	m_btnCopyPath = new QPushButton("F", this);


	connect(m_btnCopyName, &QPushButton::clicked, this, [=]() {

		QApplication::clipboard()->setText(m_info->title);
	});
	connect(m_btnCopyPath, &QPushButton::clicked, this, [=]() {
		QApplication::clipboard()->setText(m_info->localPath);

	});
}

void DownloadWndListItem::MenuPopup(QMouseEvent *event)
{
	if (event->button() == Qt::RightButton)
	{
		QMenu menu;
		auto actViewUrl = menu.addAction(TR("�鿴��ҳ"));
		//auto actDelete = menu.addAction(TR("ɾ��"));
		auto actExpa = menu.addAction(TR("չ��"));
		auto actLocalView = menu.addAction(TR("�鿴������Ƶ"));
		auto actLocalFileDirectory = menu.addAction(TR("��λ������Ƶ"));
		menu.addSeparator();
		auto actConvert2Download = menu.addAction(TR("��ǵ�ǰ�ɴ�����"));
		auto actConvert2DownloadAll = menu.addAction(TR("�������ѡ��ɴ�����"));
		auto actConvert2History = menu.addAction(TR("��ǳ�����ʷ"));

		auto act = menu.exec(event->globalPos());
		if (act == actViewUrl)
		{
			OpenURL();
		}
		else if (actExpa == act)
		{
			ExpanItem();

		}
		else if (actLocalView == act)
		{
			LocalFile(m_info->localPath);

		}
		else if (actLocalFileDirectory == act)
		{
			LocalFileDirectory(m_info->localPath);
		}
		else if (actConvert2Download == act)
		{
			sigConvert2Task();
		}
		else if (actConvert2DownloadAll == act)
		{
			sigConvert2TaskAll();
		}
		else if (actConvert2History == act)
		{
			sigConvert2Hsitory();
		}
	}
}

void DownloadWndListItem::GenerateInfo()
{
	TaskWndListItem::GenerateInfo();
	if (QFile::exists(m_info->localPath))
		m_desc->append(m_info->localPath);
	else
		m_desc->append(TR("�ļ�������"));


}

void DownloadWndListItem::resizeEvent(QResizeEvent *event)
{

	TaskWndListItem::resizeEvent(event);

	int left = 0;
	const int btnw = 22;
	int top = height() - (2 * (btnw + 10));
	m_btnCopyName->setGeometry(left, top, btnw, btnw);
	top = m_btnCopyName->geometry().bottom() + 2;
	m_btnCopyPath->setGeometry(left, top, btnw, btnw);
}

#include "stdafx.h"
#include "DownloadTaskWnd.h"
#include <thread>

DownloadTaskWnd::DownloadTaskWnd(QWidget *parent)
	: QWidget(parent)
{

	m_listWnd = new QListWidget(this);
	m_listWnd->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

	m_ckbTop = new QCheckBox(TR("顺序"), this);

	m_leTaskNum = new QLineEdit(this);
	m_btnTaskNum = new QPushButton(TR("查找任务"), this);

	m_ckbBigIcon = new QCheckBox(TR("大图标"), this);
	m_btnAllTaskNum = new QPushButton(TR("总任务数"), this);

	m_cmbVideoType = gCreateVideoTypeComboBox(this);


	InitUI();

	connect(m_btnTaskNum, &QPushButton::clicked, this, &DownloadTaskWnd::slotSearchTaskNumber);

	connect(m_ckbBigIcon, &QCheckBox::stateChanged, this, &DownloadTaskWnd::slotBigIconChanged);
	connect(m_btnAllTaskNum, &QPushButton::clicked, this, []()
	{
		std::thread t([]() {
			int count = MY_DB->DownladCount();
			LOG(QString("%1:%2").arg(TR("已下载任务总数")).arg(count));
		});
		t.detach();
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
		MY_DB->DownladRemove(info->id);
		MY_DB->HistoryInsert(info);
		RemoveItemByInfo(info);
		emit sigConvert2History(info);
	});

	connect(w, &TaskWndListItem::sigConvert2Task, this, [=]()
	{
		auto info = qobject_cast<TaskWndListItem*>(sender())->GetInfo();
		MY_DB->DownladRemove(info->id);
		MY_DB->TaskInsert(info);
		RemoveItemByInfo(info);
		emit sigConvert2Task(info);
	});
}

void DownloadTaskWnd::SetEnabled(bool enabled)
{

	m_ckbTop->setEnabled(enabled);
	m_leTaskNum->setEnabled(enabled);
	m_btnTaskNum->setEnabled(enabled);
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
		QMessageBox::warning(this, TR("参数设置错误"), TR("参数设置错误"));
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


	// list
	left = margins;
	top = m_ckbTop->geometry().bottom() + margins;
	m_listWnd->setGeometry(0, toolbarHeight, width(), height() - toolbarHeight);
}


////
DownloadWndListItem::DownloadWndListItem(QListWidgetItem *item)
	:TaskWndListItem(item)
{

}

void DownloadWndListItem::MenuPopup(QMouseEvent *event)
{
	if (event->button() == Qt::RightButton)
	{
		QMenu menu;
		auto actViewUrl = menu.addAction(TR("查看网页"));
		//auto actDelete = menu.addAction(TR("删除"));
		auto actExpa = menu.addAction(TR("展开"));
		auto actLocalView = menu.addAction(TR("查看本地视频"));
		auto actLocalFileDirectory = menu.addAction(TR("定位本地视频"));
		menu.addSeparator();
		auto actConvert2Download = menu.addAction(TR("标记成待下载"));
		auto actConvert2History = menu.addAction(TR("标记成已历史"));

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
		m_desc->append(TR("文件不存在"));


}

#include "stdafx.h"
#include "DownloadTaskWnd.h"
#include <thread>
#include <AutoUploadManager.h>


DownloadTaskWnd::DownloadTaskWnd(QWidget *parent)
	: QWidget(parent)
{
	m_uploadCount = 0;
	m_autoUpload = nullptr;
	m_bUploading = false;

	m_listWnd = new QListWidget(this);
	m_listWnd->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

	m_ckbTop = new QCheckBox(TR("顺序"), this);

	m_leTaskNum = new QLineEdit(this);
	m_btnTaskNum = new QPushButton(TR("查找任务"), this);

	m_ckbBigIcon = new QCheckBox(TR("大图标"), this);
	m_btnAllTaskNum = new QPushButton(TR("总任务数"), this);

	m_cmbVideoType = gCreateVideoTypeComboBox(this);

	m_cmbLoginType = new QComboBox(this);
	auto CreateLoginTypeFun = [=](int i) {
		m_cmbLoginType->addItem(QString(TR("账号%1")).arg(i), i);

	};
	CreateLoginTypeFun(1);
	CreateLoginTypeFun(2);
	CreateLoginTypeFun(3);
	CreateLoginTypeFun(4);
	CreateLoginTypeFun(5);


	m_ckbUploadLoop = new QCheckBox(TR("无限上传"), this);


	m_btnAutoUploadLogin = new QPushButton(TR("登陆"), this);
	m_btnAutoUpload = new QPushButton(TR("自动上传"), this);
	m_btnAutoUploadStop = new QPushButton(TR("停止自动上传"), this);

	m_lbUploadNum = new QLabel(TR("上传总数:"), this);
	m_leUploadNum = new QLineEdit(this);

	m_leUploadNum->setText("60");


	m_autoUpload = new AutoUploadManager(this);

	m_lbUploadInternalTime = new QLabel(TR("上传间隔时间:"), this);
	m_leUploadInternalTime = new QLineEdit(this);

	m_lbKeyWords = new QLabel(TR("关键词:"), this);
	m_leKeyWords = new QLineEdit(this);

	m_cmbMajorKeyWord = new QComboBox(this);
	m_cmbMajorKeyWord->addItem(TR("搞笑"), 12);
	m_cmbMajorKeyWord->addItem(TR("影视"), 1);
	m_cmbMajorKeyWord->addItem(TR("娱乐"), 13);
	m_cmbMajorKeyWord->addItem(TR("儿童"), 8);
	m_cmbMajorKeyWord->addItem(TR("亲子"), 9);

	m_btnSaveSettings = new QPushButton(TR("S"), this);

	m_leTabNum = new QLineEdit(this);
	m_leTabNum->setText("5");

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
	 
	connect(m_btnAutoUploadLogin, &QPushButton::clicked, this, &DownloadTaskWnd::slotLoginUpload);
	connect(m_btnAutoUpload, &QPushButton::clicked, this, &DownloadTaskWnd::slotStartUpload);
	connect(m_btnAutoUploadStop, &QPushButton::clicked, this, &DownloadTaskWnd::slotStopUpload);
	connect(m_btnSaveSettings, &QPushButton::clicked, this, &DownloadTaskWnd::SyncUIData);

	connect(this, &DownloadTaskWnd::sigUploadStart, this, [=]()
	{
		SetEnabled(false);
	});
	connect(this, &DownloadTaskWnd::sigUploadStop, this, [=]()
	{
		SetEnabled(true);
	});

	connect(m_cmbLoginType, SIGNAL(currentIndexChanged(int)), this, SLOT(slotLoginTypeChanged(int)));

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

	m_cmbVideoType->setEnabled(enabled);
	m_leUploadNum->setEnabled(enabled);
	//m_cmbLoginType->setEnabled(enabled);
	m_btnAutoUpload->setEnabled(enabled);
	m_btnAutoUploadLogin->setEnabled(enabled);
	m_btnAutoUploadStop->setEnabled(enabled);

	m_leKeyWords->setEnabled(enabled);
	m_cmbMajorKeyWord->setEnabled(enabled);
	m_leTabNum->setEnabled(enabled);

}

void DownloadTaskWnd::slotLoginTypeChanged(int index)
{
	InitUI();
}

void DownloadTaskWnd::slotSearchTaskNumber()
{
	if (!SyncUIData())
		return;


	m_listWnd->clear();

	int count = m_leTaskNum->text().toInt();
	bool order = m_ckbTop->isChecked();
	TaskInfos infos = MY_DB->DownladGet(count, DownloadFinishConfig::VideoType(LoginIndex()), order);
	for (auto info : infos)
	{
		AddItem(info);
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

void DownloadTaskWnd::slotLoginUpload()
{
	if (!SyncUIData())
		return;

	m_cmbLoginType->setEnabled(false);
	m_autoUpload->Login(m_cmbLoginType->currentData().toInt());
}

void DownloadTaskWnd::slotStartUpload()
{

	if (!SyncUIData())
		return;

	m_bUploading = true;
	m_uploadCount = 0;

	emit sigUploadStart();
	m_btnAutoUploadStop->setEnabled(true);
	m_cmbLoginType->setEnabled(false);
	StartAutoUpload();
}

void DownloadTaskWnd::slotStopUpload()
{
	m_bUploading = false;
	m_autoUpload->StopUpload();
	StopUploadTask();
}

void DownloadTaskWnd::InitUI()
{
	int index = LoginIndex();
	m_ckbTop->setChecked(DownloadFinishConfig::Order());
	m_leTaskNum->setText(QString("%1").arg(DownloadFinishConfig::Number()));
	m_cmbVideoType->setCurrentIndex(m_cmbVideoType->findData(DownloadFinishConfig::VideoType(index)));
	m_leUploadNum->setText(QString("%1").arg(DownloadFinishConfig::UploadNumber(index)));
	m_ckbUploadLoop->setChecked(DownloadFinishConfig::UploadLoop(index));
	m_leUploadInternalTime->setText(QString("%1").arg(DownloadFinishConfig::UploadInternalTime(index)));

	m_leKeyWords->setText(QString("%1").arg(DownloadFinishConfig::KeyWords(index)));
	m_cmbMajorKeyWord->setCurrentIndex(m_cmbMajorKeyWord->findData(DownloadFinishConfig::MajorKeyWord(index)));
	m_leTabNum->setText(QString("%1").arg(DownloadFinishConfig::TabNum(index)));

}

bool DownloadTaskWnd::CheckUI()
{
	bool bok = false;
	m_leTaskNum->text().toInt(&bok);
	if (!bok) return false;

	m_leUploadNum->text().toInt(&bok);
	if (!bok) return false;

	m_leUploadInternalTime->text().toInt(&bok);
	if (!bok) return false;

	if (m_leKeyWords->text().isEmpty())
		return false;

	m_leTabNum->text().toInt(&bok);
	if (!bok) return false;


	return true;
}

void DownloadTaskWnd::SaveUI()
{
	int index = LoginIndex();
	DownloadFinishConfig::SetOrder(m_ckbTop->isChecked());
	DownloadFinishConfig::SetNumber(m_leTaskNum->text().toInt());
	DownloadFinishConfig::SetVideoType(index, m_cmbVideoType->currentData().toInt());
	DownloadFinishConfig::SetUploadNumber(index, m_leUploadNum->text().toInt());
	DownloadFinishConfig::SetUploadLoop(index, m_ckbUploadLoop->isChecked());
	DownloadFinishConfig::SetUploadInternalTime(index, m_leUploadInternalTime->text().toInt());
	DownloadFinishConfig::SetKeyWords(index, m_leKeyWords->text());
	DownloadFinishConfig::SetMajorKeyWord(index, m_cmbMajorKeyWord->currentData().toInt());
	DownloadFinishConfig::SetTabNum(index, m_leTabNum->text().toInt());

}


bool DownloadTaskWnd::SyncUIData()
{
	if (!CheckUI())
	{
		QMessageBox::warning(this, TR("参数设置错误"), TR("参数设置错误"));
		return false;
	}

	SaveUI();

	return true;
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
		++m_uploadCount;
		Convert2Hsitory(info);

		if (ContinueTask())
		{
			int time = DownloadFinishConfig::UploadInternalTime(LoginIndex());
			//int time = qrand() % 30;
			LOG(QString(TR("上传成功，%1秒后下一个上传任务")).arg(time));
			QTimer::singleShot(time * 1000, this, &DownloadTaskWnd::NextUploadTask);
		}
		else
		{
			StopUploadTask();
		}
	}
	else
	{
		LOG(TR("上传失败，继续上传任务"));
		NextUploadTask();
	}


}

void DownloadTaskWnd::StartAutoUpload()
{
	if (ContinueTask())
	{
		if (m_listWnd->count() > 0)
		{

			TaskWndListItem *taksItem = qobject_cast<TaskWndListItem*>(m_listWnd->itemWidget(m_listWnd->item(0)));
			auto info = taksItem->GetInfo();
			if (QFile::exists(info->localPath))
			{

				LOG(QString(TR("开始第%1个上传任务，总任务数：%2")).arg(m_uploadCount+1).arg(m_leUploadNum->text().toInt()));

				connect(m_autoUpload, &AutoUploadManager::sigFinish, this, &DownloadTaskWnd::FinishUpload, Qt::UniqueConnection);
				connect(m_autoUpload, &AutoUploadManager::sigStop, this, &DownloadTaskWnd::StopUploadTask, Qt::UniqueConnection);
				m_autoUpload->StartUpload(info, m_cmbLoginType->currentData().toInt());
			}
			else
			{
				LOG(TR("文件路径不存在，1分钟后重新获取上传任务"));
				//Convert2Task(info);
				GetAndRemoveFromDBTaskCount(1);
				QTimer::singleShot(60 * 1000, this, &DownloadTaskWnd::StartAutoUpload);
			}
		}
		else
		{
			//m_autoUpload->StopUpload();

			GetAndRemoveFromDBTaskCount(1);
			if (m_listWnd->count() > 0)
			{
				NextUploadTask();
			}
			else
			{
				LOG(TR("无转码任务 1分钟后再获取任务"));
				QTimer::singleShot(60 * 1000, this, &DownloadTaskWnd::NextUploadTask);

			}

		}

	}
	else
	{
		StopUploadTask();
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

void DownloadTaskWnd::GetAndRemoveFromDBTaskCount(int count)
{
	m_listWnd->clear();
	bool order = m_ckbTop->isChecked();
	TaskInfos infos = MY_DB->DownladGet(count, DownloadFinishConfig::VideoType(LoginIndex()), order);

	for (auto info : infos)
	{
		//MY_DB->DownladRemove(info->id);
		AddItem(info);
	}
}

bool DownloadTaskWnd::ContinueTask()
{
	return m_uploadCount < m_leUploadNum->text().toInt()
		// 无限上传
		|| m_ckbUploadLoop->isChecked()
		;
}

int DownloadTaskWnd::LoginIndex()
{
	return m_cmbLoginType->currentData().toInt();
}

void DownloadTaskWnd::StopUploadTask()
{
	LOG(TR("上传任务完成"));
	emit sigUploadStop();
}

void DownloadTaskWnd::resizeEvent(QResizeEvent *event)
{
	const int toolbarHeight = 80;
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
	left = m_cmbVideoType->geometry().right() + margins;
	m_lbUploadInternalTime ->setGeometry(left, top, 80, btnh);
	left = m_lbUploadInternalTime->geometry().right() + margins;
	m_leUploadInternalTime->setGeometry(left, top, 50, btnh);
	left = m_leUploadInternalTime->geometry().right() + margins;
	m_leUploadInternalTime->setGeometry(left, top, 50, btnh);
	left = m_leUploadInternalTime->geometry().right() + margins;

	m_leTabNum->setGeometry(left, top, 30, btnh);
	left = m_leTabNum->geometry().right() + margins;

	m_leKeyWords->setGeometry(left, top, 170, btnh);
	left = m_leKeyWords->geometry().right() + margins;
	m_cmbMajorKeyWord->setGeometry(left, top, 100, btnh);
	left = m_cmbMajorKeyWord->geometry().right() + margins;
	m_btnSaveSettings->setGeometry(left, top, btnh, btnh);



	left = margins;
	top = m_cmbVideoType->geometry().bottom() + margins;
	m_lbUploadNum->setGeometry(left, top, 80, btnh);
	left = m_lbUploadNum->geometry().right() + margins;
	m_leUploadNum->setGeometry(left, top, 80, btnh);
	left = m_leUploadNum->geometry().right() + margins;
	m_ckbUploadLoop->setGeometry(left, top, 80, btnh);
	left = m_ckbUploadLoop->geometry().right() + margins;

	
	m_cmbLoginType->setGeometry(left, top, 100, btnh);
	left = m_cmbLoginType->geometry().right() + margins;
	m_btnAutoUploadLogin->setGeometry(left, top, 100, btnh);
	left = m_btnAutoUploadLogin->geometry().right() + margins;
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
	m_btnEditName = new QPushButton("E", this);
	m_btnCopyName = new QPushButton("T", this);
	m_btnCopyPath = new QPushButton("F", this);


	connect(m_btnCopyName, &QPushButton::clicked, this, [=]() {

		QApplication::clipboard()->setText(m_info->title);
	});
	connect(m_btnCopyPath, &QPushButton::clicked, this, [=]() {
		QApplication::clipboard()->setText(m_info->localPath);

	});
	connect(m_btnEditName, &QPushButton::clicked, this, [=]() {
		
		EditNameDlg dlg(this);
		dlg.SetText(m_info->title);
		if (dlg.exec())
		{
			if (m_info->title != dlg.GetText())
			{
				m_info->title = dlg.GetText();
				if (MY_DB->DownlodUpdate(m_info))
				{
					m_info->titleModify = true;
					m_desc->clear();
					GenerateInfo();
				}

			}
		}

	});
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
		auto actConvert2Download = menu.addAction(TR("标记当前成待下载"));
		auto actConvert2DownloadAll = menu.addAction(TR("标记所有选择成待下载"));
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
		m_desc->append(TR("文件不存在"));

	if (m_info->titleModify)
	{
		m_btnEditName->setStyleSheet("background-color:rgb(0,200,0)");
	}
}

void DownloadWndListItem::resizeEvent(QResizeEvent *event)
{

	TaskWndListItem::resizeEvent(event);

	int left = 0;
	const int btnw = 22;
	int top = height() - (3 * (btnw + 4));
	m_btnCopyName->setGeometry(left, top, btnw, btnw);
	top = m_btnCopyName->geometry().bottom() + 2;
	m_btnCopyPath->setGeometry(left, top, btnw, btnw);
	top = m_btnCopyPath->geometry().bottom() + 2;
	m_btnEditName->setGeometry(left, top, btnw, btnw);



}

EditNameDlg::EditNameDlg(QWidget *parent)
	:QDialog(parent)
{
	resize(600, 110);
	m_lbEdit = new QLineEdit(this);
	m_lbEdit->setMaxLength(30);
	m_btnOk = new QPushButton("OK", this);

	connect(m_btnOk, &QPushButton::clicked, this, [=]() {
		done(1);
	});

}

void EditNameDlg::SetText(QString text)
{
	m_lbEdit->setText(text);
}

QString EditNameDlg::GetText()
{
	return m_lbEdit->text();
}

void EditNameDlg::resizeEvent(QResizeEvent *event)
{
	const int margin = 20;
	int left = margin;
	int top = margin;
	m_lbEdit->setGeometry(left, top, width() - 2 * left, 22);

	m_btnOk->setGeometry(width() - 90, height() - 30, 70, 24);
}

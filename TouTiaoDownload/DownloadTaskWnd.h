#pragma once

#include <QWidget>

#include "TaskWnd.h"

class AutoUploadManager;
class DownloadTaskWnd : public QWidget
{
	Q_OBJECT

public:
	DownloadTaskWnd(QWidget *parent = nullptr);
	~DownloadTaskWnd();

	void AddItem(TaskInfoPtr info);
	void SetEnabled(bool enabled);

Q_SIGNALS:
	void sigConvert2Task(TaskInfoPtr info);
	void sigConvert2History(TaskInfoPtr info);
	void sigUploadStart();
	void sigUploadStop();
private:
	void slotSearchTaskNumber();
	void slotBigIconChanged(int state);
	void slotLoginUpload();
	void slotStartUpload();
	void slotStopUpload();
private:
	void InitUI();
	bool CheckUI();
	void SaveUI();

	void RemoveItemByWidget(QWidget *widget);
	void RemoveItemByInfo(TaskInfoPtr info);

	void Convert2Task(TaskInfoPtr info);
	void Convert2Hsitory(TaskInfoPtr info);
private:
	void FinishUpload(bool ret, TaskInfoPtr info);
private:
	void StartAutoUpload();
	void StopUploadTask();
	void NextUploadTask();

protected:
	void resizeEvent(QResizeEvent *event);

private:
	QListWidget *m_listWnd;
	QCheckBox *m_ckbTop;
	QLineEdit *m_leTaskNum;
	QPushButton *m_btnTaskNum;
	QCheckBox *m_ckbBigIcon;
	QPushButton *m_btnAllTaskNum;
	QComboBox *m_cmbVideoType;

	//////////////////////////////////////////////////////////////////////////
	
	QPushButton *m_btnAutoUploadLogin;
	QPushButton *m_btnAutoUpload;
	QPushButton *m_btnAutoUploadStop;

	AutoUploadManager *m_autoUpload;
	bool m_bUploading;
	int m_uploadCount;
};


class DownloadWndListItem : public TaskWndListItem
{
	Q_OBJECT
public:
	DownloadWndListItem(QListWidgetItem *item);

protected:
	virtual void MenuPopup(QMouseEvent *event);
	virtual void GenerateInfo();

protected:
	void resizeEvent(QResizeEvent *event);

private:
	QPushButton *m_btnCopyName;
	QPushButton *m_btnCopyPath;
};
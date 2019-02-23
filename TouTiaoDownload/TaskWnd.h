#pragma once

#include <QWidget>
#include "ScanTaskManager.h"

class TaskWndListItem;
class TaskWnd : public QWidget
{
	Q_OBJECT

public:
	TaskWnd(QWidget *parent = nullptr);
	~TaskWnd();

	void StartScan();
	void StopScan();
	void FinishScan();

	void AddItem(TaskInfoPtr info);
	void SetEnabled(bool enabled);

Q_SIGNALS:
	void sigDownloadStart();
	void sigDownloadStop();
	void sigDownloadFinish();
	void sigDownloadOneFinish(TaskInfoPtr info);
	void sigConvert2History(TaskInfoPtr info);


private:
	void slotNewInfo(TaskInfoPtr info);
	void slotStartDownload();
	void slotStopDownload();
	void slotSearchTaskNumber();
	void slotBigIconChanged(int state);

private:
	void InitUI();
	bool CheckUI();
	void SaveUI();
	void RemoveItemByWidget(QWidget *widget);
	void RemoveItemByInfo(TaskInfoPtr info);

	void StartDownload();
	void FinishDownload(int code, TaskInfoPtr info);
	void StopDownload();
	void NextDownload();

protected:
	void resizeEvent(QResizeEvent *event);

private:
	bool m_bDownloading;

	//TaskObtainManager *m_task;
	QPushButton *m_btnDownload;
	QPushButton *m_btnStopDownload;

	QListWidget *m_listWnd;
	QCheckBox *m_ckbTop;
	QLineEdit *m_leTaskNum;
	QPushButton *m_btnTaskNum;
	QCheckBox *m_ckbBigIcon;
	QPushButton *m_btnAllTaskNum;
	QCheckBox *m_ckbDownloadFromDB;

	QCheckBox *m_ckbLoop;
	QComboBox *m_cmbVideoType;

};



class TaskWndListItem : public QWidget
{
	Q_OBJECT
public:
	TaskWndListItem(QListWidgetItem *item);

	void SetInfo(TaskInfoPtr info);
	TaskInfoPtr GetInfo();

	void SetNum(int num);
	int GetNum();

	bool IsChecked();

Q_SIGNALS:
	void sigDelete();
	void sigDeleteSelected();
	void sigConvert2Task();
	void sigConvert2TaskAll();
	void sigConvert2Download();
	void sigConvert2Hsitory();
private:

protected:
	virtual void GenerateInfo();
	virtual void MenuPopup(QMouseEvent *event);

	void OpenURL();
	void ExpanItem();
	void LocalFile(QString filePath);
	void LocalFileDirectory(QString filePath);
	void CopyLocalFile(QString filePath);
protected:
	void resizeEvent(QResizeEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);

protected:
	TaskInfoPtr m_info;
	QListWidgetItem *m_item;

	QLabel *m_lbNum;
	QTextBrowser *m_desc;
	QCheckBox *m_ckbSelect;
};


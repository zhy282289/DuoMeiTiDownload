#pragma once

#include <QWidget>

#include "TaskWnd.h"

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

private:
	void slotSearchTaskNumber();
	void slotBigIconChanged(int state);
private:
	void InitUI();
	bool CheckUI();
	void SaveUI();

	void RemoveItemByWidget(QWidget *widget);
	void RemoveItemByInfo(TaskInfoPtr info);
protected:
	void resizeEvent(QResizeEvent *event);

private:
	QListWidget *m_listWnd;
	QCheckBox *m_ckbTop;
	QLineEdit *m_leTaskNum;
	QPushButton *m_btnTaskNum;
	QCheckBox *m_ckbBigIcon;
	QPushButton *m_btnAllTaskNum;

};


class DownloadWndListItem : public TaskWndListItem
{
	Q_OBJECT
public:
	DownloadWndListItem(QListWidgetItem *item);

protected:
	virtual void MenuPopup(QMouseEvent *event);
	virtual void GenerateInfo();

};
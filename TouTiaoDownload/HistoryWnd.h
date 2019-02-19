#pragma once

#include <QWidget>

#include "DownloadTaskWnd.h"

class HistoryWnd : public QWidget
{
	Q_OBJECT

public:
	HistoryWnd(QWidget *parent = nullptr);
	~HistoryWnd();

	void AddItem(TaskInfoPtr info);
	void SetEnabled(bool enabled);

Q_SIGNALS:
	void sigConvert2Task(TaskInfoPtr info);
	void sigConvert2Download(TaskInfoPtr info);

protected:
	void RemoveItemByWidget(QWidget *widget);
	void RemoveItemByInfo(TaskInfoPtr info);

	void InitUI();
	bool CheckUI();
	void SaveUI();
	void slotSearchTaskNumber();
	void slotBigIconChanged(int state);

protected:
	void resizeEvent(QResizeEvent *event);

protected:
	QListWidget *m_listWnd;
	QCheckBox *m_ckbTop;
	QLineEdit *m_leTaskNum;
	QPushButton *m_btnTaskNum;
	QCheckBox *m_ckbBigIcon;
	QPushButton *m_btnAllTaskNum;
};


class HistoryWndListItem : public DownloadWndListItem
{
	Q_OBJECT
public:
	HistoryWndListItem(QListWidgetItem *item);

Q_SIGNALS:
	void sigDeleteSrcVideo();

protected:
	virtual void MenuPopup(QMouseEvent *event);

};
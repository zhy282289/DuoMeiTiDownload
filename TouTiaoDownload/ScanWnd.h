#pragma once

#include <QWidget>

class ScanWnd : public QWidget
{
	Q_OBJECT

public:
	ScanWnd(QWidget *parent = nullptr);
	~ScanWnd();

	void ResetUI();
	void SetEnabled(bool enabled);

Q_SIGNALS:
	void sigStartScan();
	void sigStopScan();
	void sigScanFinish();

	void sigNewInfo(TaskInfoPtr info);

private:
	void InitUI();
	bool CheckUI();
	void SaveUI();
protected:
	void resizeEvent(QResizeEvent *event);

private:
	TaskObtainManager *m_task;

	QLineEdit *m_leTouTiao;

	QPushButton *m_btnScan;
	QPushButton *m_btnStopScan;

	QLabel *m_lbScanNumber;
	QLineEdit *m_leScanNumber;

	QCheckBox *m_chkNotOriginal;


	QLabel *m_lbPlayCount;
	QLineEdit *m_lePlayCount;

};

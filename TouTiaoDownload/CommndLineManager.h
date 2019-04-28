#pragma once

#include <QObject>

class CommndLineManager : public QObject
{
	Q_OBJECT

public:
	CommndLineManager(QObject *parent = nullptr);
	~CommndLineManager();

	static CommndLineManager* GetInstance();

	int GetIndex();
	void Parse();

Q_SIGNALS:
	// 自动上传信号
	void sigHaveAutoStart(int index);
	void sigStartLogin();
	void sigStartAutoUpload();

	// 扫描任务
	void sigStartScan();
	void sigStartDownload();

private:
	void AutoStart(int index);

private:
	int m_index;

};

#define COMMNDLINEMANAGER CommndLineManager::GetInstance()
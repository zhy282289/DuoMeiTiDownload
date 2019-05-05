#pragma once

#include <QObject>

class CommndLineManager : public QObject
{
	Q_OBJECT
public:
	enum AutoType{NONE, SCAN, DOWNLOAD, LOGIN, UPLOAD};
public:
	CommndLineManager(QObject *parent = nullptr);
	~CommndLineManager();

	static CommndLineManager* GetInstance();

	int GetIndex();
	void Parse();

	AutoType GetType(); 

Q_SIGNALS:
	// 自动上传信号
	void sigLoginIndex(int index);
	void sigStartAutoUpload();

	// 扫描任务
	void sigStartScan();
	// 下载任务
	void sigStartDownload();
	// 显示所有账号页面
	void sigStartLogin();
private:

private:
	int m_index;
	AutoType m_type;
};

#define COMMNDLINEMANAGER CommndLineManager::GetInstance()
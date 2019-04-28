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
	// �Զ��ϴ��ź�
	void sigHaveAutoStart(int index);
	void sigStartLogin();
	void sigStartAutoUpload();

	// ɨ������
	void sigStartScan();
	void sigStartDownload();

private:
	void AutoStart(int index);

private:
	int m_index;

};

#define COMMNDLINEMANAGER CommndLineManager::GetInstance()
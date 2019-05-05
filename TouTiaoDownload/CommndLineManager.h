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
	// �Զ��ϴ��ź�
	void sigLoginIndex(int index);
	void sigStartAutoUpload();

	// ɨ������
	void sigStartScan();
	// ��������
	void sigStartDownload();
	// ��ʾ�����˺�ҳ��
	void sigStartLogin();
private:

private:
	int m_index;
	AutoType m_type;
};

#define COMMNDLINEMANAGER CommndLineManager::GetInstance()
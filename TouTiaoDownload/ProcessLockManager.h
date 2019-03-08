#pragma once

#include <QObject>
#include <QString>
#include <QSharedMemory>

class ProcessLockManager
{
public:
	ProcessLockManager();
	~ProcessLockManager();

	static ProcessLockManager* GetInstance();

	bool Lock(QString name);
	void UnLock(QString name);

private:

	struct MemoryData{
		QSharedMemory* shared;
		bool bLock;

		MemoryData() {
			bLock = false;
		}
	};
	QMap<QString, MemoryData*> m_memory;
};

#define PROCESS_LOCK ProcessLockManager::GetInstance()
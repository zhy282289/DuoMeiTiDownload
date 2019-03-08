#include "stdafx.h"
#include "ProcessLockManager.h"

ProcessLockManager::ProcessLockManager()
{
}

ProcessLockManager::~ProcessLockManager()
{
	for (auto value : m_memory)
	{
		delete value->shared;
		delete value;
	}
}

ProcessLockManager* ProcessLockManager::GetInstance()
{
	static ProcessLockManager instance;
	return &instance;
}

bool ProcessLockManager::Lock(QString name)
{
	if (!m_memory.contains(name))
	{
		QSharedMemory *shared = new QSharedMemory(name);
		shared->create(1);
		shared->attach();
		MemoryData *data = new MemoryData;
		data->shared = shared;
		m_memory[name] = data;
	}

	bool ret = false;
	auto data = m_memory[name];
	if (!data->bLock)
	{
		ret = data->shared->lock();
		if (ret)
		{
			char *pch = (char*)data->shared->data();
			memcpy(pch, "1", 1);
			data->bLock = true;
		}
	}


	return ret;
}

void ProcessLockManager::UnLock(QString name)
{
	auto data = m_memory[name];
	Q_ASSERT(data);
	if (data->bLock)
	{
		char *pch = (char*)data->shared->data();
		memcpy(pch, "0", 1);
		data->bLock = false;
		data->shared->unlock();
	}

}

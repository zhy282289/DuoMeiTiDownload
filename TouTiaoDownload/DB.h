#pragma once

#include <QObject>

#include "TaskInfo.h"

class DB 
{
public:
	static DB* GetInstance();
	void Release();

	// task
	int TaskCount();
	bool TaskContain(QString id);
	bool TaskInsert(TaskInfo *info);
	bool TaskRemove(QString id);
	//bool TaskUpdate(TaskInfo *info);
	TaskInfos TaskGetUrls(int start, int end, bool order = true);

	// downlad
	int DownladCount();
	bool DownladInsert(TaskInfo *info);
	bool DownladRemove(QString id);
	TaskInfos DownladGet(int start, int end, bool order = true);


	// history
	int HistoryCount();
	bool HistoryInsert(TaskInfo *info);
	bool HistoryRemove(QString id);
	TaskInfos HistoryGet(int start, int end, bool order = true);


	bool Init();
public:
	DB();
private:
	int _Count(QString table);
	bool _Contain(QString table);
	bool _Insert(TaskInfo *info, QString table);
	bool _Remove(QString id, QString table);
	TaskInfos _GetUrls(QString insert_sql, int start, int end, bool order = true);

private:
	bool m_isOpen;
	QMutex m_mutex;
};

#define MY_DB DB::GetInstance()

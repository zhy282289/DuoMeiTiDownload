#pragma once

#include <QObject>
#include <QtSql/QtSql>

#include "ScanTaskManager.h"

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
	TaskInfos TaskGetUrls(int count, int videoType, bool order = true);

	// downlad
	int DownladCount();
	bool DownladContain(QString id);
	bool DownladInsert(TaskInfo *info);
	bool DownladRemove(QString id);
	bool DownlodUpdate(TaskInfo *info);
	TaskInfos DownladGet(int count, int videoType, bool order = true);


	// history
	int HistoryCount();
	bool HistoryContain(QString id);
	bool HistoryInsert(TaskInfo *info);
	bool HistoryRemove(QString id);
	TaskInfos HistoryGet(int count, int videoType, bool order = true);


	bool Init();


public:
	DB();

private:
	int _Count(QString table);
	bool _Contain(QString id, QString table);
	bool _Insert(TaskInfo *info, QString table);
	bool _Remove(QString id, QString table);
	TaskInfos _GetUrls(QString table, int count, int videoType, bool order = true);

private:
	TaskInfos _DowloadGetUrls(QString table, int count, int videoType, bool order = true);
	bool _DowloadInsert(TaskInfo *info, QString table);

private:
	bool m_isOpen;
	QMutex m_mutex;
};

#define MY_DB DB::GetInstance()

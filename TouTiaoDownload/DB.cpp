#include "stdafx.h"
#include "DB.h"

#include <QtSql/QtSql>

static DB *s_db = nullptr;
#define DB_IS_OPEN if (!m_isOpen) return 0;
#define DB_LOCK QMutexLocker locker(&m_mutex);

DB* DB::GetInstance()
{
	if (s_db == nullptr)
		s_db = new DB;
	return s_db;
}

void DB::Release()
{
	if (s_db)
	{
		delete s_db;
		s_db = nullptr;
	}
}


DB::DB()
	:m_isOpen(false)
{
	
}
bool DB::Init()
{
	QSqlDatabase database;

	database = QSqlDatabase::addDatabase("QSQLITE");

	QString dbPath = QApplication::applicationDirPath() + "/toutiao.db";
	database.setDatabaseName(dbPath);
	if (database.open())
	{
		QSqlQuery sql_query;
		bool ret = sql_query.exec("create table urls (id varchar primary key, title varchar, url varchar, origin int, playcount varchar, videourl varchar, username varchar, userurl varchar, localpath varchar, videotype varchar)");
		ret = sql_query.exec("create index urlindex on urls(id)");

		ret = sql_query.exec("create table downloadurls (id varchar primary key, title varchar, url varchar, origin int, playcount varchar, videourl varchar, username varchar, userurl varchar, localpath varchar, videotype varchar, titlemodify varchar)");
		ret = sql_query.exec("create index urlindex on downloadurls(id)");

		ret = sql_query.exec("create table historyurls (id varchar primary key, title varchar, url varchar, origin int, playcount varchar, videourl varchar, username varchar, userurl varchar, localpath varchar, videotype varchar)");
		ret = sql_query.exec("create index urlindex on historyurls(id)");

		m_isOpen = true;

		return true;
	}

	return false;
}
int DB::_Count(QString table)
{
	DB_IS_OPEN;
	DB_LOCK;
	int count = 0;
	QSqlQuery sql_query;
	QString insert_sql = QString("select count(*) from %1").arg(table);
	if (sql_query.exec(insert_sql))
	{
		while (sql_query.next())
		{
			count = sql_query.value(0).toInt();
			break;
		}
	}
	return count;
}

bool DB::_Contain(QString table)
{
	DB_IS_OPEN;
	DB_LOCK;
	int count = 0;
	QSqlQuery sql_query;
	QString insert_sql = QString("select 1 from %1 where id=?").arg(table);
	if (sql_query.exec(insert_sql))
	{
		while (sql_query.next())
		{
			count = sql_query.value(0).toInt();
			break;
		}
	}
	return count;

}

bool DB::_Insert(TaskInfo *info, QString table)
{
	DB_IS_OPEN;
	DB_LOCK;

	QSqlQuery sql_query;
	QString insert_sql = QString("insert into %1 values (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)").arg(table);
	sql_query.prepare(insert_sql);
	sql_query.addBindValue(info->id);
	sql_query.addBindValue(info->title);
	sql_query.addBindValue(info->url);
	sql_query.addBindValue(info->origin);
	sql_query.addBindValue(info->playCount);
	sql_query.addBindValue(info->videoUrl);
	sql_query.addBindValue(info->userName);
	sql_query.addBindValue(info->userUrl);
	sql_query.addBindValue(info->localPath);
	sql_query.addBindValue(info->videoType);

	if (!sql_query.exec())
	{
		qDebug() << sql_query.lastError();
		QString code = sql_query.lastError().nativeErrorCode();
		if (code == "19")
			return true;
		return false;
	}
	else
	{
		return true;
	}
}

bool DB::_Remove(QString id, QString table)
{
	DB_IS_OPEN;
	DB_LOCK;

	QSqlQuery sql_query;
	QString insert_sql = QString("delete from %1 where id=?").arg(table);
	sql_query.prepare(insert_sql);
	sql_query.addBindValue(id);
	if (sql_query.exec())
	{
		return true;
	}

	return false;
}

TaskInfos DB::_GetUrls(QString table, int count, int videoType, bool order /*= true*/)
{
	TaskInfos infos;

	if (count <= 0)
		return infos;

	QSqlQuery sql_query;
	QString queryString;
	//if (order)
	//	queryString = QString("select * from %1 limit %2,%3").arg(table).arg(start).arg(end);
	//else
	//	queryString = QString("select * from %1 order by rowid desc limit %2,%3").arg(table).arg(start).arg(end);

	if (order)
		queryString = QString("select * from %1 where videotype=%2 limit %3").arg(table).arg(videoType).arg(count);
	else
		queryString = QString("select * from %1 where videotype=%2 order by rowid desc limit %3").arg(table).arg(videoType).arg(count);


	DB_LOCK;

	if (sql_query.exec(queryString))
	{
		while (sql_query.next())
		{

			TaskInfo *info = new TaskInfo;

			info->id = sql_query.value("id").toString();
			info->title = sql_query.value("title").toString();
			info->url = sql_query.value("url").toString();
			info->origin = sql_query.value("origin").toInt();
			info->playCount = sql_query.value("playcount").toString();
			info->videoUrl = sql_query.value("videourl").toString();
			info->userName = sql_query.value("username").toString();
			info->userUrl = sql_query.value("userurl").toString();
			info->localPath = sql_query.value("localpath").toString();
			info->videoType = sql_query.value("videoType").toInt();
			infos.push_back(info);
		}
	}
	return infos;
}



int DB::TaskCount()
{
	return _Count("urls");
}


bool DB::TaskInsert(TaskInfo *info)
{
	return _Insert(info, "urls");
}



bool DB::TaskContain(QString id)
{
	return _Contain("urls");
}
bool DB::TaskRemove(QString id)
{
	return _Remove(id, "urls");
}
//
//bool DB::TaskUpdate(TaskInfo *info)
//{
//	DB_IS_OPEN;
//	DB_LOCK;
//
//	QSqlQuery sql_query;
//	QString insert_sql = "update urls set videourl=?, playcount=? where id=?";
//	sql_query.prepare(insert_sql);
//	sql_query.addBindValue(info->videoUrl);
//	sql_query.addBindValue(info->playCount);
//	sql_query.addBindValue(info->id);
//	if (sql_query.exec())
//	{
//		while (sql_query.next())
//		{
//			return true;
//		}
//	}
//
//	return false;
//}

TaskInfos DB::TaskGetUrls(int count, int videoType, bool order)
{
	return _GetUrls("urls", count, videoType, order);

}

int DB::DownladCount()
{
	return _Count("downloadurls");
}

bool DB::DownladContain(QString id)
{
	return _Contain("downloadurls");

}

bool DB::DownladInsert(TaskInfo *info)
{
	return _Insert(info, "downloadurls");
}

bool DB::DownladRemove(QString id)
{
	return _Remove(id, "downloadurls");
}

bool DB::DownlodUpdate(TaskInfo *info)
{
	DB_IS_OPEN;
	DB_LOCK;

	QSqlQuery sql_query;
	QString insert_sql = "update downloadurls set title=? where id=?";
	sql_query.prepare(insert_sql);
	sql_query.addBindValue(info->title);
	sql_query.addBindValue(info->id);
	if (sql_query.exec())
	{
		while (sql_query.next())
		{
			return true;
		}
	}

	return false;
}

TaskInfos DB::DownladGet(int count, int videoType, bool order /*= true*/)
{
	return _GetUrls("downloadurls", count, videoType, order);
}

int DB::HistoryCount()
{
	return _Count("historyurls");
}

bool DB::HistoryContain(QString id)
{
	return _Contain("historyurls");

}

bool DB::HistoryInsert(TaskInfo *info)
{
	return _Insert(info, "historyurls");

}

bool DB::HistoryRemove(QString id)
{
	return _Remove(id, "historyurls");

}

TaskInfos DB::HistoryGet(int count, int videoType, bool order /*= true*/)
{
	return _GetUrls("historyurls", count, videoType, order);
}


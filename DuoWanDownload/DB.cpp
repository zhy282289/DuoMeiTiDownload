#include "stdafx.h"
#include "DB.h"

#include <QtSql/QtSql>

static DB *s_db = nullptr;

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
	QSqlDatabase database;

	database = QSqlDatabase::addDatabase("QSQLITE");
	database.setDatabaseName("duowan_gif.db");
	if (database.open())
	{
		QSqlQuery sql_query;
		bool ret = sql_query.exec("create table gifurls (id int primary key, url varchar)");
		ret = sql_query.exec("create index urlindex on gifurls(url)");

		m_isOpen = true;
		
	}
	//QString url("ffffffffffffffff");
	//Insert(url);
	//Count();
	//Contain(url);
	//Contain("ddd");
}

bool DB::Insert(QString url)
{
	if (!m_isOpen) return false;

	QSqlQuery sql_query; 
	QString insert_sql = "insert into gifurls values (?, ?)";
	sql_query.prepare(insert_sql);
	sql_query.addBindValue(QVariant());
	sql_query.addBindValue(url);
	if (!sql_query.exec())
	{
		qDebug() << sql_query.lastError();
	}
	else
	{
		qDebug() << "inserted success!";
	}

	return false;
}

bool DB::Contain(QString url)
{
	if (!m_isOpen) return false;

	QSqlQuery sql_query;
	QString insert_sql = "select 2 from gifurls where url=?";
	sql_query.prepare(insert_sql);
	sql_query.addBindValue(url);
	if (sql_query.exec())
	{
		while (sql_query.next())
		{
			return true;
		}
	}

	return false;
}

int DB::Count()
{
	if (!m_isOpen) return 0;

	int count = 0;
	QSqlQuery sql_query;
	if (sql_query.exec("select count(*) from gifurls"))
	{
		while (sql_query.next())
		{
			count = sql_query.value(0).toInt();
			break;
		}
	}
	return count;
}


#pragma once

#include <QObject>

class DB 
{
public:
	static DB* GetInstance();
	void Release();

	bool Insert(QString url);
	bool Contain(QString url);
	int Count();

public:
	DB();

private:
	bool m_isOpen;

};

#define DB_GIF DB::GetInstance()

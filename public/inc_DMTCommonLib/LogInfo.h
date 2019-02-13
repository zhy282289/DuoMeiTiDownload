#pragma once

#include "DMTCommonlib_global.h"


class DMTCOMMONLIB_EXPORT LogInfo : public QObject
{
	Q_OBJECT

public:
	LogInfo();
	~LogInfo();

	static LogInfo* GetInstance();

	void ShowLog(QString log);
	
Q_SIGNALS:
	void sigShowLog(QString log);
private:
	Q_INVOKABLE void _ShowLog(QString log);


};

#define LOG_INFO LogInfo::GetInstance()
#define LOG(l) LOG_INFO->ShowLog(l)
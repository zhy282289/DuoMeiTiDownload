
#include "LogInfo.h"
#include <QDateTime>
#include <QDebug>

LogInfo::LogInfo()
	: QObject(nullptr)
{
}

LogInfo::~LogInfo()
{
}

LogInfo* LogInfo::GetInstance()
{
	static LogInfo instance;
	return &instance;
}

void LogInfo::ShowLog(QString log)
{
	log = QString("%1: %2").arg(QDateTime::currentDateTime().toString("yy-MM-dd hh:mm:ss")).arg(log);
	QMetaObject::invokeMethod(this, "_ShowLog", Qt::QueuedConnection, Q_ARG(QString, log));
}

Q_INVOKABLE void LogInfo::_ShowLog(QString log)
{
	qDebug() << log;
	emit sigShowLog(log);
}

#pragma once

#include <QObject>

class TodyTaskManager : public QObject
{
	Q_OBJECT

public:
	TodyTaskManager(QObject *parent);
	~TodyTaskManager();


	bool StartScan();

};

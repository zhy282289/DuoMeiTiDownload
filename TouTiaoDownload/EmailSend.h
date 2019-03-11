#pragma once

#include <QObject>

class EmailNetworkError
{
public:
	EmailNetworkError();

	static EmailNetworkError* GetInstance();

	bool SendEmail();

private:
	bool _SendEmail();
private:

	QElapsedTimer m_networkErrorTimer;

};

#define EMAIL_NETWORKERROR EmailNetworkError::GetInstance()
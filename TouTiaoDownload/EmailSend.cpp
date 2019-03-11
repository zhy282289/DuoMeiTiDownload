#include "stdafx.h"
#include "EmailSend.h"

#include "inc_PyDll/IPython_Exe.h"

EmailNetworkError::EmailNetworkError()
{
}



EmailNetworkError* EmailNetworkError::GetInstance()
{
	static EmailNetworkError instance;
	return &instance;
}

bool EmailNetworkError::SendEmail()
{
	if (!m_networkErrorTimer.isValid() || m_networkErrorTimer.elapsed() > 10 * 60 * 1000)
	{
		m_networkErrorTimer.restart();
		return _SendEmail();
	}
	return false;
}

bool EmailNetworkError::_SendEmail()
{
	QString title = QString(TR("[%1]������Ƶ���ر�����")).arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
	QString content(TR("�����������磡"));
	QString emailText = QString("%1;%2;%3").arg("396962243@qq.com").arg(title).arg(content);
	IPython_Exe *pyExe = IPython_Exe::GetInstance();
	bool ret = pyExe->Simple_Call("toutiao", "sendNetworkErrorMail", "(s)", emailText.toUtf8().data());
	ret |= pyExe->ReturnInt();
	return ret;
}

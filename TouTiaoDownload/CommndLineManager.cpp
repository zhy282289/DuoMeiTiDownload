#include "stdafx.h"
#include "CommndLineManager.h"

CommndLineManager::CommndLineManager(QObject *parent)
	: QObject(parent)
{
	m_index = 0;
}

CommndLineManager::~CommndLineManager()
{
}

static CommndLineManager *s_CommndLineParser = nullptr;
CommndLineManager* CommndLineManager::GetInstance()
{
	if (s_CommndLineParser == nullptr)
	{
		s_CommndLineParser = new CommndLineManager;
	}
	return s_CommndLineParser;
}

int CommndLineManager::GetIndex()
{
	return m_index;
}

void CommndLineManager::Parse()
{
	QCommandLineOption op1("a");
	op1.setValueName("a");

	QCommandLineOption op2("s");
	QCommandLineOption op3("d");

	QCommandLineParser parser;
	parser.addOption(op1);
	parser.addOption(op2);
	parser.addOption(op3);


	parser.process(*qApp);

	if (parser.isSet(op1))
	{
		auto index = parser.value(op1).toInt();
		AutoStart(index);
	}
	if (parser.isSet(op2))
	{
		emit sigStartScan();
	}
	if (parser.isSet(op3))
	{
		emit sigStartDownload();
	}
}

void CommndLineManager::AutoStart(int index)
{
	m_index = index;

	emit sigHaveAutoStart(m_index);

	QTimer::singleShot(m_index * 20 * 1000 + 2, [=]() {
		emit sigStartLogin();

		QTimer::singleShot(10 * 1000, [=]() {
			emit sigStartAutoUpload();
		});

	});

}

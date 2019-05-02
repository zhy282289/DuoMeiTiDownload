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
	QCommandLineOption opLoginIndex("i");
	opLoginIndex.setValueName("i");

	QCommandLineOption opAutoUpload("a");
	QCommandLineOption opStartScan("s");
	QCommandLineOption opDownload("d");
	QCommandLineOption opStartLogin("l");

	QCommandLineParser parser;
	parser.addOption(opLoginIndex);
	parser.addOption(opAutoUpload);
	parser.addOption(opStartScan);
	parser.addOption(opDownload);
	parser.addOption(opStartLogin);


	parser.process(*qApp);

#define ELAPSE_TIME 1000

	QString title;
	if (parser.isSet(opLoginIndex))
	{
		m_index = parser.value(opLoginIndex).toInt();
		title += QString(TR("ÕËºÅ%1")).arg(m_index + 1);
		QTimer::singleShot(ELAPSE_TIME, [=]() {
			emit sigLoginIndex(m_index);
		});

	}
	if (parser.isSet(opAutoUpload))
	{
		title += TR("-ÉÏ´«");
		QTimer::singleShot(ELAPSE_TIME, [=]() {
			emit sigStartAutoUpload();
		});

	}
	if (parser.isSet(opStartScan))
	{
		title += TR("-É¨Ãè");
		QTimer::singleShot(ELAPSE_TIME, [=]() {
			emit sigStartScan();
		});
	}
	if (parser.isSet(opDownload))
	{
		title += TR("-ÏÂÔØ");
		QTimer::singleShot(ELAPSE_TIME, [=]() {
			emit sigStartDownload();
		});
	}
	if (parser.isSet(opStartLogin))
	{
		title += TR("-µÇÂ½");
		QTimer::singleShot(ELAPSE_TIME, [=]() {
			emit sigStartLogin();
		});
	}
	WNDMESSAGEMANAGER->sigWindowTitleChanged(title);

}

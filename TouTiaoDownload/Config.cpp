#include "stdafx.h"
#include "Config.h"

Config::Config()
{
	m_path = QApplication::applicationDirPath() + "/config.ini";
}


Config* Config::GetInstance()
{
	static Config instance;
	return &instance;
}

QVariant Config::Value(QString key, QVariant defaultValue)
{
	QSettings settings(m_path, QSettings::IniFormat);

	return settings.value(key, defaultValue);
}

void Config::SetValue(QString key, QVariant value)
{
	QSettings settings(m_path, QSettings::IniFormat);
	settings.setValue(key, value);
}

//////////////////////////////////////////////////////////////////////////
/*

m_leTouTiao->setText(CONFIG_STR_DEF("scan/url", "https://www.ixigua.com/"));
m_leScanNumber->setText(CONFIG_STR_DEF("scan/number", "50"));
m_chkNotOriginal->setChecked(!CONFIG_INT_DEF("scan/origin", false));
m_leLessTime->setText(CONFIG_STR_DEF("scan/lesstime", "10"));
m_lePlayCount->setText(CONFIG_STR_DEF("scan/playcount", "5"));
*/
QString ScanConfig::Url()
{
	return CONFIG_STR_DEF("scan/url", "https://www.ixigua.com/");
}

int ScanConfig::Number()
{
	return CONFIG_INT_DEF("scan/number", "50");

}

void ScanConfig::SetNumber(int number)
{
	CONFIG_SET("scan/number", number);
}

bool ScanConfig::Origin()
{
	return CONFIG_BOOL_DEF("scan/origin", false);

}

void ScanConfig::SetOrigin(bool origin)
{
	CONFIG_SET("scan/origin", origin);
}

int ScanConfig::LessTime()
{
	return CONFIG_INT_DEF("scan/lesstime", 10);

}

void ScanConfig::SetLessTime(int lesstime)
{
	CONFIG_SET("scan/lesstime", lesstime);

}

int ScanConfig::PlayTimes()
{
	return CONFIG_INT_DEF("scan/playtimes", 10);

}

void ScanConfig::SetPlayTimes(int playtimes)
{
	CONFIG_SET("scan/playtimes", playtimes);

}

bool ScanConfig::Loop()
{

	return CONFIG_BOOL_DEF("scan/loop", false);
}

void ScanConfig::SetLoop(bool loop)
{
	CONFIG_SET("scan/loop", loop);

}

int ScanConfig::ScanType()
{
	return CONFIG_INT_DEF("scan/scantype", "0");

}

void ScanConfig::SetScanType(int type)
{
	CONFIG_SET("scan/scantype", type);

}

int ScanConfig::VideoType()
{
	return CONFIG_INT_DEF("scan/videotype", "0");

}

void ScanConfig::SetVideoType(int type)
{
	CONFIG_SET("scan/videotype", type);

}

bool DownloadConfig::Order()
{
	return CONFIG_BOOL_DEF("download/order", true);

}

void DownloadConfig::SetOrder(bool order)
{
	CONFIG_SET("download/order", order);

}

int DownloadConfig::Number()
{
	return CONFIG_INT_DEF("download/number", 10);

}

void DownloadConfig::SetNumber(int number)
{
	CONFIG_SET("download/number", number);

}


bool DownloadConfig::OnlyDownloadList()
{
	return CONFIG_BOOL_DEF("download/onlydownloadlist", true);

}

void DownloadConfig::SetOnlyDownloadList(bool order)
{
	CONFIG_SET("download/onlydownloadlist", order);

}

bool DownloadConfig::Loop()
{

	return CONFIG_BOOL_DEF("download/loop", false);
}

void DownloadConfig::SetLoop(bool loop)
{
	CONFIG_SET("download/loop", loop);

}

int DownloadConfig::VideoType()
{
	return CONFIG_INT_DEF("download/videotype", "0");

}

void DownloadConfig::SetVideoType(int type)
{
	CONFIG_SET("download/videotype", type);

}

QString DownloadConfig::DownloadPath()
{
	return CONFIG_STR_DEF("download/downloadpath", gGetTouTiaoResource());

}

void DownloadConfig::SetDownloadPath(QString path)
{
	CONFIG_SET("download/downloadpath", path);
}

bool DownloadFinishConfig::Order()
{
	return CONFIG_BOOL_DEF("downloadfinish/order", true);

}

void DownloadFinishConfig::SetOrder(bool order)
{
	CONFIG_SET("downloadfinish/order", order);

}

int DownloadFinishConfig::Number()
{
	return CONFIG_INT_DEF("downloadfinish/number", 10);

}

void DownloadFinishConfig::SetNumber(int number)
{
	CONFIG_SET("downloadfinish/number", number);

}

int DownloadFinishConfig::VideoType(int index)
{
	QString key = QString("downloadfinish/videotype%1").arg(index);
	return CONFIG_INT_DEF(key, 0);

}

void DownloadFinishConfig::SetVideoType(int index, int type)
{
	QString key = QString("downloadfinish/videotype%1").arg(index);
	CONFIG_SET(key, type);

}

int DownloadFinishConfig::UploadNumber(int index)
{
	QString key = QString("downloadfinish/uploadnumber%1").arg(index);
	return CONFIG_INT_DEF(key, 60);

}

void DownloadFinishConfig::SetUploadNumber(int index, int number)
{
	QString key = QString("downloadfinish/uploadnumber%1").arg(index);
	CONFIG_SET(key, number);

}
bool DownloadFinishConfig::UploadLoop(int index)
{

	QString key = QString("downloadfinish/uploadloop%1").arg(index);
	return CONFIG_BOOL_DEF(key, false);
}

void DownloadFinishConfig::SetUploadLoop(int index, bool loop)
{
	QString key = QString("downloadfinish/uploadloop%1").arg(index);
	CONFIG_SET(key, loop);

}

int DownloadFinishConfig::UploadInternalTime(int index)
{
	QString key = QString("downloadfinish/uploadinternaltime%1").arg(index);
	return CONFIG_INT_DEF(key, 10);

}

void DownloadFinishConfig::SetUploadInternalTime(int index, int time)
{
	QString key = QString("downloadfinish/uploadinternaltime%1").arg(index);
	CONFIG_SET(key, time);

}

QString DownloadFinishConfig::KeyWords(int index)
{
	QString key = QString("downloadfinish/keywords%1").arg(index);
	return CONFIG_STR_DEF(key, TR("��Ц Ӱ��"));

}

void DownloadFinishConfig::SetKeyWords(int index, QString words)
{
	QString key = QString("downloadfinish/keywords%1").arg(index);
	CONFIG_SET(key, words);

}

int DownloadFinishConfig::MajorKeyWord(int index)
{
	QString key = QString("downloadfinish/majorkeyword%1").arg(index);
	return CONFIG_INT_DEF(key, 12);

}

void DownloadFinishConfig::SetMajorKeyWord(int index, int number)
{
	QString key = QString("downloadfinish/majorkeyword%1").arg(index);
	CONFIG_SET(key, number);

}
int DownloadFinishConfig::TabNum(int index)
{
	QString key = QString("downloadfinish/tabnum%1").arg(index);
	return CONFIG_INT_DEF(key, 5);

}

void DownloadFinishConfig::SetTabNum(int index, int number)
{
	QString key = QString("downloadfinish/tabnum%1").arg(index);
	CONFIG_SET(key, number);

}
//////////////////////////////////////////////////////////////////////////
bool HistoryConfig::Order()
{
	return CONFIG_BOOL_DEF("hisotry/order", true);

}

void HistoryConfig::SetOrder(bool order)
{
	CONFIG_SET("hisotry/order", order);

}

int HistoryConfig::Number()
{
	return CONFIG_INT_DEF("hisotry/number", 10);

}

void HistoryConfig::SetNumber(int number)
{
	CONFIG_SET("hisotry/number", number);

}


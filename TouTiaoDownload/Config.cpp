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


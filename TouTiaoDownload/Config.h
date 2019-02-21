#pragma once



class Config
{
public:
	Config();
	
	static Config* GetInstance();

	QVariant Value(QString key, QVariant defaultValue = QVariant());
	void SetValue(QString key, QVariant value);

private:
	QString m_path;
};



#define CONFIG_STR_DEF(k,d) Config::GetInstance()->Value(k,d).toString()
#define CONFIG_INT_DEF(k,d) Config::GetInstance()->Value(k,d).toInt()
#define CONFIG_BOOL_DEF(k,d) Config::GetInstance()->Value(k,d).toBool()
#define CONFIG_FLOAT_DEF(k,d) Config::GetInstance()->Value(k,d).toFloat()
#define CONFIG_SET(k, v) Config::GetInstance()->SetValue(k, v)


class ScanConfig
{
public:
	static QString Url();

	static int Number();
	static void SetNumber(int number);

	static bool Origin();
	static void SetOrigin(bool origin);

	static int LessTime();
	static void SetLessTime(int lesstime);

	static int PlayTimes();
	static void SetPlayTimes(int playtimes);

	static bool Loop();
	static void SetLoop(bool loop);

	static int ScanType();
	static void SetScanType(int number);


};

class DownloadConfig
{
public:
	static bool Order();
	static void SetOrder(bool order);

	static int Number();
	static void SetNumber(int number);

	static bool OnlyDownloadList();
	static void SetOnlyDownloadList(bool order);

	static bool Loop();
	static void SetLoop(bool loop);
};

class DownloadFinishConfig
{
public:
	static bool Order();
	static void SetOrder(bool order);

	static int Number();
	static void SetNumber(int number);

};

class HistoryConfig
{
public:
	static bool Order();
	static void SetOrder(bool order);

	static int Number();
	static void SetNumber(int number);

};
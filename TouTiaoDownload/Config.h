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

	static int VideoType();
	static void SetVideoType(int type);

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

	static int VideoType();
	static void SetVideoType(int type);

	static QString DownloadPath();
	static void SetDownloadPath(QString path);


};

class DownloadFinishConfig
{
public:
	static bool Order();
	static void SetOrder(bool order);

	static int Number();
	static void SetNumber(int number);

	static int VideoType(int index);
	static void SetVideoType(int index, int type);

	static int UploadNumber(int index);
	static void SetUploadNumber(int index, int number);

	static bool UploadLoop(int index);
	static void SetUploadLoop(int index, bool loop);

	static int UploadInternalTime(int index);
	static void SetUploadInternalTime(int index, int time);

	static QString KeyWords(int index);
	static void SetKeyWords(int index, QString words);

	static int MajorKeyWord(int index);
	static void SetMajorKeyWord(int index, int number);

	static int TabNum(int number);
	static void SetTabNum(int index, int number);


};

class HistoryConfig
{
public:
	static bool Order();
	static void SetOrder(bool order);

	static int Number();
	static void SetNumber(int number);

};
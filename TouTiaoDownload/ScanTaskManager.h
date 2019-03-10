#pragma once

#include <QObject>

struct TaskInfo : public Ref
{
	QString id;
	QString title;
	QString url;
	bool origin;
	QString playCount;
	QString videoUrl;
	QString userName;
	QString userUrl;

	QString localPath;

	int videoType; //类型

	bool titleModify;

	int errorCode; // 临时使用，不存入数据库

	TaskInfo() {
		origin = false;
		videoType = 0;
		titleModify = false;
		errorCode = 0;
	}
};

typedef AutoRefPtr<TaskInfo> TaskInfoPtr;
typedef QList<TaskInfoPtr> TaskInfos;
Q_DECLARE_METATYPE(TaskInfoPtr)


class QWebEngineView;
class ScanTaskManager : public QObject
{
	Q_OBJECT
public:
	ScanTaskManager(QObject *parent = nullptr);
	~ScanTaskManager();

	bool StartScan();
	bool StopScan();
	TaskInfos GetInfos();

	bool UpdateInfo(TaskInfoPtr info);

Q_SIGNALS:
	void sigNewInfo(TaskInfoPtr info);
	void sigScanFinish();
	void sigStopScan();
protected:
	virtual void ParseMainPage();
	void _ParseMainPage();
	void ParseVideoPage(const QString &url);
	void _ParseVideoPage(const std::string &retString);


	void NextUrl();
	void NextDownload();

	void GetMore();
	bool IsUrlExistInDB(const QString &url);

	void _StopScan();
protected:
	bool m_scaning;
	bool m_stoping;
	TaskInfos m_infos;
	QString m_url;
	int m_count;
	int m_curCount;
	QWebEngineView *m_view;
	QWebEngineView *m_detailView;
	QWebEngineView *m_redownloadView;

	QStringList m_mainNewUrlist; //当前主页获取列表
	QStringList m_mainOldUrlist; 

	int m_tryGetMoreCount;
};


class KeyWordSearchScanTaskManager : public ScanTaskManager
{
	Q_OBJECT
public:
	KeyWordSearchScanTaskManager(QObject *parent = nullptr);
	~KeyWordSearchScanTaskManager();


	bool StartScan(QString url);
	bool StopScan();

private:
	void ParseMainPage();
};


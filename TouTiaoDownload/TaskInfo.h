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

	int type;
};


typedef AutoRefPtr<TaskInfo> TaskInfoPtr;
typedef QList<TaskInfoPtr> TaskInfos;
Q_DECLARE_METATYPE(TaskInfoPtr)


class QWebEngineView;
class TaskObtainManager : public QObject
{
	Q_OBJECT
public:
	TaskObtainManager(QObject *parent = nullptr);
	~TaskObtainManager();

	bool StartScan();
	bool StopScan();
	TaskInfos GetInfos();

	bool UpdateInfo(TaskInfoPtr info);

Q_SIGNALS:
	void sigNewInfo(TaskInfoPtr info);
	void sigScanFinish();
	void sigStopScan();
protected:
	void ParseMainPage();
	void _ParseMainPage();
	void ParseUrlDetailInfo(const QString &url);
	void _ParseUrlDetailInfo(const std::string &retString);


	void NextUrl();
	void NextDownload();

	void GetMore();
	bool IsUrlExistInDB(const QString &url);

	void _StopScan();
private:
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

};



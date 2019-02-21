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

	int videoType; //����
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
	void ParseMainPage();
	void _ParseMainPage();
	void ParseVideoPage(const QString &url);
	void _ParseVideoPage(const std::string &retString);


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

	QStringList m_mainNewUrlist; //��ǰ��ҳ��ȡ�б�
	QStringList m_mainOldUrlist; 

};


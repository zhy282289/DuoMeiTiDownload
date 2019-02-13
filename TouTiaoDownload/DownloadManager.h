#pragma once

#include <QObject>



class DownloadManager : public QObject
{
	Q_OBJECT

public:
	DownloadManager(QObject *parent = nullptr);
	~DownloadManager();

	bool Download(TaskInfoPtr info);
	bool Convert(QString dstpath);

Q_SIGNALS:
	void sigFinish(bool code, TaskInfoPtr info);
	void sigUpdateVideoUrl(TaskInfoPtr info);
private:
	void Finish(bool code);
private:
	TaskInfoPtr m_info;
};

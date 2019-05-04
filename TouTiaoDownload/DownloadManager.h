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
	void sigFinish(int code, TaskInfoPtr info);
	void sigUpdateVideoUrl(TaskInfoPtr info);
private:
	void Finish(int code);
private:
	TaskInfoPtr m_info;
	bool m_isDownload;
	QTimer *m_timer;
};


class ConvertManager : public QObject
{
	Q_OBJECT

public:
	ConvertManager(QObject *parent = nullptr);

	bool Convert(QString src, QString dst);



};


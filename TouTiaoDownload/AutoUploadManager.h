#pragma once

#include <QObject>

class QWebEngineView;

class AutoUploadManager : public QObject
{
	Q_OBJECT

public:
	AutoUploadManager(QObject *parent);
	~AutoUploadManager();


	bool StartUpload(TaskInfoPtr info);
	void LoadFinished();

Q_SIGNALS:
	void sigFinish(bool bok, TaskInfoPtr info);

private:
	void UploadFile();
	void MonitorUploadFileFinish();
	void UploadFileFinish();
	void UploadFileFinishEx();
	void Submit();
private:
	QWebEngineView *m_view;
	QString m_url;
	int m_tryLoadUpload;
	int m_tryLoadFinish;

	TaskInfoPtr m_info;
};

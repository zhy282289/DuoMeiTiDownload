#pragma once

#include <QObject>

class QWebEngineView;

class AutoUploadManager : public QObject
{
	Q_OBJECT

public:
	AutoUploadManager(QObject *parent);
	~AutoUploadManager();


	bool Login(int index);
	bool StartUpload(TaskInfoPtr infoint,int index);
	bool StopUpload();

Q_SIGNALS:
	void sigFinish(bool bok, TaskInfoPtr info);

private:
	void CreateWebView(int index);
	void LoadFinished();

private:
	void UploadFile();
	void MonitorUploadFileFinish();
	void UploadFileFinishEx();
	void Submit();
private:
	QWebEngineView *m_view;
	QString m_url;
	int m_tryLoadUpload;
	int m_tryLoadFinish;

	TaskInfoPtr m_info;
	bool m_bUploading;
	bool m_bInDialog;
};

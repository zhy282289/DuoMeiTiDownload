#pragma once

#include <QObject>

class QWebEngineView;

class AutoUploadManager : public QObject
{
	Q_OBJECT

public:
	AutoUploadManager(QObject *parent);
	~AutoUploadManager();


	bool Login();
	bool StartUpload(TaskInfoPtr info);
	bool StopUpload();
	void LoadFinished();

Q_SIGNALS:
	void sigFinish(bool bok, TaskInfoPtr info);

private:
	void CreateWebView();

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
	bool m_bLogin;
	bool m_bUploading;
	bool m_bInDialog;
};

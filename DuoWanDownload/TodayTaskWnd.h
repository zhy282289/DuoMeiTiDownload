#pragma once

#include <QWidget>

class TodayTaskToolBar;
class TodayTaskListWnd;
class QWebEngineView;

class TodayTaskWnd : public QWidget
{
	Q_OBJECT

public:
	TodayTaskWnd(QWidget *parent = nullptr);
	~TodayTaskWnd();

protected:
	void resizeEvent(QResizeEvent *event);

private:
	TodayTaskToolBar *m_toolBar;
	TodayTaskListWnd *m_listWnd;
};



class TodayTaskToolBar : public QWidget
{
	Q_OBJECT
public:
	TodayTaskToolBar(QWidget *parent);

private:
	void ToolBarVisible(bool visible);

protected:
	void resizeEvent(QResizeEvent *event);

private:
	QWebEngineView *m_mainView;

	QLabel *m_lbUrl;
	QLineEdit *m_leUrl;

	QLabel *m_lbUrlNum;
	QLineEdit *m_leUrlNum;

	QPushButton *m_btnStart;
	QPushButton *m_btnStop;
	QPushButton *m_btnDownlad;
	QPushButton *m_btnStopDownlad;
};

class TodayTaskInfo;
class TodayTaskListItem;
class TodayTaskListWnd : public QListWidget
{
	Q_OBJECT
public:
	TodayTaskListWnd(QWidget *parent);

private:
	void AddItem(TodayTaskInfo *info);
	void ClearItem();

	void Download();
	void StopDownload();

protected:
	void mouseReleaseEvent(QMouseEvent *e);
private:
	TodayTaskListItem *m_curDownloadItem;
};

class TodayTaskListItem : public QWidget
{
	Q_OBJECT
public:
	TodayTaskListItem(TodayTaskInfo *info, QWidget *parent = nullptr);
	TodayTaskInfo* GetInfo();

	void Download();
	void StopDownload();
private:
	void _Download(int page);
	void _StartDownload();
	void _FinishDownload();
	Q_INVOKABLE void RunJavaScript();
	Q_INVOKABLE void NextDownload();
	void GetViewPage();
	QString GetDownlaodMp4Directory();
protected:
	void resizeEvent(QResizeEvent *event);
	void mouseReleaseEvent(QMouseEvent *e);
private:
	TodayTaskInfo *m_info;
	
	QLabel *m_lbNumber;
	QLabel *m_lbTitle;
	QLabel *m_lbUrl;
	QLabel *m_lbDesc;

	QLabel *m_lbLoading;

private:
	QTimer *m_timerDownload;
	int m_totalPages;
	int m_curPages;
	QWebEngineView *m_view;
	int m_tryCount;
	bool m_bDonwloadLoading;
};

struct TodayTaskInfo
{
	int id;
	bool download;
	QString title;
	QString url;
	QStringList mp4Urls;
};

typedef QList<TodayTaskInfo*> TodayTaskInfos;


class TodayTaskScanUrlsManger: public QObject
{
	Q_OBJECT
public:
	static TodayTaskScanUrlsManger* GetInstance();

	TodayTaskScanUrlsManger(QObject *parent = nullptr);
	~TodayTaskScanUrlsManger();

	bool StartScan(QString url, int count);
	bool Stop();

	bool StartDownload();
	bool StopDownload();

	TodayTaskInfos GetInfos();

	void SaveInfo();

	void SetMainVeiw(QWebEngineView *view);
private:
	//void _StartScan(QString url, int count);
	void LoadInfoFromJson();
	void CheckUnique();
	void CheckUrlsCount();
	void ParseURLAndSave();
	bool IsTryTimesOut(int count);
Q_SIGNALS:
	void sigScan();
	void sigScanFinish(bool code);

	void sigDownload();
	void sigDownloadFinish();
	void sigDownloadFinishAll();
	void sigStopDownload();

protected:
	Q_INVOKABLE void ScanOneTimeFinish(bool code);

private:
	bool m_bRuning;
	TodayTaskInfos m_infos;
	QString m_url;
	int m_totalCount;
	QWebEngineView *m_view;
	int m_tryCount;
	bool m_startDownload;

};

class TodayTaskDownloadManger : public QObject
{
	Q_OBJECT
public:
	static TodayTaskDownloadManger* GetInstance();

	TodayTaskDownloadManger(QObject *parent = nullptr);
	~TodayTaskDownloadManger();

	bool Download(QString url);
	bool DownloadFile(QString url, QString dir);

private:


};

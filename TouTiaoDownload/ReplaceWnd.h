#pragma once

#include <QWidget>


class ReplaceWnd : public QWidget
{
	Q_OBJECT

public:
	ReplaceWnd(QWidget *parent = nullptr);
	~ReplaceWnd();


private:
	void Load();
	void Save();


protected:
	void resizeEvent(QResizeEvent *event);

protected:
	QPushButton *m_btnLoad;
	QPushButton *m_btnSave;
	QPlainTextEdit *m_info;
};


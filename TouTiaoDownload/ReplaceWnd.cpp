#include "stdafx.h"
#include "ReplaceWnd.h"

#include <thread>

ReplaceWnd::ReplaceWnd(QWidget *parent)
	: QWidget(parent)
{

	m_btnLoad = new QPushButton(TR("�����滻��"), this);
	m_btnSave = new QPushButton(TR("�����滻��"), this);

	m_info = new QPlainTextEdit(this);


	connect(m_btnLoad, &QPushButton::clicked, this, &ReplaceWnd::Load);
	connect(m_btnSave, &QPushButton::clicked, this, &ReplaceWnd::Save);


	Load();
}

ReplaceWnd::~ReplaceWnd()
{
}

void ReplaceWnd::Load()
{
	m_info->clear();
	auto &words = REPLACEWORDS_MANAGER->GetWords();
	for (auto &w : words)
	{
		m_info->appendPlainText(QString("%1=%2").arg(w.key).arg(w.value));
	}
}

void ReplaceWnd::Save()
{
	Words words;
	QString text = m_info->toPlainText();
	if (text.isEmpty())
	{
		QMessageBox::information(this, TR("��Ϣ"), TR("���ܱ���յ���Ϣ"));
	}

	if (QMessageBox::Yes == QMessageBox::question(this, TR("����"), TR("�Ƿ�ȷ������"), QMessageBox::Yes | QMessageBox::No));
	{
		QStringList texts = text.split("\n");
		for (auto &w : texts)
		{
			QStringList wlist = w.split("=");
			if (wlist.size() != 2)
			{
				wlist = w.split(TR("��"));
			}
			if (wlist.size() == 2)
			{
				Word word;
				word.key = wlist[0];
				word.value = wlist[1];
				words.push_back(word);
			}
			else
			{
				QMessageBox::information(this, TR("��Ϣ"), TR("������Ϣ��ʽ���󣬱���ʧ��"));
				return;
			}
		}

		REPLACEWORDS_MANAGER->Save(words);
	}


}

void ReplaceWnd::resizeEvent(QResizeEvent *event)
{
	const int margin = 10;
	int left = margin;
	int top = margin;
	const int btnw = 80;
	const int btnh = 22;
	m_btnLoad->setGeometry(left, top, btnw, btnh);
	left = m_btnLoad->geometry().right() + margin;
	m_btnSave->setGeometry(left, top, btnw, btnh);
	top = 60;
	m_info->setGeometry(margin, top, winId() - 2 * margin, height() - top);

}

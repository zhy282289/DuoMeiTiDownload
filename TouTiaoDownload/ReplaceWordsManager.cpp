#include "stdafx.h"
#include "ReplaceWordsManager.h"

#include <QtXml/QtXml>


#define LIMIT_SIZE 30

ReplaceWordsManager* ReplaceWordsManager::GetInstance()
{
	static ReplaceWordsManager instance;
	return &instance;
}




ReplaceWordsManager::ReplaceWordsManager()
{
	m_path = QApplication::applicationDirPath() + "/words.xml";

	Load();
}

void ReplaceWordsManager::Load()
{
	LoadReplaceWords();
	LoadSensitiveWords();
}


void ReplaceWordsManager::LoadReplaceWords()
{

	auto path = QApplication::applicationDirPath() + "/replaceWords.txt";
	QFile file(path);
	if (!file.open(QIODevice::ReadOnly))
	{
		return;
	}
	while (!file.atEnd())
	{
		auto b = file.readLine();
		auto words = b.split('=');
		if (words.size() == 2)
		{
			Word word;
			word.key = TR(words[0]);
			word.value = TR(words[1]);
			m_replaceWords.push_back(word);
		}
	}

}

void ReplaceWordsManager::LoadSensitiveWords()
{
	auto path = QApplication::applicationDirPath() + "/sensitiveWords.txt";
	QFile file(path);
	if (!file.open(QIODevice::ReadOnly))
	{
		return;
	}
	while (!file.atEnd())
	{
		auto b = file.readLine();
		if(!b.trimmed().isEmpty())
			m_sensitiveWords.push_back(TR(b));
	}
}

bool ReplaceWordsManager::IsSensitiveWord(QString title)
{
	for (auto word : m_sensitiveWords)
	{
		if (title.contains(word))
			return true;
	}

	return false;

}

bool ReplaceWordsManager::RotateReplace(QString &text)
{
	text.replace(TR("！"), TR("，"));
	text.replace(TR("!"), TR("，"));
	text.replace(TR("。"), TR("，"));
	text.replace(TR("."), TR("，"));
	text.replace(TR("?"), TR("，"));
	text.replace(TR("？"), TR("，"));
	text.replace(TR(","), TR("，"));
	QStringList ls = text.split(TR("，"));
	if (ls.size()>1)
	{
		QString newtext = QString(TR("%1，%2")).arg(ls[1]).arg(ls[0]);
		for (int i = 2; i < ls.size();++i)
		{
			newtext += TR("，") + ls[i];
		}
		text = newtext;
	}

	return true;
}

bool ReplaceWordsManager::BaseReplace(QString &text, const Word &word)
{
	QString src = text;
	text.replace(word.key, word.value);
	// 未替换
	if (src == text)
	{
		text.replace(word.value, word.key);
	}
	return src != text;
}

bool ReplaceWordsManager::DouHaoReplace(QString &text)
{
	text.replace(" ", TR("，"));

	return true;
}

bool ReplaceWordsManager::AIReplace(QString &text)
{
	QString src = text;

	//  增加搞笑
	static QString s[] = {
		TR("太搞笑了！"),
		TR("笑尿！"),
		TR("不要太搞笑"),
		TR("笑到肚子痛"),
		TR("忍不住笑"),
		TR("笑个不停"),
		TR("憋不住"),
		TR("憋不住笑"),
		TR("笑了"),
		TR("太搞笑"),
		TR("笑个不行"),
		TR("笑到脸痛"),
		TR("笑到抽筋"),
	};
	if (text.indexOf(TR("哈哈")) >0
		|| text.indexOf(TR("搞笑"))>0
		|| text.indexOf(TR("快乐"))>0
		|| text.indexOf(TR("笑"))>0
		)
	{
		int i = qrand() % (sizeof(s) / sizeof(*s));

		bool bappend = qrand() % 2;
		if (bappend)
		{
			text.append(s[i]);
		}
		else
		{
			text.push_front(s[i]);
		}

	}

	return true;
}

bool ReplaceWordsManager::SureReplace(QString src, QString &text)
{

	static QString s[] = {
		TR("无语"),
		TR("不知道说什么"),
		TR("网友无语"),
		TR("呆了"),
		TR("晕了"),
		TR("网友表示不懂"),
		TR("厉害了"),
		TR("想不通"),
		TR("这下好玩了"),
		TR("这下搞笑了"),
		TR("笑了"),
	};

	if (src == text)
	{
		int i = qrand() % (sizeof(s) / sizeof(*s));
		text.append(s[i]);
		if (text.size()>LIMIT_SIZE)
		{
			text = src;
		}
	}

	return true;
}

void ReplaceWordsManager::LimitWords(QString &text)
{
	text = text.left(LIMIT_SIZE);
}

QString ReplaceWordsManager::Replace(QString text)
{
	DouHaoReplace(text);
	QString src = text;

	RotateReplace(text);
	int count = 0;

	for (auto &word : m_replaceWords)
	{
		bool ret = BaseReplace(text, word);
		if (ret)
		{
			++count;
			if (count > 3)
			{
				break;
			}
		}
	}

	AIReplace(text);
	SureReplace(src, text);
	LimitWords(text);

	return text;
}

Words & ReplaceWordsManager::GetWords()
{
	return m_replaceWords;
}

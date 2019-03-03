#include "stdafx.h"
#include "ReplaceWordsManager.h"

#include <QtXml/QtXml>

ReplaceWordsManager* ReplaceWordsManager::GetInstance()
{
	static ReplaceWordsManager instance;
	return &instance;
}




ReplaceWordsManager::ReplaceWordsManager()
{
	m_path = QApplication::applicationDirPath() + "/words.json";

	Load();
}

void ReplaceWordsManager::Load()
{
	QDomDocument doc("words");
	QFile file(m_path);
	if (!file.open(QIODevice::ReadOnly))
		return;
	if (!doc.setContent(&file)) 
	{
		return;
	}

	auto docElem = doc.documentElement();

	auto childNodes = docElem.elementsByTagName("word");
	for (int i = 0; i < childNodes.size();++i)
	{
		Word word;
		auto elem = childNodes.at(i).toElement();
		word.key = elem.firstChildElement("key").text();
		word.value = elem.firstChildElement("value").text();
		m_words.push_back(word);
	}

}

bool ReplaceWordsManager::Save()
{

	QFile file(m_path);
	if (!file.open(QIODevice::WriteOnly))
		return false;


	QDomDocument doc("words");
	auto wordsElem = doc.createElement("words");
	doc.appendChild(wordsElem);

	for (auto &word : m_words)
	{
		auto wordElem = doc.createElement("word");
		auto keyElem = doc.createElement("key");
		keyElem.appendChild(doc.createTextNode(word.key));
		wordElem.appendChild(keyElem);

		auto valueElem = doc.createElement("value");
		valueElem.appendChild(doc.createTextNode(word.value));
		wordElem.appendChild(valueElem);

		wordsElem.appendChild(wordElem);
	}

	file.write(doc.toByteArray());
	return true;
}

void ReplaceWordsManager::Save(const Words &words)
{
	m_words = words;
	Save();
}

bool ReplaceWordsManager::BaseReplace(QString &text, const Word &word)
{
	QString src = text;
	text.replace(word.key, word.value);
	// δ�滻
	if (src == text)
	{
		text.replace(word.value, word.key);
	}
	return src != text;
}

bool ReplaceWordsManager::DouHaoReplace(QString &text)
{
	text.remove(" ");

	return true;
}

bool ReplaceWordsManager::AIReplace(QString &text)
{
	QString src = text;

	//  ���Ӹ�Ц
	QString s[] = {
		TR("̫��Ц�ˣ�"),
		TR("Ц��"),
		TR("��Ҫ̫��Ц"),
		TR("Ц������ʹ"),
		TR("�̲�סЦ"),
		TR("Ц����ͣ"),
		TR("�ﲻס"),
		TR("�ﲻסЦ"),
		TR("Ц��"),
		TR("̫��Ц"),
		TR("Ц������"),
		TR("Ц����ʹ"),
		TR("Ц�����"),
	};
	if (text.indexOf(TR("����")) 
		|| text.indexOf(TR("��Ц"))
		|| text.indexOf(TR("����"))
		|| text.indexOf(TR("Ц"))
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

		if (text.size()>30)
		{
			text = src;
		}

	}

	return true;
}

void ReplaceWordsManager::LimitWords(QString &text)
{
	text = text.left(30);
}

QString ReplaceWordsManager::Replace(QString text)
{
	DouHaoReplace(text);

	int count = 0;
	for (auto &word : m_words)
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
	LimitWords(text);

	return text;
}

Words & ReplaceWordsManager::GetWords()
{
	return m_words;
}

#pragma once

#include <QObject>


struct  Word
{
	QString key;
	QString value;
};
typedef QList<Word> Words;


class ReplaceWordsManager 
{
public:
	ReplaceWordsManager();

	static ReplaceWordsManager* GetInstance();

	QString Replace(QString text);

	Words &GetWords();
	void Save(const Words &words);
private:
	void Load();
	bool Save();

private:
	bool BaseReplace(QString &text, const Word &word);
	bool DouHaoReplace(QString &text);
	bool AIReplace(QString &text);
	void LimitWords(QString &text);
private:
	Words m_words;
	QString m_path;
};

#define REPLACEWORDS_MANAGER ReplaceWordsManager::GetInstance()
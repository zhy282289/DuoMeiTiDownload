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

	bool IsSensitiveWord(QString title);

private:
	void Load();
	void LoadReplaceWords();
	void LoadSensitiveWords();
private:
	bool RotateReplace(QString &text);
	bool BaseReplace(QString &text, const Word &word);
	bool DouHaoReplace(QString &text);
	bool AIReplace(QString &text);
	bool SureReplace(QString src, QString &text);
	void LimitWords(QString &text);

private:
	Words m_replaceWords;
	QString m_path;
	QList<QString> m_sensitiveWords;
};

#define REPLACEWORDS_MANAGER ReplaceWordsManager::GetInstance()
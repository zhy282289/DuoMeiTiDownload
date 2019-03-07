#pragma once

#include <QObject>
#include <QList>
#include <QDataStream>
#include <QtWebEngineWidgets/QtWebEngineWidgets>


Q_DECLARE_METATYPE(QList<QNetworkCookie>)

class NetworkCookie : public QObject
{
	Q_OBJECT

public:
	NetworkCookie(QWebEngineView *view, int viewIndex, QString cookieURL);
	~NetworkCookie();

private:
	void CookieAdded(const QNetworkCookie &cookie);
	void CookieRemoved(const QNetworkCookie &cookie);

private:
	void SetView(QWebEngineView *view);

	void LoadCookie();
	void SaveCookie();
	void RemoveSaveCookie(const QNetworkCookie &cookie);
private:
	QString m_cookiesPath;
	QWebEngineView *m_view;
	QList<QNetworkCookie> m_cookies;
	QString m_cookieURL;
};

QDataStream &operator << (QDataStream& stream, const QNetworkCookie& image);
QDataStream &operator >> (QDataStream& stream, QNetworkCookie& image);
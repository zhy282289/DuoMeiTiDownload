#include "stdafx.h"
#include "NetworkCookie.h"

NetworkCookie::NetworkCookie(QWebEngineView *view, int viewIndex, QString cookieURL)
	: QObject(nullptr)
	, m_view(view)
	, m_cookieURL(cookieURL)
	, m_bSave(true)
{
	QString dirName = cookieURL;
	dirName.remove("https://");
	dirName.remove("http://");
	dirName.remove(".com");
	dirName.remove(".");
	m_cookiesPath = QString("%1/cookies/%2").arg(QApplication::applicationDirPath()).arg(dirName);
	QDir().mkpath(m_cookiesPath);
	m_cookiesPath = QString("%1/cookie%2.ini").arg(m_cookiesPath).arg(viewIndex);

	qRegisterMetaTypeStreamOperators<QList<QNetworkCookie> >("QList<QNetworkCookie>");
	LoadCookie();
	m_bSave = m_cookies.isEmpty();
	SetView(m_view);
}

NetworkCookie::~NetworkCookie()
{
}

void NetworkCookie::SetView(QWebEngineView *view)
{
	Q_ASSERT(view);
	setParent(view);

	m_view = view;
	auto store = m_view->page()->profile()->cookieStore();
	store->deleteAllCookies();
	
	for (int i = 0;i<m_cookies.size(); ++i)
	{
		store->setCookie(m_cookies[i], QUrl(m_cookieURL));
	}
	
	connect(store, &QWebEngineCookieStore::cookieAdded, this, &NetworkCookie::CookieAdded, Qt::UniqueConnection);
	connect(store, &QWebEngineCookieStore::cookieRemoved, this, &NetworkCookie::CookieRemoved, Qt::UniqueConnection);

}

void NetworkCookie::CookieAdded(const QNetworkCookie &cookie)
{
	RemoveSaveCookie(cookie);
	m_cookies.push_back(cookie);
	SaveCookie();
}

void NetworkCookie::CookieRemoved(const QNetworkCookie &cookie)
{
	RemoveSaveCookie(cookie);
	SaveCookie();

}

void NetworkCookie::LoadCookie()
{
	QSettings cookieSettings(m_cookiesPath, QSettings::IniFormat);
	m_cookies = qvariant_cast<QList<QNetworkCookie>>(cookieSettings.value(("cookies")));
}

void NetworkCookie::SaveCookie()
{
	if (m_bSave)
	{
		QSettings cookieSettings(m_cookiesPath, QSettings::IniFormat);
		cookieSettings.setValue(("cookies"), QVariant::fromValue<QList<QNetworkCookie>>(m_cookies));

	}

}

void NetworkCookie::RemoveSaveCookie(const QNetworkCookie &cookie)
{
	auto iter = std::find_if(m_cookies.begin(), m_cookies.end(), [=](const QNetworkCookie &temp) {
		return temp.name() == cookie.name();
	});
	if (iter != m_cookies.end())
	{
		m_cookies.erase(iter);
	}

}

QDataStream & operator<<(QDataStream& stream, const QNetworkCookie& image)
{
	stream << image.name() << image.value();
	return stream;

}

QDataStream & operator >> (QDataStream& stream, QNetworkCookie& image)
{
	QByteArray name;
	QByteArray value;
	stream >> name >> value;
	image.setName(name);
	image.setValue(value);
	return stream;
}

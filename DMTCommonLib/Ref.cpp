#include "Ref.h"


Ref::Ref()
	:m_ref(0)
{

}

Ref::~Ref()
{

}

int Ref::AddRef()
{
	QMutexLocker locker(&m_mutex);
	return ++m_ref;
}

int Ref::Release()
{
	{
		QMutexLocker locker(&m_mutex);
		--m_ref;
	}

	if (m_ref <= 0)
		delete this;

	return m_ref;
}
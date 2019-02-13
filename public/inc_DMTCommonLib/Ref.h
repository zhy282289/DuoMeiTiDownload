#pragma once

#include "DMTCommonlib_global.h"

#include <QMutex>

class DMTCOMMONLIB_EXPORT Ref
{
public:
	Ref();
	virtual ~Ref();
	int AddRef();
	int Release();
protected:
	volatile int m_ref;
	QMutex m_mutex;
};


template <class T>
class  AutoRefPtr
{
public:
	AutoRefPtr() throw()
	{
		p = NULL;
	}
	AutoRefPtr(int nNull) throw()
	{
		(void)nNull;
		p = NULL;
	}
	AutoRefPtr(T* lp) throw()
	{
		p = lp;
		if (p != NULL)
		{
			p->AddRef();
		}
	}

	AutoRefPtr(const AutoRefPtr & src) throw()
	{
		p = src.p;
		if (p)
		{
			p->AddRef();
		}
	}

	~AutoRefPtr() throw()
	{
		if (p)
		{
			p->Release();
		}
	}

	T* operator->() const throw()
	{
		return p;
	}

	operator T*() const throw()
	{
		return p;
	}
	T& operator*() const
	{
		return *p;
	}
	//The assert on operator& usually indicates a bug.  If this is really
	//what is needed, however, take the address of the p member explicitly.
	T** operator&() throw()
	{
		//SASSERT(p == NULL);
		return &p;
	}
	bool operator!() const throw()
	{
		return (p == NULL);
	}
	bool operator<(T* pT) const throw()
	{
		return p < pT;
	}
	bool operator!=(T* pT) const
	{
		return !operator==(pT);
	}
	bool operator==(T* pT) const throw()
	{
		return p == pT;
	}

	T* operator=(T* lp) throw()
	{
		if (*this != lp)
		{
			if (p)
			{
				p->Release();
			}
			p = lp;
			if (p)
			{
				p->AddRef();
			}
		}
		return *this;
	}

	T* operator=(const AutoRefPtr<T>& lp) throw()
	{
		if (*this != lp)
		{
			if (p)
			{
				p->Release();
			}
			p = lp;
			if (p)
			{
				p->AddRef();
			}
		}
		return *this;
	}

	// Release the interface and set to NULL
	void Release() throw()
	{
		T* pTemp = p;
		if (pTemp)
		{
			p = NULL;
			pTemp->Release();
		}
	}

protected:
	T* p;
};

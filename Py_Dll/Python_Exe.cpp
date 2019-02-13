#include "Python_Exe.h"

#include <windows.h>

static Python_Exe *s_Python_Exe = nullptr;


#pragma warning(disable:4996)
IPython_Exe* IPython_Exe::GetInstance()
{
	if (!s_Python_Exe)
	{
		s_Python_Exe = new Python_Exe;
	}
	return s_Python_Exe;
}

void IPython_Exe::Release()
{
	if (s_Python_Exe)
		delete s_Python_Exe;
	s_Python_Exe = nullptr;
}

//////////////////////////////////////////////////////////////////////////

#include <time.h>
Python_Exe::Python_Exe()
{
	m_pModule = nullptr;

	m_pReturnObject = nullptr;

	Py_Initialize();

	if (Py_IsInitialized())
	{
		PyRun_SimpleString("import sys");
	}

	time_t t = time(0);
	char tmp[64]; 
	strftime(tmp, sizeof(tmp), "Python_Exe_Retrun_%Y-%m-%d %H-%M-%S.txt", localtime(&t));
	m_resultFileName = tmp;
}

Python_Exe::~Python_Exe()
{
	Py_Finalize();

}

bool Python_Exe::Simple_Call(const string &pythonName, const string &funName)
{
	PyObject* pFun = GetFunction(pythonName, funName);
	if (pFun == nullptr)
		return pFun ? true : false;

	//Py_DECREF(m_pCallFunctionObject);
	m_pReturnObject = PyObject_CallObject(pFun, nullptr);
	//Py_DECREF(pFun);

	return m_pReturnObject ? true : false;
}

bool Python_Exe::Simple_Call(const string &pythonName, const string &funName, const char *args_format, ...)
{
	PyObject* pFun = GetFunction(pythonName, funName);
	if (pFun == nullptr)
		return false;

	va_list args_list;
	va_start(args_list, args_format);
	PyObject *args = Py_VaBuildValue(const_cast<char *>(args_format), args_list);
	va_end(args_list);
	if (args == nullptr)
	{
		return false;
	}

	//Py_DECREF(m_pCallFunctionObject);
	m_pReturnObject = PyObject_CallObject(pFun, args);
	return m_pReturnObject ? true : false;
}

bool Python_Exe::AppendPyEnvPath(const string &pypath)
{

	if (!Py_IsInitialized())
		return false;

	string temp = "sys.path.append('" + pypath + "')";
	PyRun_SimpleString(temp.c_str());
	return true;

}

int Python_Exe::ReturnInt()
{
	int error = 0;
	if (m_pReturnObject)
	{
		int retTemp = PyArg_Parse(m_pReturnObject, "i", &error);

	}
	return error;
}

std::string Python_Exe::ReturnString()
{
	if (m_pReturnObject)
	{
		char *pch = nullptr;
		int retTemp = PyArg_Parse(m_pReturnObject, "s", &pch);
		return string(pch);
	}
	else
	{
		return string();
	}
}

void Python_Exe::PyRunSimpleString(const string &str)
{
	if (!str.empty())
	{
		PyRun_SimpleString(str.c_str());

	}

}


PyObject* Python_Exe::GetFunction(const string &pythonName, const string &funName)
{
	if (!Py_IsInitialized())
	{
		return nullptr;
	}
	if (m_pModule == nullptr)
	{
		m_pModule = PyImport_ImportModule(pythonName.c_str());
		if (!m_pModule)
			return nullptr;
		
	}

	//获取test模块中的hello函数
	PyObject* pFun = PyObject_GetAttrString(m_pModule, funName.c_str());
	
	return pFun;

}



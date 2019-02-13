#pragma once


#include <string>
#include <functional>
using namespace std;


# if defined(PY_DLL_EXPORTS)
#  define PY_DLL_EXPORT __declspec(dllexport)
# else
#  define PY_DLL_EXPORT __declspec(dllimport)
# endif



class PY_DLL_EXPORT IPython_Exe
{
public:
	static IPython_Exe* GetInstance();

	virtual bool Simple_Call(const string &pythonName, const string &funName) = 0;

	// ����������
	// args_format: (si)�������������,s:string, i:int
	// ����Simple_Call("test","mypirnt",(sis),"i am", 18, "age");
	virtual bool Simple_Call(const string &pythonName, const string &funName, const char *args_format, ...) = 0;

	// ����pyton�ļ�·����·��ʹ��/
	virtual bool AppendPyEnvPath(const string &pypath) = 0;
	
	// ����int����ֵ
	virtual int ReturnInt() = 0;
	virtual string ReturnString() = 0;

	virtual void PyRunSimpleString(const string &str) = 0;

	void Release();

};


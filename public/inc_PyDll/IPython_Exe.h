#pragma once

# if defined(PY_DLL_EXPORTS)
#  define PY_DLL_EXPORT __declspec(dllexport)
# else
#  define PY_DLL_EXPORT __declspec(dllimport)
# endif

#include <string>


class PY_DLL_EXPORT IPython_Exe
{
public:
	static IPython_Exe* GetInstance();
	static IPython_Exe* CreateInstance();

	virtual bool Simple_Call(const std::string &pythonName, const  std::string &funName) = 0;

	// ����������
	// args_format: (si)�������������,s:string, i:int
	// ����Simple_Call("test","mypirnt",(sis),"i am", 18, "age");
	virtual bool Simple_Call(const std::string &pythonName, const std::string &funName, const char *args_format, ...) = 0;

	// ����pyton�ļ�·����·��ʹ��/
	virtual bool AppendPyEnvPath(const std::string &pypath) = 0;
	
	// ����int����ֵ
	virtual int ReturnInt() = 0;
	virtual std::string ReturnString() = 0;

	virtual void PyRunSimpleString(const std::string &str) = 0;

	void Release();

};


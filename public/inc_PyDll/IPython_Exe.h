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

	// 带参数调用
	// args_format: (si)用括号括起参数,s:string, i:int
	// 例子Simple_Call("test","mypirnt",(sis),"i am", 18, "age");
	virtual bool Simple_Call(const string &pythonName, const string &funName, const char *args_format, ...) = 0;

	// 增加pyton文件路径，路径使用/
	virtual bool AppendPyEnvPath(const string &pypath) = 0;
	
	// 返回int返回值
	virtual int ReturnInt() = 0;
	virtual string ReturnString() = 0;

	virtual void PyRunSimpleString(const string &str) = 0;

	void Release();

};


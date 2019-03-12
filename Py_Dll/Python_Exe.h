#pragma once

#include "IPython_Exe.h"

#include "Python.h"

#include <string>
#include <functional>
#include <thread>
using namespace std;



class Python_Exe : public IPython_Exe
{
public:
	Python_Exe();
	~Python_Exe();

	bool Simple_Call(const string &pythonName, const string &funName) override;

	// 带参数调用
	// args_format: (si)用括号括起参数,s:string, i:int
	// 例子Simple_Call("test","mypirnt",(sis),"i am", 18, "age");
	bool Simple_Call(const string &pythonName, const string &funName, const char *args_format, ...) override;

	// 增加pyton文件路径，路径使用/
	bool AppendPyEnvPath(const string &pypath) override;
	
	// 返回python返回错误码
	int ReturnInt() override;
	string ReturnString() override;

	void PyRunSimpleString(const string &str) override;


private:
	PyObject* GetFunction(const string &pythonName, const string &funName);

private:
	PyObject* m_pModule;
	PyObject* m_pReturnObject;
	
};

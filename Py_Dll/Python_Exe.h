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

	// ����������
	// args_format: (si)�������������,s:string, i:int
	// ����Simple_Call("test","mypirnt",(sis),"i am", 18, "age");
	bool Simple_Call(const string &pythonName, const string &funName, const char *args_format, ...) override;

	// ����pyton�ļ�·����·��ʹ��/
	bool AppendPyEnvPath(const string &pypath) override;
	
	// ����python���ش�����
	int ReturnInt() override;
	string ReturnString() override;

	void PyRunSimpleString(const string &str) override;


private:
	PyObject* GetFunction(const string &pythonName, const string &funName);

private:
	PyObject* m_pModule;
	PyObject* m_pReturnObject;
	
};

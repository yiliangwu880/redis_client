/*
author:yiliangwu880
you can get more refer from https://github.com/yiliangwu880/svr_util.git
brief:
	迷你日志。
	写库时需要打印日志，同时给使用库的用户提供改变日志实现的选择。
	日志宏在 log_def.h.  供开发库用户使用。
	使用库用户需要自己定义新的宏来定义新的实现。
	代码无依赖，直接复制就轻易使用到你的项目。



使用库用户例子：
void MyPrintf(LogLv lv, const char * file, int line, const char *fun, const char * pattern)
{
		...
}

int main(int argc, char* argv[])
{
	LogMgr::Obj().SetLogPrinter(&MyPrintf)
}

开发库用户使用：
	直接复制到你的库，改下命名空间， 参考log_def.h定义你的日志宏，就可以使用了。

*/
#pragma once
#include <string>

namespace redis_com
{
	enum LogLv
	{
		//优先级从高到底
		LL_FATAL,
		LL_ERROR,
		LL_WARN,
		LL_INFO,
		LL_DEBUG,
		LL_TRACE //追踪BUG用，频繁打印的时候用
	};

	using PrintfCB = void(*)(LogLv lv, const char *file, int line, const char *fun, const char * pattern);

	//日志管理单例
	class LogMgr
	{
		PrintfCB m_cb = nullptr;

	public:
		static LogMgr &Obj();
		void SetLogPrinter(PrintfCB cb); //改变日志实现
		void Printf(LogLv lv, const char * file, int line, const char *fun, const char * pattern, ...);
		void Printf(LogLv lv, const char * file, int line, const char *fun, const char * pattern, va_list vp);
		void PrintfCond(LogLv lv, const char * file, int line, const char *fun, const char * cond, const char * pattern = "", ...);

	private:
		LogMgr() {};
		PrintfCB &GetPrintfCB();
		static void DefaultPrintf(LogLv lv, const char *file, int line, const char *fun, const char * pattern);
	};
}
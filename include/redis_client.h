/*
封装hiredis.h，提供简单的原始字符串命令接口
*/

#pragma once

#include "hiredis.h"
#include <string>
#include <memory> // unique_ptr
#include <async.h>



//阻塞同步方式
class RedisCon
{
private:
	redisContext *m_c = nullptr;

private:
	static void FreeReplyObject(redisReply *p);

public:
	using UNIQUE_PTR = std::unique_ptr<redisReply, decltype(&RedisCon::FreeReplyObject)>;

	~RedisCon();
	//@wait_sec 连接等待时长，秒。
	bool Init(const std::string &ip, uint16_t port, uint32_t wait_sec=5);
	//请求命令
	//返回 redisReply *的unique_ptr,   返回值作用域结束，自动释放内存
	//注意：反回值使用先判空，语法 nullptr != p.get() 
	UNIQUE_PTR Cmd(const char *format, ...);
	//@argc 参数数量
	//@argv 参数数组指针
	//@argvlen 参数数组长度列表
	UNIQUE_PTR Cmd(int argc, const char **argv, const size_t *argvlen);

};

struct event_base;
//libevent异步方式
class RedisAsynCon
{
private:
	redisAsyncContext *m_c = nullptr;
	bool m_is_can_free = true; //true表示允许用户销毁对象. 用来防止野指针
	bool m_is_del = false; //true 表示进入析构函数。 避免调用虚函数用
public:
	~RedisAsynCon();
	bool Init(event_base *base,const std::string &ip, uint16_t port);
	void Dicon();

	//@privdata 用户自定义数据，不推荐传指针，生存期难控制。
	bool Command(void *privdata, const char *format, ...);
	bool VCommand(void *privdata, const char *format, va_list ap);
	bool CommandArgv(void *privdata, int argc, const char **argv, const size_t *argvlen);

private:
	virtual void OnCon() = 0;
	virtual void OnDisCon() = 0;
	virtual void OnRev(const redisReply *reply, void *privdata) = 0; //reply 回调函数结束后，自动释放

	static void getCallback(redisAsyncContext *c, void *r, void *privdata);
	static void connectCallback(const redisAsyncContext *c, int status);
	static void disconnectCallback(const redisAsyncContext *c, int status);
};

#include "redis_client.h"
#include <adapters/libevent.h>
#include "log_def.h"

bool RedisCon::Init(const std::string &ip, uint16_t port, uint32_t wait_sec)
{
	if (nullptr != m_c)
	{
		L_ERROR("repeated Init\n");
		return false;
	}

	struct timeval timeout = { wait_sec, 0 }; 
	m_c = redisConnectWithTimeout(ip.c_str(), port, timeout);
	if (m_c == nullptr)
	{
		L_ERROR("Connection error: can't allocate redis context\n");
		return false;
	}
	if ( m_c->err) {
		L_ERROR("Connection error: %s\n", m_c->errstr);
		redisFree(m_c);
		m_c = nullptr;
		return false;
	}
	return true;
}

void RedisCon::FreeReplyObject(redisReply *p)
{
	freeReplyObject(p);
}

RedisCon::~RedisCon()
{
	redisFree(m_c); //Disconnects and frees the context 
}

RedisCon::UNIQUE_PTR RedisCon::Cmd(const char *format, ...)
{
	va_list ap;
	va_start(ap, format);

	redisReply *p = (redisReply *)redisvCommand(m_c, format, ap);
	if (nullptr == p)
	{
		return UNIQUE_PTR(nullptr, &RedisCon::FreeReplyObject);
	}
	UNIQUE_PTR unique_p(p, RedisCon::FreeReplyObject);

	va_end(ap);
	return unique_p; //对象所有权会传给返回值
}

RedisCon::UNIQUE_PTR RedisCon::Cmd(int argc, const char **argv, const size_t *argvlen)
{
	redisReply *p = (redisReply *)redisCommandArgv(m_c, argc, argv, argvlen);
	if (nullptr == p)
	{
		return UNIQUE_PTR(nullptr, &RedisCon::FreeReplyObject);
	}
	UNIQUE_PTR unique_p(p, RedisCon::FreeReplyObject);

	return unique_p; //对象所有权会传给返回值
}

RedisAsynCon::~RedisAsynCon()
{
	m_is_del = true;
	Dicon();
}

void RedisAsynCon::Dicon()
{
	if (nullptr == m_c)
	{
		return;
	}
	if (!m_is_can_free)
	{
		L_FATAL("fatal, can't free RedisAsynCon");
		*((int *)(nullptr)) = 1; //故意弄宕机，比野了好。
	}
	redisAsyncDisconnect(m_c);
	m_c = nullptr;
}

bool RedisAsynCon::Init(event_base *base, const std::string &ip, uint16_t port)
{
	if (nullptr == base || ip.empty())
	{
		return false;
	}
	m_c = redisAsyncConnect(ip.c_str(), port);
	if (m_c->err) {
		/* Let *c leak for now... */
		L_ERROR("Error: %s\n", m_c->errstr);
		redisAsyncDisconnect(m_c);
		m_c = nullptr;
		return false;
	}
	m_c->data = this;

	if (REDIS_OK != redisLibeventAttach(m_c, base))
	{
		redisAsyncDisconnect(m_c);
		m_c = nullptr;
		return false;
	}
	if (REDIS_OK != redisAsyncSetConnectCallback(m_c, connectCallback))
	{
		redisAsyncDisconnect(m_c);
		m_c = nullptr;
		return false;
	}
	if (REDIS_OK != redisAsyncSetDisconnectCallback(m_c, disconnectCallback))
	{
		redisAsyncDisconnect(m_c);
		m_c = nullptr;
		return false;
	}

	return true;
}

bool RedisAsynCon::Command(void *privdata, const char *format, ...)
{
	va_list ap;
	int status;
	va_start(ap, format);
	status = redisvAsyncCommand(m_c, getCallback, privdata, format, ap);
	va_end(ap);
	return REDIS_OK == status;
}

bool RedisAsynCon::VCommand(void *privdata, const char *format, va_list ap)
{
	int status = redisvAsyncCommand(m_c, getCallback, privdata, format, ap);
	return REDIS_OK == status;
}

bool RedisAsynCon::CommandArgv(void *privdata, int argc, const char **argv, const size_t *argvlen)
{
	int status = redisAsyncCommandArgv(m_c, getCallback, privdata, argc, argv, argvlen);
	return REDIS_OK == status;
}



void RedisAsynCon::getCallback(redisAsyncContext *c, void *r, void *privdata)
{
	RedisAsynCon *obj = (RedisAsynCon*)c->data;
	redisReply *reply = (redisReply *)r;
	obj->m_is_can_free = false; //禁止回调里面释放对象。 回调结束，系统还是引用了本对象。
	obj->OnRev(reply, privdata);
	obj->m_is_can_free = true;
}

void RedisAsynCon::connectCallback(const redisAsyncContext *c, int status)
{
	if (status != REDIS_OK) {
		L_ERROR("Error: %s\n", c->errstr);
		return;
	}
	RedisAsynCon *obj = (RedisAsynCon*)c->data;
	obj->m_is_can_free = false; //禁止回调里面释放对象。 回调结束，系统还是引用了本对象。
	obj->OnCon();
	obj->m_is_can_free = true;
}

void RedisAsynCon::disconnectCallback(const redisAsyncContext *c, int status)
{
	if (status != REDIS_OK) {
		L_ERROR("Error: %s\n", c->errstr);
		return;
	}
	RedisAsynCon *obj = (RedisAsynCon*)c->data;
	if (!obj->m_is_del) //析构函数不能调用虚函数
	{
		obj->m_is_can_free = false; //禁止回调里面释放对象。 回调结束，系统还是引用了本对象。
		obj->OnDisCon();
		obj->m_is_can_free = true;
	}
}

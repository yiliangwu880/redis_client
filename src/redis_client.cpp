#include "redis_client.h"
#include <adapters/libevent.h>

bool RedisCon::Init(const std::string &ip, uint16_t port, uint32_t wait_sec)
{
	if (nullptr != m_c)
	{
		printf("repeated Init\n");
		return false;
	}

	struct timeval timeout = { wait_sec, 0 }; 
	m_c = redisConnectWithTimeout(ip.c_str(), port, timeout);
	if (m_c == nullptr)
	{
		printf("Connection error: can't allocate redis context\n");
		return false;
	}
	if ( m_c->err) {
		printf("Connection error: %s\n", m_c->errstr);
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

RedisCon::UNIQUE_PTR RedisCon::Cmd(const std::string& cmd)
{
	redisReply *p = (redisReply *)redisCommand(m_c, cmd.c_str());
	if (nullptr == p)
	{
		return UNIQUE_PTR(nullptr, &RedisCon::FreeReplyObject);
	}
	UNIQUE_PTR unique_p(p, RedisCon::FreeReplyObject);
	return unique_p; //对象所有权会传给返回值
}

RedisAsynCon::~RedisAsynCon()
{
	Dicon();
}

void RedisAsynCon::Dicon()
{
	if (nullptr == m_c)
	{
		return;
	}
	if (!m_isCanFree)
	{
		printf("fatal, can't free RedisAsynCon");
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
		printf("Error: %s\n", m_c->errstr);
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

bool RedisAsynCon::CommandArgv(void *privdata, int argc, const char **argv, const size_t *argvlen)
{
	int status = redisAsyncCommandArgv(m_c, getCallback, privdata, argc, argv, argvlen);
	return REDIS_OK == status;
}

bool RedisAsynCon::FormattedCommand(void *privdata, const char *cmd, size_t len)
{
	int status = redisAsyncFormattedCommand(m_c, getCallback, privdata, cmd, len);
	return REDIS_OK == status;
}

void RedisAsynCon::getCallback(redisAsyncContext *c, void *r, void *privdata)
{
	RedisAsynCon *obj = (RedisAsynCon*)c->data;
	redisReply *reply = (redisReply *)r;
	obj->m_isCanFree = false; //禁止回调里面释放对象。 回调结束，系统还是引用了本对象。
	obj->OnRev(reply, privdata);
	obj->m_isCanFree = true;
}

void RedisAsynCon::connectCallback(const redisAsyncContext *c, int status)
{
	if (status != REDIS_OK) {
		printf("Error: %s\n", c->errstr);
		return;
	}
	RedisAsynCon *obj = (RedisAsynCon*)c->data;
	obj->m_isCanFree = false; //禁止回调里面释放对象。 回调结束，系统还是引用了本对象。
	obj->OnCon();
	obj->m_isCanFree = true;
}

void RedisAsynCon::disconnectCallback(const redisAsyncContext *c, int status)
{
	if (status != REDIS_OK) {
		printf("Error: %s\n", c->errstr);
		return;
	}
	RedisAsynCon *obj = (RedisAsynCon*)c->data;
	obj->m_isCanFree = false; //禁止回调里面释放对象。 回调结束，系统还是引用了本对象。
	obj->OnDisCon();
	obj->m_isCanFree = true;
}

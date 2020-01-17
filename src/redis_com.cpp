#include "redis_com.h"

using namespace std;

namespace redis_com
{
	bool RedisCom::Init(const std::string &ip, uint16_t port, uint32_t wait_sec /*= 5*/)
	{
		return m_client.Init(ip, port, wait_sec);
	}

	bool RedisCom::DelKey(const std::string &key)
	{
		//del runoobkey
		RedisArgInfo arg_info;
		RedisArgParse::Obj().DelKey(key, arg_info);
		RedisCon::UNIQUE_PTR r = m_client.Cmd(arg_info.argc, arg_info.argv, arg_info.argvlen);
		if (nullptr == r.get())
		{
			return false;
		}
		return 1 == r->integer;
	}

	bool RedisCom::SetStr(const std::string &key, const std::string &value)
	{
		RedisArgInfo arg_info;
		RedisArgParse::Obj().SetStr(key, value, arg_info);
		RedisCon::UNIQUE_PTR r = m_client.Cmd(arg_info.argc, arg_info.argv, arg_info.argvlen);
		if (nullptr == r.get())
		{
			return false;
		}
		return string("OK") == r->str;
	}

	std::string RedisCom::GetStr(const std::string &key)
	{
		//GET runoobkey
		RedisArgInfo arg_info;
		RedisArgParse::Obj().GetStr(key, arg_info);
		RedisCon::UNIQUE_PTR r = m_client.Cmd(arg_info.argc, arg_info.argv, arg_info.argvlen);
		if (nullptr == r.get())
		{
			return "";
		}
		return string(r->str, r->len);
	}

	RedisAsynConAdapter::RedisAsynConAdapter(RedisAsynCom &com)
		:m_com(com)
	{

	}

	void RedisAsynConAdapter::OnCon()
	{
		m_com.OnCon();
	}

	void RedisAsynConAdapter::OnDisCon()
	{
		m_com.OnDisCon();
	}

	void RedisAsynConAdapter::OnRev(const redisReply *reply, void *privdata)
	{
		uint64_t id = reinterpret_cast<uint64_t>(privdata);
		ReqInfo info;
		bool r = m_com.PopReqInfo(id, info);
		if (!r)
		{
			printf("err");
			return;
		}
		switch (info.method)
		{
		default:
			printf("err");
			break;
		case RAM::DelKey:
			m_com.OnDelKey(info.privdata, (1 == reply->integer));
			break;
		}
	}

	bool RedisAsynCom::Init(event_base *base, const std::string &ip, uint16_t port)
	{
		return m_client.Init(base, ip, port);
	}

	void RedisAsynCom::Dicon()
	{
		m_client.Dicon();
	}

	bool RedisAsynCom::Command(void *privdata, const char *format, ...)
	{
		va_list ap;
		va_start(ap, format);
		bool ret = m_client.VCommand(privdata, format, ap);
		va_end(ap);
		return ret;
	}

	bool RedisAsynCom::CommandArgv(void *privdata, int argc, const char **argv, const size_t *argvlen)
	{
		return m_client.CommandArgv(privdata, argc, argv, argvlen);
	}

	bool RedisAsynCom::DelKey(void *privdata, const std::string &key)
	{
		//del runoobkey
		RedisArgInfo arg_info;
		RedisArgParse::Obj().DelKey(key, arg_info);
		uint64_t id = PushReqInfo(privdata, RAM::DelKey);
		return m_client.CommandArgv((void *)id, arg_info.argc, arg_info.argv, arg_info.argvlen);
	}

	bool RedisAsynCom::SetStr(void *privdata, const std::string &key, const std::string &value)
	{
		return false;
	}

	bool RedisAsynCom::GetStr(void *privdata, const std::string &key)
	{
		return false;
	}

	bool RedisAsynCom::PopReqInfo(uint64_t id, ReqInfo &info)
	{
		for (auto it= m_req_info_list.begin(); it!= m_req_info_list.end(); ++it)
		{
			if (id == it->id)
			{
				info = *it;
				m_req_info_list.erase(it);
				return true;
			}
		}
		return false;
	}

	uint32_t RedisAsynCom::PushReqInfo(void *privdata, RAM method)
	{
		m_id_seed++;
		ReqInfo d;
		d.id = m_id_seed;
		d.method = method;
		d.privdata = privdata;
		m_req_info_list.push_back(d);
		return m_id_seed;
	}

}
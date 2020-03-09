#include "log_def.h"
#include "redis_com.h"
#include "redis_parse_cmd.h"

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
		RedisArgParse arg_info;
		arg_info.DelKey(key);
		RedisCon::UNIQUE_PTR r = m_client.Cmd(arg_info.argc, arg_info.argv, arg_info.argvlen);
		if (nullptr == r.get())
		{
			return false;
		}
		return 1 == r->integer;
	}

	bool RedisCom::SetStr(const std::string &key, const std::string &value)
	{
		RedisArgParse arg_info;
		arg_info.SetStr(key, value);
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
		RedisArgParse arg_info;
		arg_info.GetStr(key);
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
			L_ERROR("PopReqInfo fail. id=%lld", id);
			return;
		}
		switch (info.method)
		{
		default:
			L_ERROR("unknow method: %d", info.method);
			break;
		case RAM::DelKey:
			m_com.OnDelKey(info.privdata, (1 == reply->integer));
			break;
		case RAM::SetStr:
			m_com.OnSetStr(info.privdata, (string("OK") == reply->str));
			break;
		case RAM::GetStr:
			m_com.OnGetStr(info.privdata, string(reply->str, reply->len));
			break;
		case RAM::SetHash:
			m_com.OnSetHash(info.privdata, (string("OK") == reply->str));
			break;
		case RAM::GetHash:
			m_com.OnGetHash(info.privdata, string(reply->str, reply->len));
			break;
		case RAM::GetAllHash:
			{
				redis_com::VecFieldValue vec;
				if (reply->type != REDIS_REPLY_ARRAY) {
					L_ERROR("GetAllHash fail. id=%lld", id);
					return;
				}

				uint32_t idx = 0;
				redis_com::VecFieldValue::value_type v;
				for (uint32_t i = 0; i < reply->elements; i++)
				{
					if (idx % 2 == 0)
					{
						v.first = reply->element[i]->str;
					}
					else
					{
						v.second = reply->element[i]->str;
						vec.push_back(v);
					}
				}
				m_com.OnGetAllHash(info.privdata, vec);
			}
			break;
		}
	}

	RedisAsynCom::RedisAsynCom()
		:m_client(*this)
	{

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
		RedisArgParse arg_info;
		arg_info.DelKey(key);
		uint64_t id = PushReqInfo(privdata, RAM::DelKey);
		return m_client.CommandArgv((void *)id, arg_info.argc, arg_info.argv, arg_info.argvlen);
	}

	bool RedisAsynCom::SetStr(void *privdata, const std::string &key, const std::string &value)
	{
		RedisArgParse arg_info;
		arg_info.SetStr(key, value);
		uint64_t id = PushReqInfo(privdata, RAM::SetStr);
		return m_client.CommandArgv((void *)id, arg_info.argc, arg_info.argv, arg_info.argvlen);
	}

	bool RedisAsynCom::GetStr(void *privdata, const std::string &key)
	{
		RedisArgParse arg_info;
		arg_info.GetStr(key);
		uint64_t id = PushReqInfo(privdata, RAM::GetStr);
		return m_client.CommandArgv((void *)id, arg_info.argc, arg_info.argv, arg_info.argvlen);
	}

	bool RedisAsynCom::SetHash(void *privdata, const std::string &key, const redis_com::VecFieldValue &vec_field_value)
	{
		RedisArgParse arg_info;
		arg_info.SetHash(key, vec_field_value);
		uint64_t id = PushReqInfo(privdata, RAM::SetHash);
		return m_client.CommandArgv((void *)id, arg_info.argc, arg_info.argv, arg_info.argvlen);
	}

	bool RedisAsynCom::SetHash(void *privdata, const std::string &key, const std::string &field, const std::string &value)
	{
		RedisArgParse arg_info;
		arg_info.SetHash(key, field, value);
		uint64_t id = PushReqInfo(privdata, RAM::SetHash);
		return m_client.CommandArgv((void *)id, arg_info.argc, arg_info.argv, arg_info.argvlen);
	}

	bool RedisAsynCom::GetHash(void *privdata, const std::string &key, const std::string &field)
	{
		RedisArgParse arg_info;
		arg_info.GetHash(key, field);
		uint64_t id = PushReqInfo(privdata, RAM::GetHash);
		return m_client.CommandArgv((void *)id, arg_info.argc, arg_info.argv, arg_info.argvlen);
	}

	bool RedisAsynCom::GetAllHash(void *privdata, const std::string &key)
	{
		RedisArgParse arg_info;
		arg_info.GetAllHash(key);
		uint64_t id = PushReqInfo(privdata, RAM::GetAllHash);
		return m_client.CommandArgv((void *)id, arg_info.argc, arg_info.argv, arg_info.argvlen);
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
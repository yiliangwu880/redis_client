#include "redis_parse_cmd.h"
#include <cstring>
#include "log_def.h"

using namespace std;

RedisArgParse & RedisArgParse::Obj()
{
	static RedisArgParse obj;
	return obj;
}

void RedisArgParse::DelKey(const std::string &key, RedisArgInfo &arg_info)
{
	m_vec_pchar.clear();
	m_vec_len.clear();

	m_vec_pchar.push_back("del");
	m_vec_len  .push_back(3);
	m_vec_pchar.push_back(key.c_str());
	m_vec_len  .push_back(key.length());

	InitRedisArgInfo(arg_info);
}

void RedisArgParse::SetStr(const std::string &key, const std::string &value, RedisArgInfo &arg_info)
{
	//SET runoobkey redis

	m_vec_pchar.clear();
	m_vec_len  .clear();

	m_vec_pchar.push_back("set");
	m_vec_len  .push_back(3);
	m_vec_pchar.push_back(key.c_str());
	m_vec_len  .push_back(key.length());
	m_vec_pchar.push_back(value.c_str());
	m_vec_len  .push_back(value.length());

	InitRedisArgInfo(arg_info);
}

void RedisArgParse::SetHash(const std::string &key, const VecFieldValue &vec_field_value, RedisArgInfo &arg_info)
{
	//HMSET runoobkey name1 v1 name2 v2

	m_vec_pchar.clear();
	m_vec_len.clear();

	m_vec_pchar    .push_back("HMSET");
	m_vec_len      .push_back(5);
	m_vec_pchar    .push_back(key.c_str());
	m_vec_len      .push_back(key.length());
	for (const auto &v: vec_field_value)
	{
		m_vec_pchar.push_back(v.first.c_str());
		m_vec_len  .push_back(v.first.length());
		m_vec_pchar.push_back(v.second.c_str());
		m_vec_len  .push_back(v.second.length());
	}

	InitRedisArgInfo(arg_info);
}

void RedisArgParse::SetHash(const std::string &key, const std::string &field, const std::string &value, RedisArgInfo &arg_info)
{
	//HMSET runoobkey name1 v1 name2 v2

	m_vec_pchar.clear();
	m_vec_len.clear();

	m_vec_pchar    .push_back("HMSET");
	m_vec_len      .push_back(5);
	m_vec_pchar    .push_back(key.c_str());
	m_vec_len      .push_back(key.length());

	m_vec_pchar.push_back(field.c_str());
	m_vec_len.push_back(field.length());
	m_vec_pchar.push_back(value.c_str());
	m_vec_len.push_back(value.length());

	InitRedisArgInfo(arg_info);
}

void RedisArgParse::GetStr(const std::string &key, RedisArgInfo &arg_info)
{
	m_vec_pchar.clear();
	m_vec_len.clear();

	m_vec_pchar.push_back("get");
	m_vec_len  .push_back(3);
	m_vec_pchar.push_back(key.c_str());
	m_vec_len  .push_back(key.length());

	InitRedisArgInfo(arg_info);
}

void RedisArgParse::InitRedisArgInfo(RedisArgInfo &arg_info)
{
	arg_info.argc = m_vec_pchar.size();
	arg_info.argv = &(m_vec_pchar[0]);
	arg_info.argvlen = &(m_vec_len[0]);

}

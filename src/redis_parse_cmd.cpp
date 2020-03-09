#include "redis_parse_cmd.h"
#include <cstring>
#include "log_def.h"

using namespace std;
using namespace redis_com;

void RedisArgParse::DelKey(const std::string &key)
{
	m_vec_pchar.clear();
	m_vec_len.clear();

	PushPara("del");
	PushPara(key);

	InitRedisArgInfo();
	
}

void RedisArgParse::SetStr(const std::string &key, const std::string &value)
{
	//SET runoobkey redis

	m_vec_pchar.clear();
	m_vec_len  .clear();

	PushPara("set");
	PushPara(key);
	PushPara(value);

	InitRedisArgInfo();
	
}

void  RedisArgParse::SetHash(const std::string &key, const VecFieldValue &vec_field_value)
{
	//HMSET runoobkey name1 v1 name2 v2

	m_vec_pchar.clear();
	m_vec_len.clear();


	PushPara("HMSET");
	PushPara(key);
	for (const auto &v: vec_field_value)
	{
		PushPara(v.first);
		PushPara(v.second);
	}

	InitRedisArgInfo();
	
}

void  RedisArgParse::SetHash(const std::string &key, const std::string &field, const std::string &value)
{
	//HMSET key field1 v1  field2 v2
	m_vec_pchar.clear();
	m_vec_len.clear();

	PushPara("HMSET");
	PushPara(key);
	PushPara(field);
	PushPara(value);

	InitRedisArgInfo();
	
}

void RedisArgParse::GetHash(const std::string &key, const std::string &field)
{
	//HGET key field
	m_vec_pchar.clear();
	m_vec_len.clear();

	PushPara("HGET");
	PushPara(key);
	PushPara(field);

	InitRedisArgInfo();

}

void RedisArgParse::GetAllHash(const std::string &key)
{
	//	HGETALL key 
	m_vec_pchar.clear();
	m_vec_len.clear();

	PushPara("HGETALL");
	PushPara(key);

	InitRedisArgInfo();

}

void RedisArgParse::SetZSet(const std::string &key, const std::string &member, int64_t score)
{
	//	ZADD key score1 member1 [score2 member2] 
	m_vec_pchar.clear();
	m_vec_len.clear();

	PushPara("ZADD");
	PushPara(key);
	PushPara(std::to_string(score));
	PushPara(member);

	InitRedisArgInfo();
}

void RedisArgParse::SetZSet(const std::string &key, const redis_com::VecMemberScore &vec_m_s)
{
	//	ZADD key score1 member1 [score2 member2] 
	m_vec_pchar.clear();
	m_vec_len.clear();

	PushPara("ZADD");
	PushPara(key);
	for (const auto &v : vec_m_s)
	{
		PushPara(std::to_string(v.second));
		PushPara(v.first);
	}

	InitRedisArgInfo();
}

void RedisArgParse::GetZSetRange(const std::string &key, uint32_t start_idx, int32_t stop_idx)
{
	//ZRANGE key start stop [WITHSCORES] 
	m_vec_pchar.clear();
	m_vec_len.clear();

	PushPara("ZRANGE");
	PushPara(key);
	PushPara(std::to_string(start_idx));
	PushPara(std::to_string(stop_idx));

	InitRedisArgInfo();
}

void RedisArgParse::GetZSetRevRange(const std::string &key, uint32_t start_idx, int32_t stop_idx)
{
	//ZREVRANGE key start stop [WITHSCORES] 
	m_vec_pchar.clear();
	m_vec_len.clear();

	PushPara("ZREVRANGE");
	PushPara(key);
	PushPara(std::to_string(start_idx));
	PushPara(std::to_string(stop_idx));

	InitRedisArgInfo();
}

void RedisArgParse::GetZSetTotalNum(const std::string &key)
{
	//	ZCARD key 
	m_vec_pchar.clear();
	m_vec_len.clear();

	PushPara("ZCARD");
	PushPara(key);

	InitRedisArgInfo();
}

void RedisArgParse::GetZSetIdx(const std::string &key, const std::string &member)
{
	//ZRANK key member
	m_vec_pchar.clear();
	m_vec_len.clear();

	PushPara("ZRANK");
	PushPara(key);
	PushPara(member);

	InitRedisArgInfo();
}

void RedisArgParse::GetZSetRevIdx(const std::string &key, const std::string &member)
{
	//ZREVRANK key member 
	m_vec_pchar.clear();
	m_vec_len.clear();

	PushPara("ZREVRANK");
	PushPara(key);
	PushPara(member);

	InitRedisArgInfo();
}

void RedisArgParse::GetZSetScore(const std::string &key, const std::string &member)
{
	//ZSCORE key member
	m_vec_pchar.clear();
	m_vec_len.clear();

	PushPara("ZSCORE");
	PushPara(key);
	PushPara(member);

	InitRedisArgInfo();

}

void RedisArgParse::PushList(const std::string &key, const std::string &value)
{
	//LPUSH key value1[value2]
	m_vec_pchar.clear();
	m_vec_len.clear();

	PushPara("LPUSH");
	PushPara(key);
	PushPara(value);

	InitRedisArgInfo();
}

void RedisArgParse::PopList(const std::string &key)
{
	//	RPOP key  
	m_vec_pchar.clear();
	m_vec_len.clear();

	PushPara("RPOP");
	PushPara(key);

	InitRedisArgInfo();
}

void  RedisArgParse::GetStr(const std::string &key)
{
	m_vec_pchar.clear();
	m_vec_len.clear();

	PushPara("get");
	PushPara(key);

	InitRedisArgInfo();
	
}

void RedisArgParse::InitRedisArgInfo()
{
	argc = m_vec_pchar.size();
	argv = &(m_vec_pchar[0]);
	argvlen = &(m_vec_len[0]);

}

/*
解析 client command的功能

*/

#pragma once

#include <vector>
#include <string>


using VecFieldValue = std::vector< std::pair<std::string, std::string> >;
using VecMemberScore = std::vector< std::pair<std::string, uint64_t> >;

struct RedisArgInfo
{
	int argc=0;	//字符串数量
	const char **argv=nullptr;	
	const std::size_t *argvlen = nullptr;
};

//注意：
//为了减少复制，凡是 RedisParseArgv 类型返回值，指针指向的内存，都是输入参数的。 注意别弄野
class RedisArgParse
{
	std::vector<const char *> m_vec_pchar;
	std::vector<std::size_t>       m_vec_len;

public:
	static RedisArgParse &Obj();

	void DelKey(const std::string &key, RedisArgInfo &arg_info);


	//字符串(String)
	//---------------------------------------------------------------------
	void SetStr(const std::string &key, const std::string &value, RedisArgInfo &arg_info);
	void GetStr(const std::string &key, RedisArgInfo &arg_info);

	//哈希(Hash)
	//---------------------------------------------------------------------
	void SetHash(const std::string &key, const VecFieldValue &vec_field_value, RedisArgInfo &arg_info);
	void SetHash(const std::string &key, const std::string &field, const std::string &value, RedisArgInfo &arg_info);
	void GetHash(const std::string &key, const std::string &field, std::string &value, RedisArgInfo &arg_info);
	void GetAllHash(const std::string &key, VecFieldValue &vec_field_value, RedisArgInfo &arg_info);

	//有序集合(sorted set)
	//---------------------------------------------------------------------
	void SetZSet(const std::string &key, const std::string &member, int64_t score, RedisArgInfo &arg_info);
	void SetZSet(const std::string &key, const VecMemberScore &vec_m_s, RedisArgInfo &arg_info);
	//获取区间[start_idx, stop_idx]成员 
	//@start_idx, stop_idx  指定索引
	void GetZSetRange(const std::string &key, uint32_t start_idx, uint32_t stop_idx, VecMemberScore &vec_m_s, RedisArgInfo &arg_info);
	void GetZSetAll(const std::string &key, VecMemberScore &vec_m_s, RedisArgInfo &arg_info);
	void GetZSetIdx(const std::string &key, const std::string &member, RedisArgInfo &arg_info); //获取排名索引。 0开始
	void GetZSetScore(const std::string &key, const std::string &member, RedisArgInfo &arg_info);

	//列表(List)
	//---------------------------------------------------------------------
	void PushList(const std::string &key, const std::string &value, RedisArgInfo &arg_info);
	void PopList(const std::string &key, RedisArgInfo &arg_info);

	//集合(Set)
	//---------------------------------------------------------------------
	void AddSet(const std::string &key, const std::vector<std::string> &vec_value, RedisArgInfo &arg_info);
	void GetSet(const std::string &key, std::vector<std::string> &vec_value, RedisArgInfo &arg_info);

private:
	RedisArgParse() {}
	~RedisArgParse() {}

	void InitRedisArgInfo(RedisArgInfo &arg_info);//根据 m_vec_pchar 初始化 

};
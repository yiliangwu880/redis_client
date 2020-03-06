/*
解析 简单参数格式 成 redis 命令 
*/

#pragma once

#include <vector>
#include <string>


using VecFieldValue = std::vector< std::pair<std::string, std::string> >;
using VecMemberScore = std::vector< std::pair<std::string, uint64_t> >;



//解析 简单参数格式 成 argc argv argvlen 格式
//注意：
//为了减少复制，RedisArgParse 的指针指向的内存，都是输入参数的。 注意别弄野
struct RedisArgParse
{
	std::vector<const char *> m_vec_pchar;
	std::vector<std::size_t> m_vec_len;

	int argc = 0;	//字符串数量
	const char **argv = nullptr;
	const std::size_t *argvlen = nullptr;

	void DelKey(const std::string &key);

	//字符串(String)
	//---------------------------------------------------------------------
	void SetStr(const std::string &key, const std::string &value);
	void GetStr(const std::string &key);

	//哈希(Hash)
	//---------------------------------------------------------------------
	void  SetHash(const std::string &key, const VecFieldValue &vec_field_value);
	void  SetHash(const std::string &key, const std::string &field, const std::string &value);
	void  GetHash(const std::string &key, const std::string &field, std::string &value);
	void  GetAllHash(const std::string &key, VecFieldValue &vec_field_value);

	//有序集合(sorted set)
	//---------------------------------------------------------------------
	void  SetZSet(const std::string &key, const std::string &member, int64_t score);
	void  SetZSet(const std::string &key, const VecMemberScore &vec_m_s);
	//获取区间[start_idx, stop_idx]成员 
	//@start_idx, stop_idx  指定索引
	void  GetZSetRange(const std::string &key, uint32_t start_idx, uint32_t stop_idx, VecMemberScore &vec_m_s);
	void  GetZSetAll(const std::string &key, VecMemberScore &vec_m_s);
	void  GetZSetIdx(const std::string &key, const std::string &member); //获取排名索引。 0开始
	void  GetZSetScore(const std::string &key, const std::string &member);

	//列表(List)
	//---------------------------------------------------------------------
	void  PushList(const std::string &key, const std::string &value);
	void  PopList(const std::string &key);

	//集合(Set)
	//---------------------------------------------------------------------
	void  AddSet(const std::string &key, const std::vector<std::string> &vec_value);
	void  GetSet(const std::string &key, std::vector<std::string> &vec_value);

private:


	void InitRedisArgInfo();//根据 m_vec_pchar m_vec_len初始化 

};
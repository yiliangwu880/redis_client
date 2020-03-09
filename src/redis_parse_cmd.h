/*
解析 简单参数格式 成 redis 命令 
example:
		RedisArgParse arg_info;
		arg_info.SetStr(key, value);
		RedisCon::UNIQUE_PTR r = m_client.Cmd(arg_info.argc, arg_info.argv, arg_info.argvlen);
*/

#pragma once

#include <vector>
#include <string>
#include "redis_com.h"





//把 简单参数格式 转换 argc argv argvlen 格式
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
	void  SetHash(const std::string &key, const redis_com::VecFieldValue &vec_field_value);
	void  SetHash(const std::string &key, const std::string &field, const std::string &value);
	void  GetHash(const std::string &key, const std::string &field);
	void  GetAllHash(const std::string &key);

	//有序集合(sorted set)
	//---------------------------------------------------------------------
	void  SetZSet(const std::string &key, const std::string &member, int64_t score);
	void  SetZSet(const std::string &key, const redis_com::VecMemberScore &vec_m_s);
	//获取区间[start_idx, stop_idx]成员 
	//@start_idx, stop_idx  指定索引 分数从低到高. 0指向开始，-1指向尾端
	void  GetZSetRange(const std::string &key, uint32_t start_idx, int32_t stop_idx);
	//获取区间[start_idx, stop_idx]成员 
	//@start_idx, stop_idx  指定索引 分数从高到低. 0指向开始，-1指向尾端
	void  GetZSetRevRange(const std::string &key, uint32_t start_idx, int32_t stop_idx);
	void  GetZSetTotalNum(const std::string &key); //获取有序集合的成员数
	//	返回有序集合中指定成员的索引, 从低到高排序
	void  GetZSetIdx(const std::string &key, const std::string &member);
	//	返回有序集合中指定成员的索引, 从高到低排序
	void  GetZSetRevIdx(const std::string &key, const std::string &member);
	//返回有序集中，成员的分数值
	void  GetZSetScore(const std::string &key, const std::string &member);

	//列表(List)
	//---------------------------------------------------------------------
	void  PushList(const std::string &key, const std::string &value);	//将一个或多个值插入到列表头部
	void  PopList(const std::string &key);	//移除列表的最后一个元素，返回值为移除的元素。

private:

	inline void PushPara(const std::string &para)
	{
		m_vec_pchar.push_back(para.c_str());
		m_vec_len.push_back(para.length());
	}
	void InitRedisArgInfo();//根据 m_vec_pchar m_vec_len初始化 

};
/*
redis_client.h基础，实现大多数命令的简单接口。
推荐用户直接用这个接口
这里的string支持任意二进制数据。
*/


#pragma once

#include "redis_client.h"
#include <list>
#include <vector>

namespace redis_com
{
	using VecFieldValue = std::vector< std::pair<std::string, std::string> >;
	using VecMemberScore = std::vector< std::pair<std::string, int64_t> >;
	//阻塞同步方式
	class RedisCom
	{
	public:
		RedisCon m_client;//获取RedisCon对象，用户就可以操作原生redis client命令.

	public:
		bool Init(const std::string &ip, uint16_t port, uint32_t wait_sec = 5);


		bool DelKey(const std::string &key);

		//字符串(String)
		//---------------------------------------------------------------------
		bool SetStr(const std::string &key, const std::string &value);
		//失败返回空字符串
		std::string GetStr(const std::string &key);

		//哈希(Hash)
		//---------------------------------------------------------------------
		void  SetHash(const std::string &key, const redis_com::VecFieldValue &vec_field_value);
		void  SetHash(const std::string &key, const std::string &field, const std::string &value);
		void  GetHash(const std::string &key, const std::string &field, std::string &value);
		void  GetAllHash(const std::string &key, redis_com::VecFieldValue &vec_field_value);

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


	};

	class RedisAsynCom;
	//适配RedisAsynCon用，避免RedisAsynCom多层继承带来的复杂问题。
	class RedisAsynConAdapter : public RedisAsynCon
	{
		RedisAsynCom &m_com;
	public:
		RedisAsynConAdapter(RedisAsynCom &com);

	private:
		virtual void OnCon();
		virtual void OnDisCon();
		virtual void OnRev(const redisReply *reply, void *privdata); //reply 回调函数结束后，自动释放

	};

	//异步访问方法，用来区分 RedisAsynCom 不同函数回调
	//RAM= redis asyn method
	enum class RAM
	{
		None,
		DelKey,
		SetStr,
		GetStr,
		SetHash,
		GetHash,
		GetAllHash,
	};

	//记录每个请求的信息。
	struct ReqInfo
	{
		uint64_t id = 0; //ReqInfo  唯一标识
		RAM method = RAM::None;
		void *privdata = nullptr;//用户的自定义参数
	};

	//libevent异步方式
	class RedisAsynCom
	{
		friend class RedisAsynConAdapter;
	private:
		RedisAsynConAdapter m_client;
		std::list<ReqInfo>  m_req_info_list; //记录每个请求的信息, 等接受响应的时候获取信息。
		uint64_t            m_id_seed = 0;

	public:
		RedisAsynCom();
		virtual ~RedisAsynCom() {};
		bool Init(event_base *base, const std::string &ip, uint16_t port);
		void Dicon();

		//测试用，缓存 ReqInfo 过大表示可能泄漏内存了。
		size_t GetReqInfoSize() const;

		//@privdata 用户自定义数据，不推荐传指针，生存期难控制。
		bool Command(void *privdata, const char *format, ...);
		bool CommandArgv(void *privdata, int argc, const char **argv, const size_t *argvlen);

		bool DelKey(void *privdata, const std::string &key);

		//字符串(String)
		//---------------------------------------------------------------------
		bool SetStr(void *privdata, const std::string &key, const std::string &value);
		bool GetStr(void *privdata, const std::string &key);

		//哈希(Hash)
		//---------------------------------------------------------------------
		bool  SetHash(void *privdata, const std::string &key, const redis_com::VecFieldValue &vec_field_value);
		bool  SetHash(void *privdata, const std::string &key, const std::string &field, const std::string &value);
		bool  GetHash(void *privdata, const std::string &key, const std::string &field);
		bool  GetAllHash(void *privdata, const std::string &key);

	private:
		virtual void OnCon() = 0;
		virtual void OnDisCon() = 0;

		virtual void OnDelKey(void *privdata, bool is_success) {};
		virtual void OnSetStr(void *privdata, bool is_success) {};
		virtual void OnGetStr(void *privdata, const std::string &value) {};
		virtual void OnSetHash(void *privdata, bool is_success) {};
		virtual void OnGetHash(void *privdata, const std::string &value) {};
		virtual void OnGetAllHash(void *privdata, const redis_com::VecFieldValue &vec_field_value) {};

		uint32_t PushReqInfo(void *privdata, RAM method);//返回新加req info id
		bool PopReqInfo(uint64_t id, ReqInfo &req_info);

	};
}